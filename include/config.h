#ifndef IO_CONFIG_H
#define IO_CONFIG_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <inttypes.h>
#include <string.h>

#include "strings.h"
#include "custom_types.h"
#include "structures.h"

typedef struct construct_s {
	
	size_t       structure_size;
	size_t      *structure_map;
	parameter_s *parameters;
	
} construct_s;


size_t getTotalTypeArraySize(
    const type_e   *types, 
    const int32_t   length
    ) {

	size_t total_size = 0;

	for (int32_t index = 0; index < length; index++) {
		
		total_size += getSizeOfType(types[index]);
	}
	
	return total_size; 
} 

bool checkTypeOrder(
    const type_e    *types, 
    const int32_t    length
   ) {

	size_t last_value = getSizeOfType(types[0]);;
    size_t value;

	for (int32_t index = 1; index < length - 1; index++ ){
    
        value = getSizeOfType(types[index]);
		
		if(value > last_value){
			
			fprintf(stderr, 
				"Error! Struture elements must be aranged in decreasing size to ensure no padding is added! Exiting. Types: %s: %zu > %s: %zu. \n", 
				typetoString(types[index]), 
				getSizeOfType(types[index]), 
				typetoString(types[index - 1]), 
				last_value
			);
			
			exit(1);
		}
        
        last_value = value;
	}
	
	return 1; 
} 

void castToVoid(
	const char   *string,
	const type_e  type,
	      void   *structure
	) {
	
	const size_t size = getSizeOfType(type);
	
	void *value = StringToVoid(string, type);
	memcpy(structure, value, size);
}


void castToVoidArray(
    const char    *value_string,
    const type_e  *types, 
    const int32_t  index, 
          void    *structure
    ){

	const size_t structure_position = getTotalTypeArraySize(types, index);
	const type_e type               = types[index];
	
	castToVoid(value_string, type, (uint8_t*) &((char*) structure)[structure_position]);
}

