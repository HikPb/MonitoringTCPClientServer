#include "search.h"

Client_detail *head = NULL;

/*	void freeList(Client_detail* head)
	---------------------------------------------------------------------------
	TODO	: > Free linked list
	---------------------------------------------------------------------------
	INPUT	: - Client_detail* head 	[First address pointer of linked-list]
*/
void freeList()
{
   Client_detail* tmp;

   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }

}

/*	int insertFirstPos(cJSON *client)
	---------------------------------------------------------------------------
	TODO	: > Insert node to first position of linked-list
	---------------------------------------------------------------------------
	INPUT	: - cJSON *client			[Node need insert]
	OUTPUT	: + return 0				[Insert success]
*/
int insertFirstPos(cJSON *client) {
	Client_detail *temp = (Client_detail *)malloc(sizeof(Client_detail));
	temp->value = client;
	temp->next = head;
	// Update first list address
	head = temp;
	return 0;
}


/*	int getDataFromFile(char *ip, cJSON *client)
	---------------------------------------------------------------------------
	TODO	: > Get JSON data from file with name is IP'client.
	---------------------------------------------------------------------------
	INPUT	: - char *ip				[IP address]
			  - cJSON *client			[Result of searching infomation client]
	OUTPUT	: + return 0				[Have result]
			  + return -1				[No result]
*/
int getDataFromFile(char *ip, cJSON *client) {
	char path[BUFF_SIZE] = "";
	sprintf(path, "result/%s.txt", ip);
	FILE *fin = fopen(path, "r");
	char temp[BUFF_SIZE] = "";
	char json[BUFF_SIZE] = "";
	int curr = 0;
	cJSON *results = NULL;
	results = cJSON_AddArrayToObject(client, "results");
	cJSON_AddStringToObject(client, "ip", ip);

	if (fin == NULL) {
		return -1;
	}

	while (!feof(fin)){
		strcpy(temp, "");
		fgets(temp, BUFF_SIZE, fin);
		strcat(json, temp);
		if (strcmp(temp, "}\n") == 0) {
			json[strlen(json) - 1] = '\0';
			cJSON *result = cJSON_Parse(json);
			cJSON_AddItemToArray(results, result);
			curr++;
			strcpy(json, "");
		}
	}

	fclose(fin);
	return 0;
}


/*	int searchByIp()
	---------------------------------------------------------------------------
	TODO	: > Search client infomation by IPv4
	---------------------------------------------------------------------------
	OUTPUT	: + return 0				[Have result]
			  + return -1				[IP doesn't exist in server]
*/
int searchByIp() {
	char ip[BUFF_SIZE];
	cJSON *res_client = cJSON_CreateObject();
	cJSON *results;
	cJSON *result;
	char command[BUFF_SIZE], c[BUFF_SIZE];
	int i = 1, select = 0, back;
	printf("Enter a ipv4: \n");
	scanf("%s", ip);
	if (getDataFromFile(ip, res_client) == -1) {
		printf("IP not found.\n");
		return -1;
	}

	while (1) {
		i = 1;
		results = cJSON_GetObjectItemCaseSensitive(res_client, "results");
		cJSON_ArrayForEach(result, results) {
			cJSON *datetime = cJSON_GetObjectItemCaseSensitive(result, "datetime");
			printf("%d. %s\n", i, datetime->valuestring);
			i++;
		}
		back = i;
		printf("%d. BACK\n", back);
		printf("Enter a number: \n");
		while (1) {
			scanf("%[^0-9]%d",c,&select);
			if ((select > 0) && (select <= back)) {
				break;
			} else {
				printf("Wrong number. Enter again: \n");
			}
		}
		if (select == back) {
			break;
		}
		i = 1;
		cJSON_ArrayForEach(result, results) {
			if (i != select) {
				i++;
				continue;
			}
			cJSON *infomation = cJSON_GetObjectItemCaseSensitive(result, "infomation");
			cJSON *keyboard_mouse_operations = cJSON_GetObjectItemCaseSensitive(result, "keyboard_mouse_operations");
			cJSON *process_info = cJSON_GetObjectItemCaseSensitive(result, "process_info");
			cJSON *image = cJSON_GetObjectItemCaseSensitive(result, "image");
			
			sprintf(command, "xdg-open %s", infomation->valuestring);
			system(command);
			sprintf(command, "xdg-open %s", process_info->valuestring);
			system(command);
			sprintf(command, "xdg-open %s", keyboard_mouse_operations->valuestring);
			system(command);
			sprintf(command, "xdg-open %s", image->valuestring);
			system(command);
			break;
		}
		system("clear");
	}
	
	return 0;
}


