#ifndef IO_CONFIG_H
#define IO_CONFIG_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <inttypes.h>
#include <string.h>

#include "strings.h"

typedef struct construct_s {
	
	size_t  structure_size;
	size_t *structure_map;
	char   *variable_types_string;
	char   *variable_names_string;
} construct_s;

size_t getSizeOfType(
    const char* string
    ) {	

	if (!strcmp(string,"double")){
		
		return sizeof(double);
	} else if (!strcmp(string,"float")){
		
		return sizeof(float);
	} else if (!strcmp(string,"size_t")){
		
		return sizeof(size_t);
	} else if (!strcmp(string, "int")) {
		
		return sizeof(int);
	} else if (!strcmp(string, "uint16_t")){
		
		return sizeof(uint16_t);
	} else if (!strcmp(string, "uint32_t")){
		
		return sizeof(uint32_t);
    } else if (!strcmp(string, "int32_t")){
		
		return sizeof(int32_t);
	} else if (!strcmp(string, "bool")){
		
		return sizeof(bool);
	} else if (!strcmp(string, "char*")){
		
		return sizeof(char*);
	}  else if (!strcmp(string, "float*")){
		
		return sizeof(float*);
    } else if (!strcmp(string, "double*")){
		
		return sizeof(double*);
	} else if (!strcmp(string, "uint32_t*")){
		
		return sizeof(uint32_t*);
	} else if (!strcmp(string, "construct_s*")){
		
		return sizeof(construct_s*);
	} else {
		fprintf(
			stderr, 
			"Warning! Unrecognised variable type: %s in function getSizeOfType. \n", 
			string
		);
		
		return 0;
	}
}

bool isInt(
    const char *string
    ) {

	if (
		((!strcmp(string, "size_t")) || (!strcmp(string, "int"))) || ((!strcmp(string, "uint16_t")) || (!strcmp(string, "uint32_t")))){
		
		return 1;
	}
	else{
	
		return 0;
	}
}

size_t getTotalTypeArraySize(
          char    **type_array, 
    const int32_t   length
    ) {

	size_t total_size = 0;

	for (int32_t index = 0; index < length; index++) {
		
		total_size += getSizeOfType(type_array[index]);
	}
	
	return total_size; 
} 

bool checkTypeOrder(
          char    ** type_array, 
    const int32_t    length
   ) {

	size_t last_value = getSizeOfType(type_array[0]);;
    size_t value;

	for (int32_t index = 1; index < length - 1; index++ ){
    
        value = getSizeOfType(type_array[0]);
		
		if(getSizeOfType(type_array[index]) > last_value){
			
			fprintf(stderr, 
				"Error! Struture elements must be aranged in decreasing size to ensure no padding is added! Exiting. Types: %s: %zu > %s: %zu. \n", 
				type_array[index], 
				getSizeOfType(type_array[index]), 
				type_array[index - 1], 
				last_value
			);
			
			exit(1);
		}
        
        last_value = value;
	}
	
	return 1; 
} 

void castToVoid(
    const char     * value_string  ,
          char    ** type_array    , 
    const int32_t    index         , 
    const char     * name          , 
          void     * config_struct
    ){

	size_t  cum_size    = getTotalTypeArraySize(type_array, index);
	char   *type_string = type_array[index];

	if ( !strcmp("double", type_string)) {
	
		double value = (double) atof(value_string);
		memcpy((uint8_t*) &((char*) config_struct)[cum_size], &value, sizeof(double));
  	} else if ( !strcmp("float", type_string)) {
	
		float value = (float) atof(value_string);
		memcpy((uint8_t*) &((char*) config_struct)[cum_size], &value, sizeof(float));
	} else if ( !strcmp("int", type_string)) {
	
		int value = (int) atoi(value_string);
		memcpy((uint8_t*) &((char*) config_struct)[cum_size], &value, sizeof(int));
	} else if ( !strcmp("size_t", type_string)) {
	
		size_t value = (size_t) atoi(value_string);		
		memcpy((uint8_t*) &((char*) config_struct)[cum_size], &value, sizeof(size_t));
	} else if ( !strcmp("uint16_t", type_string)) {
	
		uint16_t value = (uint16_t) atoi(value_string);
		memcpy((uint8_t*) &((char*) config_struct)[cum_size], &value, sizeof(uint16_t));
	} else if ( !strcmp("uint32_t", type_string)) {
	
		uint32_t value = (uint32_t) atoi(value_string);
		memcpy((uint8_t*) &((char*) config_struct)[cum_size], &value, sizeof(uint32_t));
    } else if ( !strcmp("int32_t", type_string)) {
	
		int32_t value = (int32_t) atoi(value_string);
		memcpy((uint8_t*) &((char*) config_struct)[cum_size], &value, sizeof(int32_t));
	} else if ( !strcmp("bool", type_string)) {
	
		bool value = (bool) atoi(value_string);
		memcpy((uint8_t*) &((char*) config_struct)[cum_size], &value, sizeof(bool));
	} else if ( !strcmp("char*", type_string)) {
	
		//char* value = malloc(sizeof(&value_string));
		memcpy( &((uint8_t*) config_struct)[cum_size], &value_string, sizeof(char*));
	} else if (!strcmp("uint32_t*", type_string)) {
	
		memcpy( &((uint8_t*) config_struct)[cum_size], &value_string, sizeof(uint32_t*));
	} else {
	
		fprintf(stderr, "Warning! Unrecognised variable type: %s given for variable: %s. Ignoring.\n", type_string, name);
	}
}

