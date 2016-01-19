#ifndef GUERILLAMAIL_CONSOLE_CLIENT
#define GUERILLAMAIL_CONSOLE_CLIENT
#include "guerillamail-api/guerillamail-structs.h"

void get_email_address_callback(GuerillaApiInstance *instance);
void fetch_email_callback(GuerillaApiInstance *instance);
void get_email_list_callback(GuerillaApiInstance *instance);
void check_mail_callback(GuerillaApiInstance *instance);
void add_new_instance(ApiInstancesVector *instances);

int console_main(int argc, char **argv);
void print_mail(Mail *mail);
void print_email_list(GuerillaApiInstance *instance);

#endif