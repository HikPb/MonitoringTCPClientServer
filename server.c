#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> 
#include "search.h"
#include "message.h"
#include "createfile.h"

#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define NAME_SIZE 256
#define FOLDER_INFO "info"
#define FOLDER_PROCESSING "processing"
#define FOLDER_RESULT "result"
#define FOLDER_IMG "images"
#define FOLDER_LOG "logs"
#define WAIT 0
#define FINISH 1
#define KEY_INFO "infomation"
#define KEY_PROCESSING "process_info"
#define KEY_KBMS "keyboard_mouse_operations"
#define KEY_IMAGE "image"

fd_set	readfds, allset, writefds;
int nready, client[FD_SETSIZE], maxi, time_wait = 10;

/*  int sendTime(int sockfd, int *time_wait)
    ---------------------------------------------------------------------------
    TODO   : > Send time wait to client.
    ---------------------------------------------------------------------------
    INPUT  : - int sockfd       	[Socket file descriptor of client]
    		 - int *time_wait      	[Poiter of time wait]
    OUTPUT : + return ret        	[return bytes sent]
*/
int sendTime(int sockfd, int *time_wait) {
	char *mess;
	int ret;
	char timeWait[5];
	sprintf(timeWait, "%d", *time_wait);
	mess = makeMessage(4, strlen(timeWait), timeWait);
	ret = send(sockfd, mess, BUFF_SIZE + 5, 0);
	free(mess);
	return ret;
}


/*  void sendTimeWait()
    ---------------------------------------------------------------------------
    TODO   : > Send time wait to all client.
    ---------------------------------------------------------------------------
*/
void sendTimeWait() {
	int sockfd;
	int ret, t, i;
	char str[BUFF_SIZE];
	printf("Enter time wait (s): ");
	while(1) {
		if (scanf("%[^0-9]%d",str,&t) > 0) {
			time_wait = t;
			printf("Successful change!\n");
			break;
		} else printf("Re-enter: ");
	}

	for (i = 0; i <= maxi; i++) {
		if ( (sockfd = client[i]) < 0)
			continue;
		if (FD_ISSET(sockfd, &allset)) {
			ret = sendTime(sockfd, &time_wait);
			if (ret <= 0){
				FD_CLR(sockfd, &allset);
				close(sockfd);
				client[i] = -1;
			}
		}
		if (--nready <= 0)
			break;		/* no more readable descriptors */
	}
}

/*  int parseMess(char *mess, int *opcode, int *length, char *payload)
    ---------------------------------------------------------------------------
    TODO   : > Processing the received message(str) and save to file 
    ---------------------------------------------------------------------------
    INPUT  : - ClientInfo* cli_info		[Current IP'client]
    		 - char *mess 				[Message recive from client]
    OUTPUT : + return 0					[Process success]
    		 + return -1				[Can't open file]
    		 + return -2				[Can't set path of result file]
    		 + return -3				[Can't add datetime property to JSON object]
    		 + return -4				[Can't save JSON to file]
    		 + return -5 				[Client error]
*/
int processData(ClientInfo* cli_info, char *mess)
{
    char payload[BUFF_SIZE];
    int opcode;
    int length;
    FILE *fp;
    parseMess(mess, &opcode, &length, payload);
    switch(cli_info->status) {
    	case WAIT:
    		switch(opcode) {
        		case 0:
        			if (length != 0) {
        				if ((fp = fopen(cli_info->tmp_info, "a+")) == NULL) {
							printf("Can't open file client's infomation.\n");
							return -1;
						}
						fwrite(payload, 1, length, fp);
			            fclose(fp);
        			} else {
        				if (setPath(cli_info, KEY_INFO, "txt", FOLDER_INFO, cli_info->tmp_info) != 0) {
        					fprintf(stderr, "Setting info is wrong.\n");
        					return -2;
        				}
        			}
		            break;

		        case 1:
        			if (length != 0) {
        				if ((fp = fopen(cli_info->tmp_processing, "a+")) == NULL) {
							printf("Can't open file client's infomation.\n");
							return -1;
						}
						fwrite(payload, 1, length, fp);
			            fclose(fp);
        			} else {
        				if (setPath(cli_info, KEY_PROCESSING, "txt", FOLDER_PROCESSING, cli_info->tmp_processing) != 0) {
        					fprintf(stderr, "Setting info is wrong.\n");
        					return -2;
        				}
        			}

		            break;

		        case 2:
        			if (length != 0) {
		        		if ((fp = fopen(cli_info->tmp_operation, "a+")) == NULL) {
		        			printf("Can't open file client's mouse and keyboard event\n");
							return -1;
		        		}
		        		fwrite(payload, 1, length, fp);
		        		fclose(fp);
		        	} else {
		        		if (setPath(cli_info, KEY_KBMS, "txt", FOLDER_LOG, cli_info->tmp_operation) != 0) {
        					fprintf(stderr, "Setting info is wrong.\n");
        					return -2;
        				}
		        	}
		        	break;

		        case 3:
		        	if (length != 0) {
		        		if ((fp = fopen(cli_info->tmp_image, "ab+")) == NULL) {
		        			printf("Can't open file client's image.\n");
							return -1;
		        		}
		        		fwrite(payload, 1, length, fp);
		        		fclose(fp);
		        	} else {
		        		if (setPath(cli_info, KEY_IMAGE, "png", FOLDER_IMG, cli_info->tmp_image) != 0) {
        					fprintf(stderr, "Setting info is wrong.\n");
        					return -2;
        				}

        				if (setDatetime(cli_info) != 0) {
        					fprintf(stderr, "Can't set datetime.\n");
		        			return -3;
        				}

		        		if (saveJsonToFile(cli_info) != 0) {
		        			fprintf(stderr, "Can't save json.\n");
		        			return -4;
		        		}
		        	}
		        	break;
		        case 5:
		        	return -5;
	        }
    		break;
    	case FINISH:
    		break;
    }
    return 0;
}

