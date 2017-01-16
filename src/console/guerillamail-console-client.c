#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "guerillamail-api/guerillamail-structs.h"
#include "guerillamail-api/guerillamail-api.h"
#include "guerillamail-console-client.h"
#include "guerillamail-console-client-common.h"

#include "dbg/debug.h"

#define COMMAND_MAX 1
#define EMPTY_CMD_PARAM -99

/* 10 minutes */
#define GUERILLAMAIL_CHECKER_DELAY 600000000

static pthread_t MAIN_THREAD;

/*
	Pthread worker function
	Run checker every GUERILLAMAIL_CHECKER_DELAY microseconds
*/
void *guerillamail_background_updater(void *arg)
{
	pthread_join(MAIN_THREAD, NULL);
	ApiInstancesVector *instances = (ApiInstancesVector*) arg;

	while(TRUE) {
		/* It check to instances will be active in user session */
		__guerillamail_expired_checker(instances);
		usleep(GUERILLAMAIL_CHECKER_DELAY);
	}
}

/*
	Console client for guerillamail
*/
int console_main(int argc, char **argv)
{
	MAIN_THREAD = pthread_self();

	ApiInstancesVector *instances = create_instances(); 
	
	GuerillaApiInstance *instance;

	/*
		pos - current instance position
		num - used to store user choice
	*/
	int num, pos, i, j;
	int menu_loop = 1;

	Mail *tmp_mail = NULL;

	char input[LINE_MAX];
	char cmd;
	int cmd_param;

	/* background thread to check email box will exists in session */
	pthread_t pth;

	pthread_create(&pth, NULL, guerillamail_background_updater, instances);

	/* main menu loop */
	while(menu_loop == 1) {

		instance = get_current_instance(instances);
		pos = get_current_instance_pos(instances);
		
		printf("[%s]:$ ", instance == NULL ? "guerillamail" : instance->email_addr);

		fgets(input, LINE_MAX, stdin);

		cmd_param = EMPTY_CMD_PARAM;

		sscanf(input, "%c %d", &cmd, &cmd_param);

		/* recognize user input */
		switch(cmd) {
			/* exit from app */
			case 'q':
				menu_loop = 0;
			break;
			case 'f':
				printf("%s\n", "Try to forget you:)");
				if(instance == NULL) {
					instance_not_initialized_error();
					break;
				}
				/* IMPLEMENT THIS: */
				instance->func = FORGET_ME;
				query_api(instance, &forget_me_callback);
			break;
			case 'g':
				DEBUG_PRINT("%s\n", "SET_EMAIL_ADDRESS");
				instance->func = SET_EMAIL_ADDRESS;
				query_api(instance, &get_email_address_callback);
			break;
			/* check for new emails */
			case 'c':
				if(instance == NULL) {
					instance_not_initialized_error();
					break;
				}
				instance->func = CHECK_EMAIL;
				query_api(instance, &check_mail_callback);
			break;
			/* read email message */
			case 'r':
				if(cmd_param == EMPTY_CMD_PARAM) {
					wrong_cmd_param_error(cmd);
					break;
				}

				if(instance == NULL) {
					instance_not_initialized_error();
					break;
				}

				tmp_mail = find_mail_by_pos(instance, cmd_param);

				if(tmp_mail == NULL) {
					mail_not_found_error(cmd_param);
					break;
				}

				instance->func = FETCH_EMAIL;
				instance->last_email_id = tmp_mail->mail_id;//(int) num;
				query_api(instance, &fetch_email_callback);
			break;
			/* list existing/download emails */
			case 'l':
				if(instance == NULL) {
					instance_not_initialized_error();
					break;
				}

				if(instance->emails == NULL || instance->emails->size == 0) {
					instance->func = GET_EMAIL_LIST;
					query_api(instance, &get_email_list_callback);
				}
				
				print_email_list(instance);
			break;
			/* register new mailbox */
			case 'n':
				instance = create_guerilla_api_instance();
				query_api(instance, &get_email_address_callback);
				instances_append(instances, instance);
			break;
			/* list of mailboxes */
			case 'i':
				printf("Mailboxes [%d]: \n", instances->size);

				if(instances->size == 0) {
					printf("%s \n", "No instances! Type `n` to get new mailbox!");
				}
				else {
					for(j = 0; j < instances->size; j++) {
						// printf("another check: %s\n", instances.data[j] == NULL ? "IS NULL" : "FUCK!");
						// if(instances->data[j] == NULL)
						// 	continue;
						// else {
						// 	printf("Not NULL, SO IS %s\n", instances->data[j]);
						// }
						if(instances->data[j]->active == TRUE) 
							printf("[%s%d] %s \n", ((j == pos) ? "*" : ""), j, instances->data[j]->email_addr);
					}
				}
			break;
			/* switch current mailbox */
			case 's':
				if(cmd_param == EMPTY_CMD_PARAM) {
					wrong_cmd_param_error(cmd);
					break;
				}

				if(cmd_param >= instances->size) {
					printf("%s \n", "Element not found! Use 'i' to list all instances!");
					break;
				}

				set_current_instance_pos(instances, cmd_param);
			break;
			/* help */
			default:
				printf("%s \n", "Keys:");
				printf("%s \n", "c - check email");
				printf("%s \n", "r <id> - read message <id>");
				printf("%s \n", "l - list messages");
				printf("%s \n", "n - new email instance");
				printf("%s \n", "i - list of instances");
				printf("%s \n", "s <id> - switch instance");
			break;
		}
	}

	free_instances(instances);

	return 0;
}