#include <iostream>
#include "Serial.h"
#include "Timer.h"
#include "Decoder.h"

#define PICK_COM true
#define SERIAL_BUFFER_SIZE 1024u
// Grid encoding (256 x 256) = 8480 bytes
#define MESSAGE_BUFFER_SIZE 10000u
#define EMPTY_BUFFER_MAX_TICKS 5

int main()
{
    std::cout << "Hello World!\n";



    // Open port
    Serial* port;
#if PICK_COM
    std::string portname;
    while (1)
    {
        std::cout << "Port: ";
        std::cin >> portname;
        std::cout << "Connecting..." << std::endl;
        port = new Serial(("\\\\.\\" + portname).c_str());
        if (port->IsConnected())
        {
            std::cout << "Connected successfully" << std::endl;
            break;
        }
        std::cout << "\nFailed to open " << portname << "\nPlease try again..." << std::endl;
    }
#else
    std::cout << "Connecting..." << std::endl;
    port = new Serial("\\\\.\\com7");
    if (!port->IsConnected())
    {
        std::cout << "Failed to open COM7" << std::endl;
        return 0xBAADDA7E;
    }
    std::cout << "Connected successfully" << std::endl;
#endif

    char serial_buffer[SERIAL_BUFFER_SIZE] = "";
    char msg_buffer[MESSAGE_BUFFER_SIZE] = "";
    unsigned int msg_len = 0;
    unsigned int sbuf_len = 0;

    // Flush the buffer
    do sbuf_len = port->ReadData(serial_buffer, SERIAL_BUFFER_SIZE - 1);
    while (sbuf_len > 0);

    bool msg_started = false;
    short int exit_ticker = -1;
    // Run
    while(1)
    {
        sbuf_len = port->ReadData(serial_buffer, SERIAL_BUFFER_SIZE - 1); // constant polling is probably fiiiiine
        serial_buffer[sbuf_len] = 0;
        if (sbuf_len > 0)
        {
            exit_ticker = 0; // activate/reset exit ticker
            strcat(msg_buffer, serial_buffer);
            msg_len += sbuf_len;
            memset(serial_buffer, 0, sbuf_len); // clear the Sbuf so it doesn't mess up the next set of info recieved
        }
        // PRIMARY EXIT CONDITION
        if (sbuf_len == 0 && exit_ticker > -1)
        {
            exit_ticker++;
            if (exit_ticker > EMPTY_BUFFER_MAX_TICKS)
                break;
        }

        // ADDITIONAL EXIT CONDITIONS:
        if (!port->IsConnected()) // COM port disconnected
        {
            throw std::runtime_error("Connection lost");
            break;
        }
        if (!(GetAsyncKeyState(VK_ESCAPE) && 1 << (8 * sizeof(SHORT) - 1)))// if escape pressed, exit
        {
            break;
        }
    }; 
    // one-shot program, don't need to worry about resetting the above loop environment

    Decoder::decode(msg_buffer, msg_len);

    delete port;
    return 0;
}