int getVariableindex(
    const char     *variable_name, 
    const int32_t   num_variables, 
          char    **variable_array
    ) {	

	//Find variable number:	
	
	int32_t index = 0;
	while (index < num_variables){
	
		if (!strcmp(variable_name, variable_array[index])){
		
			break;
		}
		index++;
	}
	
	return index;
}

void* readConfig(
     const char    *filename         ,  
     const char    *variable_names_o, 
     const char    *variable_types_o, 
     const int32_t  num_variables    , 
     const int32_t  num_configs      , 
     const size_t   mem_aligment     , 
     const size_t   com_struct_size
    ){

	//Paser settings:
	const char * comment            = "#" ;
	const char * new_line           = ";" ;
	const char * variable_separator = "\"";
	const char * value_indicator    = "=" ;
	const char * new_config         = "{" ;
	const char * end_config         = "}" ;
    
    char * variable_names = strdup(variable_names_o);
    char * variable_types = strdup(variable_types_o);

	//Splits variable string by delimiter and removes spaces
	char ** variable_array = NULL;
	removeStringChars(variable_names, " ", &variable_names);
	splitString      (variable_names, num_variables, ",", &variable_array);

	//Splits type string by delimiter and removes spaces
	char** type_array = NULL;
	removeStringChars(variable_types, " ", &variable_types);
	splitString(variable_types, num_variables, ",", &type_array);

	if ( checkTypeOrder(type_array, num_variables) ) {

	 	size_t struct_size = getTotalTypeArraySize(type_array, num_variables); //<-- Calculate total structure size

		//Opening file:
		FILE* file;
		file = fopen(filename, "r");

		if (file == NULL){ 

			if( filename != NULL){
				fprintf(stderr, "Warning! Could not find config file: \"%s\"! Exiting.\n", filename); exit(1); 
			} 
			else {
				fprintf(stderr, "Invalid filename. Exiting.");
			}
		}

		void** config_structs = malloc(sizeof(void*) * (size_t) num_configs);
		
		for (int32_t config_index = 0; config_index < num_configs; config_index++) {
		
			config_structs[config_index] = calloc(com_struct_size, 1);
		}

		if (struct_size > com_struct_size){
			
			fprintf(
				stderr, 
				"Error! Inputted struct size: (%zu) is larger than compiled structure: (%zu), exiting.\n", 
				struct_size, 
				com_struct_size
			);
			exit(1);
		} else if ((com_struct_size >= 8) && (struct_size < com_struct_size - mem_aligment)){
			
			fprintf(
				stderr, 
				"Warning! Inputted struct size: (%zu) is smaller than compiled structure, even acounting for padding: (%zu), exiting.\n", 
				struct_size, 
				com_struct_size - mem_aligment
			);
			exit(1);
		}

		int32_t   config_index = 0;

		//Reading file:
		size_t    line_length   = 1; 
		bool      in_config     = 0;
		char    * line_string   = NULL; //<-- String which will contain the read in line.

		int32_t   line_index    = 0;

		while (getline(&line_string, &line_length, file) != EOF){

			line_index++;

			bool    variable_read  = 0;
			bool    value_check    = 0;
			int32_t variable_start = 0;

			int32_t variable_index = 0;
			char*   variable_name  = calloc(line_length, 1);

			int32_t char_index = 0;

			while(line_string[char_index] != 0) {

				//Checks for config closing:
				if (in_config && strchr(end_config, line_string[char_index])) {
				
					in_config = 0; config_index++; break;
				} else if (!in_config && strchr(end_config, line_string[char_index])) {
				
					fprintf(stderr, "Warning! Unexpected close config '}'"); break;
				}

				//Checks for new config opening:
				if (!in_config && strchr(new_config, line_string[char_index])) {
				
					in_config = 1; 
					if (config_index >= num_configs){
					
						fprintf(stderr, "Error! More found configs tabs than expected in: %s, exiting! \n", filename);
						exit(1);
					}
					break;
				} else if (in_config && strchr(new_config, line_string[char_index])) {
					fprintf(stderr, "Warning! Unexpected open config '{'"); break;
				}

				//Checks for comment character and starts a new line if found:
				if (strchr(comment, line_string[char_index])) { 				
					char_index++; line_length = 1; break; 
				}

				//Finding start variable names in line:
				if (strchr(variable_separator, line_string[char_index]) && !variable_read){ 	
					variable_start = char_index + 1;
					variable_read = !variable_read;
				}

				//Recording variable name into string:
				if (variable_read && !strchr(variable_separator, line_string[char_index])){
					variable_name[char_index - variable_start] = line_string[char_index];
				}
				//On variable end:
				else if (strchr(variable_separator, line_string[char_index]) && variable_name[0]) {

					//If variable exists:
					bool variable_exists = 0;
					for (int32_t variable_index_2 = 0; variable_index_2 < num_variables; variable_index_2++){
						
						if ((strcmp(variable_array[variable_index_2], variable_name) == 0)){
							//Find variable number:	
							variable_index = getVariableindex(variable_name, num_variables, variable_array);
							value_check = 1;
							variable_exists = 1;
						}

					}
					
					//Warning if variable is not recognised.
					if( !variable_exists ){
						fprintf(stderr, "Warning! Unrecognised variable name: \"%s\".\n", variable_name);
					}
				}

				//Finds the value after detecting value indicatior character
				if (value_check && strchr(value_indicator, line_string[char_index])){

					//Splits string from value indicator onwards until new line symbol.
					char * variable_copy = strdup(&line_string[char_index]);
					char * value_string  = strtok(variable_copy, new_line);
					
					if (!strcmp(type_array[variable_index], "char*")) {
					
						strtok(value_string, "\"");
						value_string = strtok(NULL, "\"");
                        
                        if (value_string == NULL) {
                        
                            fprintf(stderr, "Error! Could not find closing \", for variable %s. \n",  variable_name);
                            exit(1);
                        }
                        
					} else {
					
						//Removes spaces and value indicator
						removeStringChars(value_string, " =", &value_string);
					}

					if ( isInt(type_array[variable_index]) && strchr(".", value_string[0])){
						fprintf(stderr, "Warning! Decimal detected in integer variable: %s, ignoring post decimal values. \n", variable_name);
						value_string = strchr(".", value_string[0]);
					}
					castToVoid(value_string, type_array, variable_index, variable_name, config_structs[config_index]);

					//Resets variable name:
					memset(variable_name, 0, strlen(variable_name));

					value_check = 0;
				}

				char_index++; 
			}

			line_length = 1; 
			value_check = 0;
		}

		free(line_string);
        
        free(variable_names); variable_names = NULL;
        free(variable_types); variable_types = NULL;
		free(variable_array); variable_array = NULL;
		free(type_array)    ; type_array     = NULL;

		if(file != NULL){
		
			fclose(file);
		}

		return config_structs;
	} else {
		
		fprintf(stderr, "Warning! Config structure variables not orgnised in type order. Exiting. \n");
		exit(1);
	}
}

