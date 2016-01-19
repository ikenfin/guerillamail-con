#ifndef GUERILLAMAIL_API_H

#define GUERILLAMAIL_API_H

#include "guerillamail-structs.h"

/* 
	Guerillamail api url
*/
#define API_URL "https://api.guerrillamail.com/ajax.php"

size_t curl_to_string(void *ptr, size_t size, size_t nmemb, void *data);
char * api_strcat(char *a, char *b);
void parse_mail_list(GuerillaApiInstance *instance, int update);

int query_api(GuerillaApiInstance *instance, void (*callback)(GuerillaApiInstance *instance));

void set_instance_options(GuerillaApiInstance *instance);

#endif