void* readConfig(
	 const int32_t          verbosity,
     const char            *file_name,  
	 const loader_config_s  config_setup
    ){
	
	//Opening file:
	FILE* file;
	if ( !checkOpenFile(verbosity, file_name, "r", &file) ) {

		fprintf(stderr, "Could not load config: %s Exiting!", file_name);
		exit(1);
	}
	
	const size_t largest_memory_alignment = sizeof(double);
 
	//Paser settings:
	const char *comment             = "#";
	const char *new_line            = ";";
	const char *parameter_separator = "\"";
	const char *value_indicator     = "=";
	const char *new_config          = "{";
	const char *end_config          = "}";
    
	const int32_t      num_defined_parameters = config_setup.num_defined_parameters;
	const size_t       compiled_struct_size   = config_setup.struct_size;
	const parameter_s *defined_parameters     = config_setup.defined_parameters;
	const int32_t      num_configs            = 1;
	
	type_e types[num_defined_parameters];
	for (int32_t index = 0; index < num_defined_parameters; index++) {
		
		types[index] = defined_parameters[index].type;
	}
	
	if ( checkTypeOrder(types, num_defined_parameters) ) {

	 	const size_t struct_size = getTotalTypeArraySize(types, num_defined_parameters); //<-- Calculate total structure size
		
		void **config_structures = malloc(sizeof(void*) * (size_t) num_configs);
		
		for (int32_t index = 0; index < num_configs; index++) {
		
			config_structures[index] = 
				calloc(compiled_struct_size, largest_memory_alignment);
		}

		if (struct_size > compiled_struct_size){
			
			fprintf(
				stderr, 
				"Error! Inputted struct size: (%zu) is larger than compiled structure: (%zu), exiting.\n", 
				struct_size, 
				compiled_struct_size
			);
			exit(1);
		} else if ((compiled_struct_size >= 8) && (struct_size < compiled_struct_size - largest_memory_alignment)){
			
			fprintf(
				stderr, 
				"Warning! Inputted struct size: (%zu) is smaller than compiled structure, even acounting for padding: (%zu), exiting.\n", 
				struct_size, 
				compiled_struct_size - largest_memory_alignment
			);
			exit(1);
		}

		int32_t  config_index = 0;
		bool     in_config    = 0;
		size_t   line_length  = 1; 
		char    *line_string  = NULL; //<-- String which will contain the read in line.
		int32_t  line_index   = 0;
		
		//Reading file:
		while (getline(&line_string, &line_length, file) != EOF){

			line_index++;

			bool    parameter_read  = 0;
			bool    value_check     = 0;
			int32_t parameter_start = 0;

			int32_t parameter_index = 0;
			char*   parameter_name  = calloc(line_length, sizeof(char));

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
					
						fprintf(stderr, "Error! More found configs tabs than expected in: %s, exiting! \n", file_name);
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

				//Finding start parameter names in line:
				if (strchr(parameter_separator, line_string[char_index]) && !parameter_read){ 	
					parameter_start = char_index + 1;
					parameter_read = !parameter_read;
				}

				//Recording parameter name into string:
				if (parameter_read && !strchr(parameter_separator, line_string[char_index])){
					parameter_name[char_index - parameter_start] = line_string[char_index];
				}
				//On parameter end:
				else if (strchr(parameter_separator, line_string[char_index]) && parameter_name[0]) {

					//If parameter exists:
					bool parameter_exists = 0;
					
					for (parameter_index = 0; parameter_index < num_defined_parameters; parameter_index++){
						
						if ((strcmp(defined_parameters[parameter_index].name, parameter_name) == 0)){
							//Find parameter number:	
							value_check = 1;
							parameter_exists = 1;
							
							break;
						}
					}
					
					//Warning if parameter is not recognised.
					if( !parameter_exists ){
						fprintf(stderr, "Warning! Unrecognised parameter name: \"%s\".\n", parameter_name);
					}
				}

				//Finds the value after detecting value indicatior character
				if (value_check && strchr(value_indicator, line_string[char_index])){

					//Splits string from value indicator onwards until new line symbol.
					char * parameter_copy = strdup(&line_string[char_index]);
					char * value_string  = strtok(parameter_copy, new_line);
					
					if (types[parameter_index] == string_e) {
					
						strtok(value_string, "\"");
						value_string = strtok(NULL, "\"");
                        
                        if (value_string == NULL) {
                        
                            fprintf(stderr, "Error! Could not find closing \", for parameter %s. \n",  parameter_name);
                            exit(1);
                        }
                        
					} else {
					
						//Removes spaces and value indicator
						removeStringChars(value_string, " =", &value_string);
					}

					if ( isTypeInt(types[parameter_index]) && strchr(".", value_string[0])){
						fprintf(stderr, "Warning! Decimal detected in integer parameter: %s, ignoring post decimal values. \n", parameter_name);
						value_string = strchr(".", value_string[0]);
					}
					castToVoidArray(
						value_string, 
						types, 
						parameter_index, 
						config_structures[config_index]
					);

					//Resets parameter name:
					memset(parameter_name, 0, strlen(parameter_name));

					value_check = 0;
				}

				char_index++; 
			}

			line_length = 1; 
			value_check = 0;
		}

		free(line_string);
		if(file != NULL){
		
			fclose(file);
		}

		return config_structures;
	} else {
		
		fprintf(stderr, "Warning! Config structure parameter not orgnised in type order. Exiting. \n");
		exit(1);
	}
}

size_t *createStructureParameterMap(
     const type_e  *types, 
     const int32_t  length
    ) {
    
	checkTypeOrder(types, length);
	
	size_t *structure_map = malloc(sizeof(size_t) * (size_t) length);
	
	for (int32_t index = 0; index < length; index++) {
		
		structure_map[index] = getTotalTypeArraySize(types, index);
	}
	
	return structure_map;
}

void* pullValueFromStruct(
    const void    *structure        , 
    const size_t   parameter_position, 
    const size_t   size
    ) {
	
	//Warning check your compiler padding before using this function:
	void* value = malloc(size);
	memcpy(value, &((uint8_t*) structure)[parameter_position], size);
	
	return value;
}

void pullUInt16ArrayFromStruct(
    const int32_t      start    , 
    const int32_t      stop     , 
    const construct_s *construct, 
    const void        *structure, 
          uint16_t    *array
    ) {
	
	uint16_t* pointer = NULL;
	
	for (int32_t index = start; index < stop; index++) {
	
		pointer = (uint16_t*) pullValueFromStruct(structure, construct->structure_map[index], sizeof(uint16_t));
		array[index - start] = *pointer;
	}
	
	free(pointer);
}

#endif