size_t *createStructureVariableMap(
     const char    *type_string, 
     const int32_t  length
    ) {
	
    char *filtered_string;
	removeStringChars(type_string, " ", &filtered_string);
    
    char **type_array = NULL;
	splitString(filtered_string, length, ",", &type_array);
    free(filtered_string);
		
	checkTypeOrder(type_array, length);
	
	size_t *structure_map = malloc(sizeof(size_t) * (size_t) length);
	
	for (int32_t index = 0; index < length; index++) {
		
		structure_map[index] = getTotalTypeArraySize(type_array, index);
	}
	
	free(type_array); type_array = NULL;
	
	return structure_map;
}

void* pullValueFromStruct(
    const void    * structure        , 
    const size_t    variable_position, 
    const size_t    size
    ) {
	
	//Warning check your compiler padding before using this function:
	void* value = malloc(size);
	memcpy(value, &((uint8_t*) structure)[variable_position], size);
	
	return value;
}

void pullUInt16ArrayFromStruct(
    const int32_t        start    , 
    const int32_t        stop     , 
    const construct_s *  construct, 
    const void        *  structure, 
          uint16_t    *  array
    ) {
	
	uint16_t* pointer = NULL;
	
	for (int32_t index = start; index < stop; index++) {
	
		pointer = (uint16_t*) pullValueFromStruct(structure, construct->structure_map[index], sizeof(uint16_t));
		array[index - start] = *pointer;
	}
	
	free(pointer);
}

#endif