#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <inttypes.h>

#include "config.h"
#include "structures.h"

void printTestResult(
    const int32_t  pass,
    const char    *test_name
    ) {
    
    /**
     * Prints result of test
     * @param 
     *     const bool       pass: test result.
     *     const char *test_name: test name.
     * @see
     * @return void
     */
    
    const char *result = pass ? "FAILED" : "PASSED";
    printf("%s: %s.\n\n", test_name, result); 
}

int main() {

	//Variables:
	const char* config_directory_name       = "configs";
	const char* basic_test_config_file_name = "basic_test_config.cfg";
	
	#include "basic_test_config.h"	

	char* config_file_path;
	asprintf(&config_file_path, "./%s/%s", config_directory_name, basic_test_config_file_name);
	
	basic_test_config_s* basic_test_config = 
		((basic_test_config_s**) 
			 readConfig(
				 config_file_path, 
				 "parameter_1, parameter_2, parameter_3, parameter_4", 
				 "char* , float, float, int", 
				 4, 
				 1, 
				 8, 
				 sizeof(basic_test_config_s)
			 )
		)[0];
	
	printf("%s \n", basic_test_config->parameter_1);
	
	free(config_file_path);
	 
	bool pass = 0;
	
	
	printTestResult(pass, "all tests.");
	
	return 0;
}