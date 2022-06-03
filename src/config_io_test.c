#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

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
	
	bool pass = 0;
	
	printTestResult(pass, "all tests.");
	
	#include "./configs/basic_test_config.h"
	
	return 0;
}