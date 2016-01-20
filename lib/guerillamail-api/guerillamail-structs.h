#ifndef GUERILLAMAIL_STRUCTS_H
#define GUERILLAMAIL_STRUCTS_H
#include <curl/curl.h>

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE 1
#endif

/* 
	API methods enum
*/
typedef enum API_CALL_ID 
{
	GET_EMAIL_ADDRESS,
	SET_EMAIL_ADDRESS,
	FORGET_ME,
	CHECK_EMAIL,
	GET_EMAIL_LIST,
	FETCH_EMAIL
} api_call;

/*
	Email message structure
*/
typedef struct
{
	int is_new;
	char *mail_from;
	int mail_timestamp;
	int mail_read;
	char *mail_date;
	char *reply_to;
	char *mail_subject;
	char *mail_excerpt;
	int mail_id;
	int att;
	char *content_type;
	char *mail_recipient;
	int source_id;
	int source_mail_id;
	char *mail_body;
	int size;
} Mail;

/*
	Email messages vector
*/
typedef struct
{
	int size;
	int capacity;
	int last_email_id;
	Mail **data;
} MailList;

/*
	Mailbox structure
*/
typedef struct GUERILLA_API_INSTANCE {
	char *email_addr;
	unsigned email_timestamp;
	char *sid_token;
	int auto_increase;
	char *cookies_file;
	api_call func;
	char *last_result;
	int last_email_id;
	MailList *emails;
	int emails_count;
} GuerillaApiInstance;

/*
	Mailboxes vector
*/
typedef struct
{
	int size;
	int capacity;
	int current_instance;
	GuerillaApiInstance **data;
} ApiInstancesVector;

/*
	Helper struct for process curl response
*/
typedef struct
{
	size_t size;
	char *data;
} CURLresponse;


ApiInstancesVector * create_instances();
int instances_append(ApiInstancesVector *instances, GuerillaApiInstance *instance);
// void instances_remove(ApiInstancesVector *instances, GuerillaApiInstance *instance);

GuerillaApiInstance * instances_get(ApiInstancesVector *instances, int pos);
void instances_set(ApiInstancesVector *instances, int pos, GuerillaApiInstance *instance);
void free_instances(ApiInstancesVector *instances);
GuerillaApiInstance * get_current_instance(ApiInstancesVector *instances);
void set_current_instance_pos(ApiInstancesVector *instances, int pos);
int get_current_instance_pos(ApiInstancesVector *instances);

Mail * find_mail_by_id(GuerillaApiInstance *instance, int id);
Mail * find_mail_by_pos(GuerillaApiInstance *instance, int pos);

Mail * create_mail();
void free_mail(Mail *mail);

GuerillaApiInstance * create_guerilla_api_instance();
void free_guerilla_api_instance(GuerillaApiInstance *isntance);

void set_instance_options(GuerillaApiInstance *instance);
int is_new_instance(GuerillaApiInstance *instance);

void append_mail_to_list(MailList *list, Mail *mail);

#endif