#include "cjson/cJSON.h"
#define BUFF_SIZE 1024

/*
This struct contain temporary infomation client.
*/
typedef struct client_info {
	int status;
	char ip_address[BUFF_SIZE];
	char result[BUFF_SIZE];
	char tmp_info[BUFF_SIZE];
	char tmp_image[BUFF_SIZE];
	char tmp_operation[BUFF_SIZE];
	char tmp_processing[BUFF_SIZE];
	cJSON *json;
} ClientInfo;