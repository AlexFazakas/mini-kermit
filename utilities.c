#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Aici avem corpurile tuturor functiilor ajutatoarea. In principal,
 * planuiam sa scriu toate tipurile de mesaje direct in campul
 * payload al structurii msg.
 */

char* split_crc (unsigned short crc)
{
    /* Functie care imparte suma de control (un word, 2 octeti) in 2 char-uri,
     * fiecare avand un octet. In digits, rezultatul este memorat UPPER HALF,
     * LOWER HALF. Presupunem ca sistemul este little endian.
     */    
    char *digits;

    digits = malloc (2 * sizeof (char));
    digits[0] = crc >> 8;
    digits[1] = crc;

    return digits;
}

void create_send_init (msg *message)
{
    /* Functie care creeaza un packet de tipul Send-Init. */
    unsigned short check;
    char *crc_digits;

    message->payload[SOH_POS] = 1; // SOH
    message->payload[LEN_POS] = 10; // LEN
    message->payload[SEQ_POS] = 1; // SEQ
    message->payload[TYPE_POS] = 'S'; // TYPE
    // Start of data field
    message->payload[DATA_POS] = 250; // MAXL
    message->payload[5] = 5; // TIME
    message->payload[6] = 0; // NPAD
    message->payload[7] = 0; // PADC
    message->payload[8] = 14; // EOL
    message->payload[9] = 0; // QCTL
    message->payload[10] = 0; // QBIN
    message->payload[11] = 0; // CHKT
    message->payload[12] = 0; // REPT
    message->payload[13] = 0; // CAPA
    message->payload[14] = 0; // R
    // End of data field

    check = crc16_ccitt (message->payload, 15);
    crc_digits = split_crc (check);

    message->payload[15] = crc_digits[0];
    message->payload[16] = crc_digits[1];
    free (crc_digits);
    /* Am impartit suma de control in upper si lower half, astfel avem
     * partea superioara pe pozitia 9 in payload, iar cea inferioara pe
     * pozitia 10 in payload.
     */

    message->payload[17] = 14; //MARK

    message->len = 18;
}

void create_file_header (msg *message, int current_seq, char *file_name)
{
    /* Functie care creeaza un packet de tipul File Header. */
    unsigned short check;
    char *crc_digits;
    int file_name_length;

    file_name_length = strlen (file_name);
    message->payload[SOH_POS] = 1; // SOH
    message->payload[LEN_POS] = file_name_length + 1; // LEN
    message->payload[SEQ_POS] = current_seq; // SEQ
    message->payload[TYPE_POS] = 'F'; // TYPE
    // Start of data field
    /* In functie de lungimea numelui fisierului, de acolo completam
     * restul campurilor.
     */
    strcpy (message->payload + 4, file_name);
    message->payload [4 + file_name_length] = 0;
    // End of data field

    check = crc16_ccitt (message->payload, 5 + file_name_length);
    crc_digits = split_crc (check);

    message->payload[5 + file_name_length] = crc_digits[0];
    message->payload[6 + file_name_length] = crc_digits[1];

    free (crc_digits);
    /* Am impartit suma de control in upper si lower half, astfel avem
     * partea superioara pe pozitia 9 in payload, iar cea inferioara pe
     * pozitia 10 in payload.
     */

    message->payload[7 + file_name_length] = 14; // MARK

    message->len = 8 + file_name_length;
}

void create_data (msg *message, int current_seq, char data_size, char *data)
{
    /* Functie care creeaza un packet de tipul Date. */
    unsigned short check;
    char *crc_digits;

    message->payload[SOH_POS] = 1; // SOH
    message->payload[LEN_POS] = data_size; // LEN
    message->payload[SEQ_POS] = current_seq; // SEQ
    message->payload[TYPE_POS] = 'D'; // TYPE
    // Start of data field
    /* In functie de lungimea dimensiunea datelor, de acolo completam
     * restul campurilor.
     */
    for (int i = 0; i < data_size; i++)
        message->payload[4 + i] = data[i];
    // End of data field

    check = crc16_ccitt (message->payload, 4 + data_size);
    crc_digits = split_crc (check);

    message->payload[4 + data_size] = crc_digits[0];
    message->payload[5 + data_size] = crc_digits[1];
    free (crc_digits);
    /* Am impartit suma de control in upper si lower half, astfel avem
     * partea superioara pe pozitia 9 in payload, iar cea inferioara pe
     * pozitia 10 in payload.
     */

    message->payload[6 + data_size] = 14; // MARK

    message->len = 8 + data_size;
}

