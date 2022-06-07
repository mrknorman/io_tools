#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <inttypes.h>

#include "config.h"
#include "structures.h"
#include "console.h"

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
    
    const char *result = pass ? "PASSED" : "FAILED";
    printf("%s: %s.\n\n", test_name, result); 
}

bool testSingleConfig(
	const int32_t  verbosity,
	const char    *config_directory_name
	) {
	
	const char* file_name = "single_config_test.cfg";

	bool pass = true;
	
	#include "single_config_test.h"	

	char* config_file_path;
	asprintf(&config_file_path, "./%s/%s", config_directory_name, file_name);
	
	const int32_t expected_num_variables = 5;
	const int32_t expected_num_configs = 1;
	      int32_t num_configs = 0; 
	single_config_test_s* test_results = 
		((single_config_test_s**) 
			 readConfig(
			     verbosity,
				 config_file_path, 
				 test_config,
				 &num_configs
			 )
		)[0];
		
	pass *= (num_configs == expected_num_configs);
	
	if (test_results == NULL) {
		return false;
	}
	
	single_config_test_s known_results = {
		.parameter_string = "Config Test",
		.parameter_float  = 1.1f,
		.parameter_int    = 1,
		.parameter_bool   = false,
		.parameter_char   = 'a'
	};
	
	uni_s *cells = malloc(sizeof(uni_s)* (size_t) ( expected_num_configs * expected_num_variables * 2 ));

	cells[0] = (uni_s) {*((uni_u*)(&known_results.parameter_string)), string_e};
	cells[1] = (uni_s) {*((uni_u*)(&known_results.parameter_float )), float_e};
	cells[2] = (uni_s) {*((uni_u*)(&known_results.parameter_int   )), int_e};
	cells[3] = (uni_s) {*((uni_u*)(&known_results.parameter_bool  )), bool_e};
	cells[4].value.c = known_results.parameter_char; cells[4].type = char_e;
		
	cells[expected_num_variables + 0] = (uni_s) {*((uni_u*)(&test_results->parameter_string)), string_e};
	cells[expected_num_variables + 1] = (uni_s) {*((uni_u*)(&test_results->parameter_float )), float_e};
	cells[expected_num_variables + 2] = (uni_s) {*((uni_u*)(&test_results->parameter_int   )), int_e};
	cells[expected_num_variables + 3] = (uni_s) {*((uni_u*)(&test_results->parameter_bool  )), bool_e};
	cells[expected_num_variables + 4].value.c = test_results->parameter_char; cells[expected_num_variables + 4].type = char_e;
	
	pass *= !strcmp(known_results.parameter_string, test_results->parameter_string);
	pass *= (known_results.parameter_float == test_results->parameter_float);
	pass *= (known_results.parameter_int   == test_results->parameter_int);
	pass *= (known_results.parameter_bool  == test_results->parameter_bool);
	pass *= (known_results.parameter_char  == test_results->parameter_char);
	
	table_column_s columns[] = {
		{"Known String" , 4, 1},
		{"Known Float"  , 4, 1},
		{"Known Int"    , 4, 1},
		{"Known Bool"   , 4, 1},
		{"Known Char"   , 4, 1},
		{"Loaded String", 4, 1},
		{"Loaded Float" , 4, 1},
		{"Loaded Int"   , 4, 1},
		{"Loaded Bool"  , 4, 1},
		{"Loaded Char"  , 4, 1}
	};
	
	table_s table = {
		expected_num_configs,
		expected_num_variables * 2,
		"Single Config Test",
		cells,
		columns
	};
	
	printTable(table);
	
	printTestResult(pass, "single config test.");
	
	free(config_file_path);

	return pass;
}

