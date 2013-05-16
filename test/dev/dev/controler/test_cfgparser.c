#include <stdlib.h>
#include <stdio.h>

#include "cfgparser.h"

int main(void){

    printf("====\ntest_port_controller_invalide.cfg :\n");
    if (cfgparser_parse("tests_cfgparser/test_port_controller_invalide.cfg"))
	printf("EXIT_FAILURE\n");
    else
	cfgparser_data_print();
    printf("====\ntest_commentaire_invalide.cfg :\n");
    if (cfgparser_parse("tests_cfgparser/test_commentaire_invalide.cfg"))
	printf("EXIT_FAILURE\n");
    else
	cfgparser_data_print();
    printf("====\ntest_valide.cfg :\n");
    if (cfgparser_parse("tests_cfgparser/test_valide.cfg"))
	printf("EXIT_FAILURE\n");
    else
	cfgparser_data_print();
    printf("====\ntest_valide1.cfg :\n");
    if (cfgparser_parse("tests_cfgparser/test_valide1.cfg"))
	printf("EXIT_FAILURE\n");
    else
	cfgparser_data_print();

    return EXIT_SUCCESS;
}
