#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 1024

int parseMess(char *mess, int *opcode, int *length, char *payload);
char *makeMessage(int opcode, int length, char* payload);

#endif