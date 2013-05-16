#ifndef CFGPARSER_H
#define CFGPARSER_H

int cfgparser_parse(char *fileName);
int get_controller_port();
int get_poll_timeout_value();
void cfgparser_data_print();

#endif
