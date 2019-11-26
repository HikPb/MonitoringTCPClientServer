#ifndef _SEARCH_H_
#define _SEARCH_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include "cjson/cJSON.h"

#define BUFF_SIZE 1024
#define NAME_SIZE 256

/*	
This struct contain all client infomation.
*/
typedef struct client_detail {
	cJSON *value;
	struct client_detail *next;
} Client_detail;

int init();
void freeList();
void searchTime();
int searchByTime(char *time, cJSON *arr_res);
int searchByIp();
int getDataFromFile(char *ip, cJSON *client);
int insertFirstPos(cJSON *client);

#endif