#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "guerillamail-structs.h"

/* Main guerillamail api structures control functions: */

/* helper: check if file exists 
	TODO:
		move to guerillamail-helpers.c
*/
int file_exists(const char *filename)
{
	struct stat st;
	int result = stat(filename, &st);
	return result == 0;
}

/* Initialize new Mail instance */
Mail * create_mail()
{
	Mail *mail = malloc(sizeof(Mail));

	mail->is_new = TRUE;
	mail->mail_from = NULL;
	mail->mail_date = NULL;
	mail->reply_to = NULL;
	mail->mail_subject = NULL;
	mail->mail_excerpt = NULL;
	mail->content_type = NULL;
	mail->mail_recipient = NULL;
	mail->mail_body = NULL;
	
	return mail;
}

/* Destroy Mail and free memory */
void free_mail(Mail *mail)
{
	if(mail != NULL) {
		free(mail->mail_from);
		free(mail->mail_date);
		free(mail->reply_to);
		free(mail->mail_subject);
		free(mail->mail_excerpt);
		free(mail->content_type);
		free(mail->mail_recipient);
		free(mail->mail_body);

		free(mail);
	}
}

/* Initialize new MailList instance */
MailList * create_maillist()
{
	MailList *list = malloc(sizeof(MailList));
	list->data = NULL;//malloc(sizeof(Mail));
	list->size = 0;
	list->last_email_id = 0;

	return list;
}

/* Destroy MailList and free memory */
void free_maillist(MailList *list)
{
	int i;

	for(i = (list->size - 1); i >= 0; i--) {
		free_mail(list->data[i]);
	}
	free(list->data);
	free(list);
}

/* Initialize new mailbox instance */
GuerillaApiInstance * create_guerilla_api_instance()
{
	time_t _time;
	srand((unsigned) time(&_time));

	GuerillaApiInstance *instance = malloc(sizeof(GuerillaApiInstance));
	instance->email_addr = NULL;
	instance->sid_token = NULL;
	instance->last_result = NULL;
	instance->func = GET_EMAIL_ADDRESS;
	
	instance->cookies_file = malloc(sizeof(char) * 12);
	sprintf(instance->cookies_file, "cookie_%d", (rand() % 900));
	
	instance->emails = create_maillist();
	instance->emails_count = 0;

	return instance;
}

/* Destroy mailbox instance and free memory */
void free_guerilla_api_instance(GuerillaApiInstance *instance)
{
	if(instance == NULL)
		return;

	if(instance->email_addr != NULL)
		free(instance->email_addr);

	if(instance->last_result != NULL)
		free(instance->last_result);

	if(instance->cookies_file != NULL) {
		if(file_exists(instance->cookies_file)) {
			unlink(instance->cookies_file);
		}
		free(instance->cookies_file);
	}

	free_maillist(instance->emails);
	
	free(instance);
}

/* check if instance is new */
int is_new_instance(GuerillaApiInstance *instance)
{
	return instance->email_addr == NULL;
}

/* appends Mail to MailList */
void append_mail_to_list(MailList *list, Mail *mail)
{
	mail->is_new = FALSE;

	if(list->data == NULL)
		list->data = malloc(sizeof(Mail));
	else
		list->data = realloc(list->data, sizeof(Mail) * (list->size + 1));
	
	if(list->data == NULL) {
		fprintf(stderr, "%s \n", "Not enough memory!");
		exit(0);
	}
	
	list->last_email_id = mail->mail_id;
	list->data[list->size++] = mail;
}

/* Mailbox'es vector initialization */
ApiInstancesVector * create_instances() {
	ApiInstancesVector *instances = malloc(sizeof(ApiInstancesVector));
	instances->size = 0;
	instances->capacity = 10;
	instances->current_instance = 0;
	instances->data = malloc(sizeof(GuerillaApiInstance) * instances->capacity);

	return instances;
}

/* Destroy meailboxes vector and free memory */
void free_instances(ApiInstancesVector *instances)
{
	if(instances == NULL)
		return;

	int i;

	if(instances->data != NULL) {
		for(i = (instances->size - 1); i > 0; i--) {
			free_guerilla_api_instance(instances->data[i]);
		}
		free(instances->data);
	}

	free(instances);
}

/* Append mailbox to mailboxes vector */
int instances_append(ApiInstancesVector *instances, GuerillaApiInstance *instance)
{
	if(instance->email_addr == NULL) {
		free_guerilla_api_instance(instance);
		return;
	}

	if(instances->size >= instances->capacity)
	{
		instances->capacity *= 2;
		instances->data = realloc(instances->data, sizeof(GuerillaApiInstance) * instances->capacity + 1);
	}

	instances->data[instances->size++] = instance;
	return instances->size;
}

/* Get mailbox with [pos] from vector */
GuerillaApiInstance * instances_get(ApiInstancesVector *instances, int pos)
{
	if (pos > instances->size || pos < 0) {
		printf("Index %d out of bounds for vector of size %d\n", pos, instances->size);
		exit(1);
	}
	
	return instances->data[pos];
}

/* Set mailbox in vector */
void instances_set(ApiInstancesVector *instances, int pos, GuerillaApiInstance *instance)
{
	while(pos >= instances->size)
	{
		instances_append(instances, NULL);
	}

	instances->data[pos] = instance;
}

/* returns current active mailbox */
GuerillaApiInstance * get_current_instance(ApiInstancesVector *instances)
{
	if(instances->size == 0)
		return NULL;
	return instances_get(instances, instances->current_instance);
}

/* set current mailbox by pos */
void set_current_instance_pos(ApiInstancesVector *instances, int pos)
{
	instances->current_instance = pos;
}

/* returns current active mailbox position in vector */
int get_current_instance_pos(ApiInstancesVector *instances)
{
	return instances->current_instance;
}

/*
	search Mail in mailbox MailList by id
	if not found returns NULL
*/
Mail * find_mail_by_id(GuerillaApiInstance *instance, int id)
{
	int i;
	
	if(instance->emails == NULL || instance->emails->size == 0)
		return NULL;

	for(i = 0; i < instance->emails->size; i++) {
		if(instance->emails->data[i]->mail_id == id)
			return instance->emails->data[i];
	}

	return NULL;
}

/*
	search Mail in mailbox MailList by position
	if not found returns NULL
*/
Mail * find_mail_by_pos(GuerillaApiInstance *instance, int pos)
{
	int i;
	
	if(instance->emails == NULL || instance->emails->size == 0 || pos > instance->emails->size || pos < 0)
		return NULL;

	for(i = 0; i < instance->emails->size; i++) {
		if(i == pos)
			return instance->emails->data[i];
	}

	return NULL;
}