/*	int searchByTime(char *time, cJSON *arr_res)
	---------------------------------------------------------------------------
	TODO	: > Search client infomation by datetime
	---------------------------------------------------------------------------
	INPUT	: - char *time 				[Datetime]
			  - cJSON *arr_res			[Array of searching results]
	OUTPUT	: + return 0				[No result]
			  + return 1				[Have result]
*/
int searchByTime(char *time, cJSON *arr_res) {
	Client_detail *temp = head;
	cJSON *result = NULL;
	cJSON *results = NULL;
	cJSON *datetime = NULL;
	cJSON *ip = NULL;
	int flag = 0; // if flag = 0 then no result else had result
	cJSON *res = cJSON_AddArrayToObject(arr_res, "results");

	while (temp != NULL) {
		ip = cJSON_GetObjectItemCaseSensitive(temp->value, "ip");
		results = cJSON_GetObjectItemCaseSensitive(temp->value, "results");
		cJSON_ArrayForEach(result, results) {
			datetime = cJSON_GetObjectItemCaseSensitive(result, "datetime");
			if (strcmp(datetime->valuestring, time) == 0) {
				cJSON *buff = cJSON_CreateObject();
				cJSON *infomation = cJSON_GetObjectItemCaseSensitive(result, "infomation");
				cJSON *keyboard_mouse_operations = cJSON_GetObjectItemCaseSensitive(result, "keyboard_mouse_operations");
				cJSON *process_info = cJSON_GetObjectItemCaseSensitive(result, "process_info");
				cJSON *image = cJSON_GetObjectItemCaseSensitive(result, "image");
				cJSON_AddStringToObject(buff, "ip", ip->valuestring);
				cJSON_AddStringToObject(buff, "infomation", infomation->valuestring);
				cJSON_AddStringToObject(buff, "keyboard_mouse_operations", keyboard_mouse_operations->valuestring);
				cJSON_AddStringToObject(buff, "process_info", process_info->valuestring);
				cJSON_AddStringToObject(buff, "image", image->valuestring);
				cJSON_AddStringToObject(buff, "datetime", datetime->valuestring);
			    cJSON_AddItemToArray(res, buff);
			    flag = 1;
			}
		}
		temp = temp->next;
	}

	return flag > 0;
}


/*	int searchTime()
	---------------------------------------------------------------------------
	TODO	: > Input a datetime and search client infomation by datetime.
	---------------------------------------------------------------------------
*/
void searchTime() {
	char datetime[NAME_SIZE], command[NAME_SIZE];
	cJSON *arr_res = cJSON_CreateObject();
	cJSON *results;
	cJSON *result;
	int i = 1, select, back;
	char c, str[BUFF_SIZE];
	printf("Enter a datetime(yyyy-mm-dd hh:mm:ss): ");
	scanf("%c%[^\n]s", &c, datetime);

	if (searchByTime(datetime, arr_res) == 1) {
		while (1) {
			i = 1;
			results = cJSON_GetObjectItemCaseSensitive(arr_res, "results");
			cJSON_ArrayForEach(result, results) {
				cJSON *ip = cJSON_GetObjectItemCaseSensitive(result, "ip");
				printf("%d. %s\n", i, ip->valuestring);
				i++;
			}
			back = i;
			printf("%d. BACK\n", back);
			printf("Enter a number: \n");
			while (1) {
				scanf("%[^0-9]%d",str,&select);
				if ((select > 0) && (select <= back)) {
					break;
				} else {
					printf("Wrong number. Enter again: \n");
				}
			}

			if (select == back) {
				break;
			}

			i = 1;
			cJSON_ArrayForEach(result, results) {
				if (i != select) {
					i++;
					continue;
				}
				cJSON *infomation = cJSON_GetObjectItemCaseSensitive(result, "infomation");
				cJSON *keyboard_mouse_operations = cJSON_GetObjectItemCaseSensitive(result, "keyboard_mouse_operations");
				cJSON *process_info = cJSON_GetObjectItemCaseSensitive(result, "process_info");
				cJSON *image = cJSON_GetObjectItemCaseSensitive(result, "image");

				sprintf(command, "xdg-open %s", infomation->valuestring);
				system(command);
				sprintf(command, "xdg-open %s", process_info->valuestring);
				system(command);
				sprintf(command, "xdg-open %s", keyboard_mouse_operations->valuestring);
				system(command);
				sprintf(command, "xdg-open %s", image->valuestring);
				system(command);
				break;
			}
			system("clear");
		}
	} else {
		printf("No result.\n");
	}
}


/*	int init()
	---------------------------------------------------------------------------
	TODO	: > Initialize linked-list of JSON object. This JSON object contain
				all infomation client and sending datetime. 
	---------------------------------------------------------------------------
	OUTPUT	: + return 0				[Initialize sucess]
*/
int init() {
	head = NULL;
	DIR *d;
	struct dirent *dir;
	d = opendir("result");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if ((strcmp(dir->d_name, ".") != 0) && (strcmp(dir->d_name, "..") != 0)) {
				cJSON *client = cJSON_CreateObject();
				char ip[BUFF_SIZE] = "";
				strcpy(ip, dir->d_name);
				ip[strlen(ip) - 4] = '\0'; //get ip from filename

				getDataFromFile(ip, client);
				insertFirstPos(client);
			}
		}
		closedir(d);
	}
	return 0;
}