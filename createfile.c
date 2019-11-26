#include "createfile.h"

/*	int setDatetime(ClientInfo* cli_info)
	---------------------------------------------------------------------------
	TODO	: > Set datetime property to JSON object.
	---------------------------------------------------------------------------
	INPUT	: - ClientInfo *cli_info 	[Contain ip address and JSON pointer]
	OUTPUT	: + return 0				[Set sucess]
			  + return -1				[Can't set datetime property to JSON object]
*/
int setDatetime(ClientInfo* cli_info) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char datetime[2024];

	sprintf(datetime, "%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	// Set current date_time in obj Json
    if (cJSON_AddStringToObject(cli_info->json, "datetime", datetime) == NULL)
    {
        return -1;
    }

    return 0;
}


/*	int setPath(ClientInfo* cli_info, char *key,char* extension, char* folder, char* path)
	---------------------------------------------------------------------------
	TODO	: > Set the path of the client infomation file and add to JSON object.
	---------------------------------------------------------------------------
	INPUT	: - ClientInfo *cli_info 	[Contain ip address and JSON pointer]
	OUTPUT	: + return 0				[Save sucess]
			  + return -1				[JSON pointer is NULL]
			  + return -2				[Can't pen saving file]
*/
int setPath(ClientInfo* cli_info, char *key,char* extension, char* folder, char* path) {
	char path_file[2024];

	sprintf(path_file, "%s/%s[%ld].%s", folder , cli_info->ip_address, (unsigned long)time(NULL), extension);

    if ((rename(path, path_file)) != 0) {
		fprintf(stderr, "Can't rename file.\n");
		return -1;
	}

    if (cJSON_AddStringToObject(cli_info->json, key, path_file) == NULL)
    {
        return -2;
    }

    return 0;
}


/*	int saveJsonToFile(ClientInfo* cli_info)
	---------------------------------------------------------------------------
	TODO	: > Save Json in the file with name is IPv4 address.
	---------------------------------------------------------------------------
	INPUT	: - ClientInfo *cli_info 	[Contain ip address and JSON pointer]
	OUTPUT	: + return 0				[Save sucess]
			  + return -1				[JSON pointer is NULL]
			  + return -2				[Can't pen saving file]
*/
int saveJsonToFile(ClientInfo* cli_info) {
	char *out;
	FILE *fp;
	out = cJSON_Print(cli_info->json);
    if (out == NULL) {
        fprintf(stderr, "Failed to print computer.\n");
        return -1;
    }

    cJSON_Delete(cli_info->json);
	fp = fopen(cli_info->result,"a+");
    if (fp == NULL) {
    	fprintf(stderr, "Failed to open file.\n");
        return -2;
    }
    fputs(out, fp);
    fputs("\n", fp);
    free(out);
    fclose(fp);
    cli_info->json = cJSON_CreateObject();
    return 0;
}
