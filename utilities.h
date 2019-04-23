#pragma once

#include "lib.h"
#include <stdio.h>

/* Macro-uri pentru a sti pozitiile acestor campuri cu usurinta. */
#define SOH_POS 0
#define LEN_POS 1
#define SEQ_POS 2
#define TYPE_POS 3
#define DATA_POS 4

/* Biblioteca ce imi permite obtinerea unor pachete predefinite
 * folosind cateva functii de ajutor
 */

char* split_crc (unsigned short crc);
void create_send_init (msg *message);
void create_file_header (msg *message, int current_seq, char *file_name);
void create_data (msg *message, int current_seq, char data_size, char *data);
void create_eof (msg *message, int current_seq);
void create_eot (msg *message, int current_seq);
void create_ack_for_send (msg *message);
void create_ack (msg *message, int current_seq);
void create_nak (msg *message, int current_seq);
void increment_sequence (char *sequence);
int check_crc (msg *message);
int send_file (FILE *to_send, char *current_seq, msg *sent, msg *received);
int write_file (FILE *to_write, char *current_seq, msg *sent, msg *received);
