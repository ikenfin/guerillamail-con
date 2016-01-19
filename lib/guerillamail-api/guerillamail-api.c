#include <stdlib.h>
#include <string.h>
#include <json/json.h>
#include <curl/curl.h>

#include "guerillamail-api.h"

/* curl response handler callback */
size_t curl_to_string(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	
	if(realsize == 0)
		return 0;

	CURLresponse *resp = (CURLresponse *) data;
	resp->data = realloc(resp->data, resp->size + realsize + 1);

	if(resp->data == NULL)
	{
		fprintf(stderr, "%s \n", "Out of memory! \n");
		exit(1);
	}

	memcpy(&(resp->data[resp->size]), ptr, realsize);
	resp->size += realsize;
	resp->data[resp->size] = '\0';

	return realsize;
}

/* strings concat implementation */
char * api_strcat(char *a, char *b)
{
	char *buffer = malloc(strlen(a) + strlen(b) + 1);

	if(buffer == NULL) {
		fprintf(stderr, "%s \n", "Out of memory!");
	}
	else {
		strcpy(buffer, a);
		strcat(buffer, b);
	}

	return buffer;
}

/*
	Set mailbox options after GET_EMAIL_ADDRESS called
*/
void set_instance_options(GuerillaApiInstance *instance)
{
	struct json_object *new_obj = NULL;
	struct json_object *email_element = NULL;
	struct json_object *timestamp_element = NULL;
	struct json_object *sid_token_element = NULL;

	new_obj = json_tokener_parse(instance->last_result);
	
	if(json_object_object_get_ex(new_obj, "email_addr", &email_element)) {
		instance->email_addr = strdup((char *) json_object_get_string(email_element));
	}	
	
	if(json_object_object_get_ex(new_obj, "email_timestamp", &timestamp_element)) {
		instance->email_timestamp = json_object_get_int(timestamp_element);
	}

	if(json_object_object_get_ex(new_obj, "sid_token", &sid_token_element)) {
		instance->sid_token = strdup((char *) json_object_get_string(sid_token_element));
	}

	json_object_put(new_obj);
}


/*
	Fill existing/new email message structure from json
*/
void fetch_email_callback(GuerillaApiInstance *instance)
{
	struct json_object *new_obj;
	struct json_object *json_mail_from;
	struct json_object *json_mail_subject;
	struct json_object *json_mail_date;
	struct json_object *json_mail_body;

	// printf("%s \n", instance->last_result);
	new_obj = json_tokener_parse(instance->last_result);
	Mail *mail = find_mail_by_id(instance, instance->last_email_id);

	if(mail == NULL) {
		mail = create_mail();
		append_mail_to_list(instance->emails, mail);
	}

	if(json_object_object_get_ex(new_obj, "mail_from", &json_mail_from)) {
		mail->mail_from = strdup((char *) json_object_get_string(json_mail_from));
	}
	if(json_object_object_get_ex(new_obj, "mail_subject", &json_mail_subject)) {
		mail->mail_subject = strdup((char *) json_object_get_string(json_mail_subject));
	}
	if(json_object_object_get_ex(new_obj, "mail_date", &json_mail_date)) {
		mail->mail_date = strdup((char *) json_object_get_string(json_mail_date));
	}
	if(json_object_object_get_ex(new_obj, "mail_body", &json_mail_body)) {
		mail->mail_body = strdup((char *) json_object_get_string(json_mail_body));
	}

	print_mail(mail);

	json_object_put(new_obj);
}

void parse_mail_body(GuerillaApiInstance *instance)
{
	fetch_email_callback(instance);
}

