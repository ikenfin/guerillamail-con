#include <stdlib.h>
#include <json/json.h>
#include <string.h>
#include <time.h>

#include "guerillamail-api/guerillamail-structs.h"
#include "guerillamail-api/guerillamail-api.h"
#include "guerillamail-console-client.h"

#define SECS_IN_17_MINUTES 1020

/*
	Error informers
*/
void instance_not_initialized_error()
{
	printf("%s \n", "No initialized instances! Please use `n` option, before other.");
}

void mail_not_found_error(int id)
{
	printf("Email [%d] not found \n", id);
}

void wrong_cmd_param_error(char cmd)
{
	switch(cmd) {
		case 'r' :
			printf("Type `%c %s` to %s\n", cmd, "<mail_id>", "read email");
			break;
		case 's' :
			printf("Type `%c %s` to %s\n", cmd, "<mailbox_id>", "switch mailbox");
			break;
		default :
			printf("%s\n", "Unrecognized error!");
			break;
	}
}

/*
	Console app api callbacks
*/

void forget_me_callback(GuerillaApiInstance *instance)
{
	free_guerilla_api_instance(instance);
	printf("%s\n", "Ok, forgetted.");
}

/*
	Fill mailbox settings
*/
void get_email_address_callback(GuerillaApiInstance *instance)
{
	// set email and timestamp from json here!
	set_instance_options(instance);
	printf("Ok, new mailbox address is - %s \n", instance->email_addr);
}

/*
	
*/
void get_email_list_callback(GuerillaApiInstance *instance)
{
	parse_mail_list(instance, FALSE);
}

/*
	Check for new emails
*/
void check_mail_callback(GuerillaApiInstance *instance)
{
	int emails_count = instance->emails_count;

	parse_mail_list(instance, TRUE);

	if(instance->emails_count > emails_count) {
		print_email_list(instance);
		printf("You have %d new message(s)\n", (instance->emails_count - emails_count));
	}
	else {
		printf("No new messages :(\n");
	}
}

/*
	Creates new guerilla mailbox and append it to mailboxes storage
*/
void add_new_instance(ApiInstancesVector *instances)
{
	GuerillaApiInstance *instance = create_guerilla_api_instance();
	instances_append(instances, instance);
}

/*
	Output email message info and text
*/
void print_mail(Mail *mail)
{
	printf("*** MESSAGE ***\n\n");
	printf("%s \n\n", mail->mail_body);
	printf("*** MESSAGE META ***\n");
	printf("* %-14s %s \n", "From:", mail->mail_from);
	printf("* %-14s %s \n", "Subject:", mail->mail_subject);
	printf("* %-14s %s \n", "Received at:", mail->mail_date);
	printf("********************\n\n");

	// mark mail as readed
	mail->mail_read = TRUE;
}

/*
	Output list of emails in current instance
*/
void print_email_list(GuerillaApiInstance *instance)
{
	int i;
	printf("Inbox [%s] emails (%d) \n", instance->email_addr, instance->emails_count);

	for(i = 0; i < instance->emails_count; i++) {
		printf("[%d] (%s) [%s] - %s \n", 
			//instance->emails->data[i]->mail_id, 
			i,
			(instance->emails->data[i]->mail_read == TRUE) ? "O" : "N",
			instance->emails->data[i]->mail_from, 
			instance->emails->data[i]->mail_subject
		);
	}
}

/*
	Check new messages for instances
	must be used in background (pthread)
*/
void __guerillamail_expired_checker(ApiInstancesVector *instances)
{
	unsigned current_timestamp = (unsigned) time(NULL);

	int i;

	if(instances == NULL || instances->size == 0) {
		return;
	}

	for(i = 0; i < instances->size; i++) {
		/* if was inactive in 17 mins - do something */
		if(current_timestamp - instances->data[i]->email_timestamp > SECS_IN_17_MINUTES) {
			/* printf("%s\n", "17 mins inactive! request for extend timestamp"); */
			instances->data[i]->func = SET_EMAIL_ADDRESS;
			query_api(instances->data[i], NULL);
		}
	}
}