void create_eof (msg *message, int current_seq)
{
    /* Functie care creeaza un packet de tipul EOF. */
    unsigned short check;
    char *crc_digits;

    message->payload[SOH_POS] = 1; // SOH
    message->payload[LEN_POS] = 0; // LEN
    message->payload[SEQ_POS] = current_seq; // SEQ
    message->payload[TYPE_POS] = 'Z'; // TYPE
    // Start of data field
    // End of data field
    check = crc16_ccitt (message->payload, 4);
    crc_digits = split_crc (check);

    message->payload[5] = crc_digits[0];
    message->payload[6] = crc_digits[1];
    free (crc_digits);
    /* Am impartit suma de control in upper si lower half, astfel avem
     * partea superioara pe pozitia 9 in payload, iar cea inferioara pe
     * pozitia 10 in payload.
     */

    message->payload[7] = 14; // MARK

    message->len = 8;
}

void create_eot (msg *message, int current_seq)
{
    /* Functie care creeaza un packet de tipul EOT. */
    unsigned short check;
    char *crc_digits;

    message->payload[SOH_POS] = 1; // SOH
    message->payload[LEN_POS] = 0; // LEN
    message->payload[SEQ_POS] = current_seq; // SEQ
    message->payload[TYPE_POS] = 'B'; // TYPE
    // Start of data field
    // End of data field
    check = crc16_ccitt (message->payload, 4);
    crc_digits = split_crc (check);

    message->payload[5] = crc_digits[0];
    message->payload[6] = crc_digits[1];
    free (crc_digits);
    /* Am impartit suma de control in upper si lower half, astfel avem
     * partea superioara pe pozitia 9 in payload, iar cea inferioara pe
     * pozitia 10 in payload.
     */

    message->payload[7] = 14; // MARK

    message->len = 8;
}

void create_ack_for_send (msg *message)
{
    /* Functie care creeaza un packet de tipul ACK, drept raspuns
     * pentru un send init. Vom schimba doar campurile necesare.
     */
    unsigned short check;
    char *crc_digits;

    message->payload[SEQ_POS] = 1; // SEQ
    message->payload[TYPE_POS] = 'Y'; // TYPE

    check = crc16_ccitt (message->payload, 15);
    crc_digits = split_crc (check);

    message->payload[15] = crc_digits[0];
    message->payload[16] = crc_digits[1];
    free (crc_digits);
    /* Am impartit suma de control in upper si lower half, astfel avem
     * partea superioara pe pozitia 9 in payload, iar cea inferioara pe
     * pozitia 10 in payload.
     */

    message->len = 18;
}

void create_ack (msg *message, int current_seq)
{
    /* Functie care creeaza un packet de tipul ACK, nu drept raspuns
     * pentru SEND INIT. 
     */
    unsigned short check;
    char *crc_digits;

    message->payload[SOH_POS] = 1; // SOH
    message->payload[LEN_POS] = 0; // LEN
    message->payload[SEQ_POS] = 0; // SEQ
    message->payload[TYPE_POS] = 'Y'; // TYPE
    // End of data field

    check = crc16_ccitt (message->payload, 4);
    crc_digits = split_crc (check);

    message->payload[5] = crc_digits[0];
    message->payload[6] = crc_digits[1];
    free (crc_digits);
    /* Am impartit suma de control in upper si lower half, astfel avem
     * partea superioara pe pozitia 9 in payload, iar cea inferioara pe
     * pozitia 10 in payload.
     */

    message->payload[7] = 14; //MARK

    message->len = 8;
}

void create_nak (msg *message, int current_seq)
{
    /* Functie care creeaza un packet de tipul NAK. */
    unsigned short check;
    char *crc_digits;

    message->payload[SOH_POS] = 1; // SOH
    message->payload[LEN_POS] = 0; // LEN
    message->payload[SEQ_POS] = 0; // SEQ
    message->payload[TYPE_POS] = 'N'; // TYPE
    // End of data field

    check = crc16_ccitt (message->payload, 4);
    crc_digits = split_crc (check);

    message->payload[5] = crc_digits[0];
    message->payload[6] = crc_digits[1];
    free (crc_digits);
    /* Am impartit suma de control in upper si lower half, astfel avem
     * partea superioara pe pozitia 9 in payload, iar cea inferioara pe
     * pozitia 10 in payload.
     */

    message->payload[7] = 14; //MARK

    message->len = 8;
}

void increment_sequence (char *current_seq)
{
    /* Functie care incrementeaza corespunzator sequence number. */
    *current_seq =  *current_seq + 1;
    *current_seq = *current_seq % 64;
}

int check_crc (msg *message)
{
    /* Aceasta functie verifica daca un pachet a fost corupt. */
    unsigned short upper;
    unsigned short lower;
    unsigned short result;
    unsigned short checksum;

    /* Preluam cifrele crc-ului din pachet conform tipului acestuia. */
    if (message->payload[TYPE_POS] == 'F')
    {
        upper = message->payload[message->payload[LEN_POS] + 4];
        lower = message->payload[message->payload[LEN_POS] + 5];
    }
    else if (message->payload[TYPE_POS] == 'D')
    {
        upper = message->payload[message->payload[LEN_POS] + 3];
        lower = message->payload[message->payload[LEN_POS] + 4];
    }
    else if (message->payload[TYPE_POS] == 'S')
    {
        upper = message->payload[message->payload[LEN_POS] + 4];
        lower = message->payload[message->payload[LEN_POS] + 5];
    }
    else
    {
        upper = message->payload[5];
        lower = message->payload[6];
    }

    /* Shiftari pentru a fi siguri ca nu apar valori aiurea. */
    upper = upper << 8;
    lower = lower << 8;
    lower = lower >> 8;
    /* Obtinem rezultatul din UPPER si LOWER. */
    result = upper + lower;
    /* Si il comparam cu checksum-ul calculat la destinatie. */
    checksum = crc16_ccitt (message->payload, message->payload[LEN_POS] + 4);

    return (result == checksum);
}