void parse_mail_list(GuerillaApiInstance *instance, int update)
{
	int i, count = 0, list_exists = 0;

	struct json_object *new_obj = NULL;
	struct json_object *element = NULL;
	struct json_object *list = NULL;

	struct json_object *json_mail_id = NULL;
	struct json_object *json_mail_subject = NULL;
	struct json_object *json_mail_from = NULL;
	struct json_object *json_mail_excerpt = NULL;
	struct json_object *json_mail_read = NULL;

	new_obj = json_tokener_parse(instance->last_result);
	list_exists = json_object_object_get_ex(new_obj, "list", &list);
	
	if(list_exists == FALSE) {
		fprintf(stderr, "%s \n", "List not found!");
		exit(1);
	}

	count = (int) json_object_array_length(list);

	if(count == 0)
		return;

	int mail_id = -1;

	for(i = 0; i < count; i++) {
		element = json_object_array_get_idx(list, i);

		if(json_object_object_get_ex(element, "mail_id", &json_mail_id)) {
			mail_id = (int) json_object_get_int(json_mail_id);			
		}

		Mail *_m = NULL;
		_m = find_mail_by_id(instance, mail_id);

		if(_m == NULL) {
			_m = create_mail();
		}

		if(_m == NULL) {
			fprintf(stderr, "%s \n", "Not enought memory for email message!");
		}

		if(json_object_object_get_ex(element, "mail_from", &json_mail_from)) {
			_m->mail_from = strdup((char *) json_object_get_string(json_mail_from));
		}

		if(json_object_object_get_ex(element, "mail_excerpt", &json_mail_excerpt)) {
			_m->mail_excerpt = strdup((char *) json_object_get_string(json_mail_excerpt));
		}

		if(json_object_object_get_ex(element, "mail_subject", &json_mail_subject)) {
			_m->mail_subject = strdup((char *) json_object_get_string(json_mail_subject));
		}

		if(json_object_object_get_ex(element, "mail_id", &json_mail_id)) {
			_m->mail_id = (int) json_object_get_int(json_mail_id);			
		}

		if(json_object_object_get_ex(element, "mail_read", &json_mail_read)) {
			_m->mail_read = (int) json_object_get_int(json_mail_read);
		}

		if(_m->is_new == TRUE)
			append_mail_to_list(instance->emails, _m);

		if(i == count - 1) {
			instance->last_email_id = _m->mail_id;
			instance->emails->last_email_id = _m->mail_id;
		}
	}

	instance->emails_count = instance->emails->size;
}

/* send query to guerillamail api via curl */
int query_api(GuerillaApiInstance *instance, void (*callback)(GuerillaApiInstance *instance))
{
	CURL *curl;
	CURLcode result;
	
	CURLresponse rsp;
	rsp.data = malloc(1);
	rsp.size = 0;

	char *url, *url_part;
	char buf[12];

	switch(instance->func) {
		case GET_EMAIL_ADDRESS:
			url = api_strcat(API_URL, "?f=get_email_address");
			break;
		/* restore session*/
		case SET_EMAIL_ADDRESS:
			url = api_strcat(API_URL, "?f=set_email_address&email_user=");
			url = api_strcat(url, instance->email_addr);
			break;
		case FORGET_ME:
			url = api_strcat(API_URL, "?f=forget_me&email_addr=");
			url = api_strcat(url, instance->email_addr);
			break;
		case CHECK_EMAIL:
			url = api_strcat(API_URL, "?f=check_email&seq=");
			sprintf(buf, "%d", instance->emails->last_email_id);
			url = api_strcat(url, buf);
			break;
		case GET_EMAIL_LIST:
			url = api_strcat(API_URL, "?f=get_email_list&offset=0");
			break;
		case FETCH_EMAIL:

			if(find_mail_by_id(instance, instance->last_email_id) == NULL) {
				printf("Email with id <%d> not found!\n", instance->last_email_id);
				return 0;
			}

			url_part = api_strcat(API_URL, "?f=fetch_email&email_id=");

			sprintf(buf, "%d", instance->last_email_id);
			url = api_strcat(url_part, buf);

			free(url_part);
			break;
	}

	if(instance->sid_token != NULL) {
		printf("SID: %s\n", instance->sid_token);
		url = api_strcat(url, "&sid_token=");
		url = api_strcat(url, instance->sid_token);
	}

	// curl_global_init(CURL_GLOBAL_ALL);
	printf("%s\n", url);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_to_string);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rsp);
	// curl_easy_setopt(curl, CURLOPT_COOKIEFILE, instance->cookies_file);
	// curl_easy_setopt(curl, CURLOPT_COOKIEJAR, instance->cookies_file);

	result = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	free(url);

	if(result != CURLE_OK) {
		fprintf(stderr, "%s \n", curl_easy_strerror(result));
		free(rsp.data);
		return 0;
	}

	free(instance->last_result);

	instance->last_result = strdup(rsp.data);
	
	if(callback != NULL)
		callback(instance);

	free(rsp.data);

}