#ifndef GUERILLAMAIL_CONSOLE_CLIENT_COMMON_H

#define GUERILLAMAIL_CONSOLE_CLIENT_COMMON_H

#include "guerillamail-api/guerillamail-structs.h"

void instance_not_initialized_error();
void mail_not_found_error(int id);
void wrong_cmd_param_error(char cmd);

void get_email_address_callback(GuerillaApiInstance *instance);
void forget_me_callback(GuerillaApiInstance *instance);
void fetch_email_callback(GuerillaApiInstance *instance);
void get_email_list_callback(GuerillaApiInstance *instance);
void check_mail_callback(GuerillaApiInstance *instance);
void add_new_instance(ApiInstancesVector *instances);
void print_mail(Mail *mail);
void print_email_list(GuerillaApiInstance *instance);

void __guerillamail_expired_checker(ApiInstancesVector *instances);

#endif