/*  void *showMenu(void *arg)
    ---------------------------------------------------------------------------
    TODO   : > Show menu
    ---------------------------------------------------------------------------
*/
void *showMenu(void *arg) {
	int choose;
	pthread_detach(pthread_self());
	while(1) {
		init();
		printf("1. Change time (Current: %ds).\n2. Search by IP\n3. Search by date\nChoose: ", time_wait);
		scanf("%d", &choose);	
		switch(choose) {
			case 1: 
				sendTimeWait();
				break;
			case 2:
				searchByIp();
				break;
			case 3:
				searchTime();
				break;
		}
		freeList();
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("The argument is error.\n");
		return 1;
	}

	int i, maxfd, listenfd, connfd, sockfd, process_status;

	ssize_t	ret;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	ClientInfo Client[FD_SETSIZE];
	char rcvBuff[BUFF_SIZE + 5];
	int port = atoi(argv[1]);
	pthread_t tid; 
	//Step 1: Construct a TCP socket to listen connection request
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);

	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	} 

	//Step 3: Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	maxfd = listenfd;			/* initialize */
	maxi = -1;				/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	FD_SET(STDIN_FILENO, &allset);
	FD_SET(STDOUT_FILENO, &allset);
	
	pthread_create(&tid, NULL, showMenu, client);
	//Step 4: Communicate with clients
	while (1) {
		readfds = allset;		/* structure assignment */
		writefds = allset;
		nready = select(maxfd+1, &readfds, &writefds, NULL, NULL);
		if(nready < 0){
			perror("\nError: ");
			return 0;
		}
		
		if (FD_ISSET(listenfd, &readfds)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0)
				perror("\nError: ");
			else{
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0) {
						client[i] = connfd;	/* save descriptor */
						break;
					}
				if (i == FD_SETSIZE){
					printf("\nToo many clients");
					close(connfd);
				}

				FD_SET(connfd, &allset);	/* add new descriptor to set */
				if (connfd > maxfd)
					maxfd = connfd;		/* for select */
				if (i > maxi)
					maxi = i;		/* max index in client[] array */

				Client[i].status = 0;
				strcpy(Client[i].ip_address, inet_ntoa(cliaddr.sin_addr));
				sprintf(Client[i].result,"%s/%s.txt",FOLDER_RESULT, inet_ntoa(cliaddr.sin_addr));
				sprintf(Client[i].tmp_info,"%s/%s.txt",FOLDER_INFO, inet_ntoa(cliaddr.sin_addr));
				sprintf(Client[i].tmp_image,"%s/%s.png",FOLDER_IMG, inet_ntoa(cliaddr.sin_addr));
				sprintf(Client[i].tmp_operation,"%s/%s.txt",FOLDER_LOG, inet_ntoa(cliaddr.sin_addr));
				sprintf(Client[i].tmp_processing,"%s/%s.txt",FOLDER_PROCESSING, inet_ntoa(cliaddr.sin_addr));
				Client[i].json = cJSON_CreateObject();
				ret = sendTime(connfd, &time_wait);
				if (ret <= 0){
					FD_CLR(sockfd, &allset);
					close(sockfd);
					client[i] = -1;
				}
				if (--nready <= 0)
					continue;		/* no more readable descriptors */
			}
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &readfds)) {
				//receives message from client
				bzero(rcvBuff, BUFF_SIZE + 5);
				ret = recv(sockfd, rcvBuff, BUFF_SIZE + 5, MSG_WAITALL);
				if (ret <= 0){
					FD_CLR(sockfd, &allset);
					close(sockfd);
					client[i] = -1;
				} else {
					process_status = processData(&Client[i], rcvBuff);
					if (process_status == -5) {
						FD_CLR(sockfd, &allset);
						close(sockfd);
						client[i] = -1;
					} else 
					if (process_status < 0) {
						goto CLOSE;
					}
				}
			}
			if (--nready <= 0)
				break;		/* no more readable descriptors */
		}
	}
	CLOSE:
	for (i = 0; i <= maxi; i++) {
		if ( (sockfd = client[i]) < 0)
			continue;
		if (FD_ISSET(sockfd, &allset)) {
			FD_CLR(sockfd, &allset);
			close(sockfd);
			client[i] = -1;
		}
		if (--nready <= 0)
			break;		
	}
	system("clear");
	printf("Server error. Closed!");
	return 0;
}
