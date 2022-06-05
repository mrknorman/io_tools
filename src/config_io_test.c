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
	const char* config_directory_name       = "./configs";
	const char* basic_test_config_file_name = "basic_test_config.cfg"
	
	bool pass = 0;
	
	#include "basic_test_config.h"	
	
	printTestResult(pass, "all tests.");
	
	return 0;
}