int send_file (FILE *send_file, char *current_seq, msg *sent, msg *received)
{
    /* Aceasta functie se ocupa de trimiterea fisierelor, cate 128 de
     * octeti per pachet. */
    char *file_buffer;
    char read_bytes;

    file_buffer = malloc (251 * sizeof (char));
    /* Citim un grup de caractere */
    while ((read_bytes = fread (file_buffer, 1, 125, send_file)))
    {
        /* Cream si trimitem un pachet de tipul Data */
        create_data (sent, *current_seq, read_bytes, file_buffer);
        send_message (sent);
        received = receive_message_timeout (5000);

        /* Verificam daca acesta s-a trimis corect si daca s-a primit ACK. */
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
        /* Daca trimiterea esueaza de 3 ori, iesim. Returnam -1 pentru a sti
         * ca ar trebui sa iesim si din functia main.
         */
        if (received == NULL || received->payload[TYPE_POS] == 'N')
        {
            return -1;
        }
        if (received->payload[TYPE_POS] == 'Y')
        {
            increment_sequence (current_seq);
        }
    }

    /* Dupa fiecare fisier transmis, trimitem un pachet EOF. */
    create_eof (sent, *current_seq);
    send_message (sent);
    received = receive_message_timeout (5000);

    /* Verificam ca a fost trimis bine. */
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
        return -1;
    }
    if (received->payload[TYPE_POS] == 'Y')
    {
        increment_sequence (current_seq);
    }

    return 0;
}

int write_file (FILE *to_write, char *current_seq, msg *sent, msg *received)
{
    /* Functie care se ocupa de scrierea in fisier a pachetelor de tip Date
     * sau EOF primite.
     */

    char *file_name = malloc (100 * sizeof (char));

    /* Aici cream string-ul de tip recv_NUME-FISIER */
    sprintf (file_name, "recv_%s", received->payload + 4);

    to_write = fopen (file_name, "wb");


    while (1)
    {
        /* Primim un pachet de tip Date. */
        received = receive_message_timeout (5000);
        /* Printf for easier debugging */
        printf ("Receiver: type = %c, seq: 0x%02x\n", received->payload[TYPE_POS],
                                                      received->payload[SEQ_POS]);
        if (received == NULL)
        {
            send_message (sent);
            received = receive_message_timeout (5000);

            if (received == NULL)
            {
                send_message (sent);
                received = receive_message_timeout (5000);
            }

            if (received == NULL)
            {
                return -1;
            }
        }
        /* Daca suma de control nu corespune, trimitem NAK */
        if (!check_crc (received))
        {
            increment_sequence (current_seq);
            create_nak (sent, *current_seq);
            send_message (sent);
            received = receive_message_timeout (5000);

            if (!check_crc (received))
            {
                increment_sequence (current_seq);
                create_nak (sent, *current_seq);
                send_message (sent);
                received = receive_message_timeout (5000);
            }

            if (!check_crc (received))
            {
                return -1;
            }
        }

        /* Daca primim un pachet de tip date, il vom scrie in fisier si apoi
         * incercam sa trimitem un ACK.
         */
        if (received->payload[TYPE_POS] == 'D')
        {
            printf ("xd\n");
            increment_sequence (current_seq);
            create_ack (sent, *current_seq);
            fwrite (received->payload + 4, 1, received->payload[LEN_POS], to_write);
            received = receive_message_timeout (5000);

            if (received == NULL)
            {
                send_message (sent);
                received = receive_message_timeout (5000);

                if (received == NULL)
                {
                    send_message (sent);
                    received = receive_message_timeout (5000);
                }

                if (received == NULL)
                {
                    return -1;
                }
            }
        }
        /* Daca pachetul primit este de tip EOF, inchidem fisierul deschis
         * si trimiem ACK.
         */
        else if (received->payload[TYPE_POS] == 'Z')
        {
            increment_sequence (current_seq);
            create_ack (sent, *current_seq);
            fclose (to_write);
            received = receive_message_timeout (5000);

            if (received == NULL)
            {
                send_message (sent);
                received = receive_message_timeout (5000);

                if (received == NULL)
                {
                    send_message (sent);
                    received = receive_message_timeout (5000);
                }

                if (received == NULL)
                {
                    return -1;
                }
            }

            return 0;
        }
    }

    return 0;
}