bool testMultiConfig(
	const int32_t  verbosity,
	const char    *config_directory_name
	) {
	
	const char* file_name = "multi_config_test.cfg";

	bool pass = true;
	
	#include "multi_config_test.h"	

	char* config_file_path;
	asprintf(&config_file_path, "./%s/%s", config_directory_name, file_name);
	
	const int32_t expected_num_variables = 5;
	const int32_t expected_num_configs = 3;
		  int32_t num_configs = 0; 
	multi_config_test_s** test_results = 
		((multi_config_test_s**) 
			 readConfig(
			     verbosity,
				 config_file_path, 
				 test_config,
				 &num_configs
			 )
		);
		
	pass *= (num_configs == expected_num_configs);
	
	if (test_results == NULL) {
		return false;
	}
	
	const char *  initial_string = "Config Test";
	const float   initial_float  = 1.0f;
	const int32_t initial_int    = 0;
	const bool    bool_array[]   = {false, true, false};
	const char    char_array[]   = {'a', 'b', 'c'};
	
	uni_s *cells = malloc(sizeof(uni_s)* (size_t) ( expected_num_configs * expected_num_variables * 2 ));
	for (int32_t index = 0; index < expected_num_configs; index++) {
		
		char *parameter_string;
		asprintf(&parameter_string, "%s %i", initial_string, index);
				
		multi_config_test_s known_results = {
			.parameter_string = parameter_string,
			.parameter_float  = initial_float + ((float)index * 0.1f),
			.parameter_int    = initial_int + index,
			.parameter_bool   = bool_array[index],
			.parameter_char   = char_array[index]
		};
		
		int32_t cell_index = index*expected_num_variables*2;
		
		cells[cell_index + 0] = (uni_s) {*((uni_u*)(&known_results.parameter_string)), string_e};
		cells[cell_index + 1] = (uni_s) {*((uni_u*)(&known_results.parameter_float )), float_e};
		cells[cell_index + 2] = (uni_s) {*((uni_u*)(&known_results.parameter_int   )), int_e};
		cells[cell_index + 3] = (uni_s) {*((uni_u*)(&known_results.parameter_bool  )), bool_e};
		cells[cell_index + 4].value.c = known_results.parameter_char;
		cells[cell_index + 4].type = char_e;
		
		cell_index = index*expected_num_variables*2 + expected_num_variables;
		
		cells[cell_index + 0] = (uni_s) {*((uni_u*)(&test_results[index]->parameter_string)), string_e};
		cells[cell_index + 1] = (uni_s) {*((uni_u*)(&test_results[index]->parameter_float )), float_e};
		cells[cell_index + 2] = (uni_s) {*((uni_u*)(&test_results[index]->parameter_int   )), int_e};
		cells[cell_index + 3] = (uni_s) {*((uni_u*)(&test_results[index]->parameter_bool  )), bool_e};
		cells[cell_index + 4].value.c = test_results[index]->parameter_char;  
		cells[cell_index + 4].type = char_e;

		pass *= !strcmp(known_results.parameter_string, test_results[index]->parameter_string);
		
		pass *= (known_results.parameter_float == test_results[index]->parameter_float);
		pass *= (known_results.parameter_int   == test_results[index]->parameter_int);
		pass *= (known_results.parameter_bool  == test_results[index]->parameter_bool);
		pass *= (known_results.parameter_char  == test_results[index]->parameter_char);
	}
		
	table_column_s columns[] = {
		{"Known String" , 4, 1},
		{"Known Float"  , 4, 1},
		{"Known Int"    , 4, 1},
		{"Known Bool"   , 4, 1},
		{"Known Char"   , 4, 1},
		{"Loaded String", 4, 1},
		{"Loaded Float" , 4, 1},
		{"Loaded Int"   , 4, 1},
		{"Loaded Bool"  , 4, 1},
		{"Loaded Char"  , 4, 1}
	};
	
	table_s table = {
		expected_num_configs,
		expected_num_variables * 2,
		"Multi Config Test",
		cells,
		columns
	};
	
	printTable(table);
	
	printTestResult(pass, "multi config test.");
	
	free(config_file_path);

	return pass;
}

int main() {
	
	const int32_t verbosity = 3;
	
	//Variables:
	const char *config_directory_name = "configs";	
		 
	bool pass = true;
	
	pass *= 
	testSingleConfig(
		verbosity,
		config_directory_name
	);
	
	pass *= 
	testMultiConfig(
		verbosity,
		config_directory_name
	);
	
	printTestResult(pass, "all tests.");
	
	return 0;
}