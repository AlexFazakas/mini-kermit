#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lib.h"
#include "utilities.h"

#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc, char** argv)
{
    msg *sent, *received;
    char current_seq = 0;
    FILE *to_send;

    init(HOST, PORT);
    /* Trimitem separat mesajul de tip SEND INIT */
    sent = malloc (sizeof (msg));
    received = malloc (sizeof (msg));

    create_send_init (sent);
    send_message (sent);

    /* Il retrimitem de fiecare data cand nu primim un raspuns, daca se
     * intampla de 3 ori, renuntam.
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

    increment_sequence (&current_seq);

    for (size_t i = 1; i < argc; i++)
    {
        to_send = fopen (argv[1], "rb");

        /* Pentru fiecare fisier primit ca argument, trimitem intai numele. */
        create_file_header (sent, current_seq, argv[1]);
        send_message (sent);
        received = receive_message_timeout (5000);

        if (received == NULL || received->payload[TYPE_POS] == 'N')
        {
            send_message (sent);
            received = receive_message_timeout (5000);
        }
        if (received == NULL || received->payload[TYPE_POS] == 'N')
        {
            send_message (sent);
            received = receive_message_timeout (5000);
        }
        if (received == NULL || received->payload[TYPE_POS] == 'N')
        {
            return 0;
        }
        if (received->payload[TYPE_POS] == 'Y')
        {
            /* Daca acesta a fost primit cu succes, putem continua cu transmiterea continutului
             * acestuia, cate 128 de octeti per transmisie.
             */
            increment_sequence (&current_seq);
            if (send_file (to_send, &current_seq, sent, received) == -1)
                return 0;
        }
    }
    /* Trimitem mesajul de tip end of transmission. */
    create_eot (sent, current_seq);
    send_message (sent);
    received = receive_message_timeout (5000);
    if (received == NULL || received->payload[TYPE_POS] == 'N')
    {
        send_message (sent);
        received = receive_message_timeout (5000);
    }
    if (received == NULL || received->payload[TYPE_POS] == 'N')
    {
        send_message (sent);
        received = receive_message_timeout (5000);
    }
    if (received == NULL || received->payload[TYPE_POS] == 'N')
    {
        return 0;
    }
    if (received->payload[TYPE_POS] == 'Y')
    {
        increment_sequence (&current_seq);
    }

    return 0;
}
