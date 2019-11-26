#include "message.h"

/*  int parseMess(char *mess, int *opcode, int *length, char *payload)
    ---------------------------------------------------------------------------
    TODO   : > parse a message and return opcode, length, payload 
    ---------------------------------------------------------------------------
    INPUT  : - char *mess		[Message will be parse]
    		 - int *opcode 		[Save opcode]
    		 - int *length 		[Save length]
    		 - char *payload	[Save payload]
    OUTPUT : + return 0			[Parse success]
*/
int parseMess(char *mess, int *opcode, int *length, char *payload) {
    char temp_str[5];
    memcpy(temp_str, mess, 1);
    temp_str[1] = '\0';
    *opcode = atoi(temp_str);

    memcpy(temp_str, mess+1, 4);
    temp_str[4] = '\0';
    *length = atoi(temp_str);

    memcpy(payload, mess+5, *length);
    return 0;
}

/*  char *makeMessage(int opcode, int length, char* payload)
    ---------------------------------------------------------------------------
    TODO   : > Make a message to send  
    ---------------------------------------------------------------------------
    INPUT  : - int opcode       [Opcode of message]
             - int length       [Length of message]
             - char *payload        [Pointer of payload]
    OUTPUT : + return message         [return a message]
*/
char *makeMessage(int opcode, int length, char* payload)
{
    char* message = malloc(BUFF_SIZE+5);
    bzero(message, BUFF_SIZE+5);
    sprintf(message, "%d%04d", opcode, length);
    memcpy(message+5, payload, length);
    return message; 
}