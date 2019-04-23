#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "lib.h"
#include "utilities.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc, char** argv)
{
    msg *sent, *received;
    char current_seq = 1;
    FILE *to_write = NULL;

    init(HOST, PORT);

    sent = malloc (sizeof (msg));
    received = malloc (sizeof (msg));

    /* Primirea mesajului de tip SEND INIT si trimiterea ACK-ului
     * corespunzator.
     */
    received = receive_message_timeout (5000);
    if (received == NULL)
    {
        received = receive_message_timeout (5000);
    }

    if (received == NULL)
    {
        received = receive_message_timeout (5000);
    }

    if (received == NULL)
    {
        return 0;
    }

    create_ack_for_send (sent);
    send_message (sent);
    increment_sequence (&current_seq);
    while (1)
    {
        received = receive_message_timeout (5000);
        if (received == NULL)
        {
            received = receive_message_timeout (5000);
        }
        if (received == NULL)
        {
            received = receive_message_timeout (5000);
        }
        if (received == NULL)
        {
            return 0;
        }
        if (received->payload[TYPE_POS] == 'F')
        {
            /* Pentru fiecare pachet de tip File Header, deschidem fisierul
             * cu numele trimis, si apelam write_file care se va ocupa de
             * scrierea efectiva a fisierului.
             */
            increment_sequence (&current_seq);
            if (check_crc (received))
            {
                create_ack (sent, current_seq);
                send_message (sent);
                if (write_file (to_write, &current_seq, sent, received) == -1)
                    return 0;
            }
            else
            {
                create_nak (sent, current_seq);
                send_message (sent);
            }
        }
        /* Daca este primit pachet de tip End of transmission, inchidem
         * receiver-ul si trimitem ACK.
         */
        if (received->payload[TYPE_POS] == 'B')
        {
            create_ack (sent, current_seq);
            send_message (sent);
            break;
        }
        
    }
	return 0;
}
