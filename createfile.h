#ifndef _CREATEFILE_H_
#define _CREATEFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "clientinfo.h"


int setDatetime(ClientInfo* cli_info);
int setPath(ClientInfo* cli_info, char *key,char* extension, char* folder, char* path);
int saveJsonToFile(ClientInfo* cli_info);

#endif