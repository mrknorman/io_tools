#ifndef IO_CONFIG_H
#define IO_CONFIG_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <inttypes.h>
#include <string.h>

#include "text.h"
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
	const int32_t  verbosity,
    const type_e  *types, 
    const int32_t  length
   ) {
	
	bool return_value = true;
	
	size_t last_value = getSizeOfType(types[0]);;
    size_t value;

	for (int32_t index = 1; index < length - 1; index++ ){
    
        value = getSizeOfType(types[index]);
		
		if(value > last_value) {
		
			return_value = false;
			
			if (verbosity > 0) {
				fprintf(stderr, 
					"Error! Struture elements must be aranged in decreasing size to ensure no padding is added! Exiting. Types: %s: %zu > %s: %zu. \n", 
					typetoString(types[index]), 
					getSizeOfType(types[index]), 
					typetoString(types[index - 1]), 
					last_value
				);
			}
		}
        
        last_value = value;
	}
	
	return return_value; 
}

bool checkStructSize( 
	const int32_t verbosity,
	const size_t  size,
	const size_t  compiled_size,
	const size_t  largest_memory_alignment
	) {
	
	bool return_value = true;
	
	if (size > compiled_size) {	
		
		if (verbosity > 0) {
			fprintf(
				stderr, 
				"Error! Inputted struct size: (%zu) is larger than compiled structure: (%zu), exiting.\n", 
				size, 
				compiled_size
			);
		}
		
		return_value = false;
	} else if (
			(compiled_size >= 8) 
			&& 
			(size < compiled_size - largest_memory_alignment)
		) {
		
		if (verbosity > 0) {
			fprintf(
				stderr, 
				"Warning! Inputted struct size: (%zu) is smaller than compiled structure, even acounting for padding: (%zu), exiting.\n", 
				size, 
				compiled_size - largest_memory_alignment
			);
		}
		
		return_value = false;
	}

	return return_value;
}

bool checkNumConfigs(
	const int32_t         verbosity,
	const int32_t         num_configs,
	const loader_config_s config_setup
	) {
	
	bool return_value = true;

	if ((num_configs > config_setup.max_num_subconfigs) && (config_setup.max_num_subconfigs > 0)) {
		
		if (verbosity > 0) {
			fprintf(stderr, "Warning! Larger than expected number of configs! Returning Null! \n");
		}
		return_value = false;
	
	} else if (num_configs < config_setup.min_num_subconfigs) {
		
		if (verbosity > 0) {
			fprintf(stderr, "Warning! Smaller than expected number of configs! Returning Null! \n");
		}
		return_value = false;
	}
	
	return return_value;
}

void castToVoid(
	const char   *string,
	const type_e  type,
	      void   *structure
	) {
	
	const size_t size = getSizeOfType(type);
	
	multi_s value_m = StringToMultiS(string, type);
	void *value = (void*) &value_m;
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

char* separateString(
	      char *string,
	const char *separator,
	const char *name
) {

	strtok(string, separator);
	string = strtok(NULL, separator);

	if (string == NULL) {

		fprintf(stderr, "Error! Could not find closing %s, for parameter %s. \n", separator, name);
	}
	
	return string;
}


char* pullValueFromLine(
	const char   *parameter_start,
	const char   *new_line,
	const char   *string_separator,
	const char   *char_separator,
	const char   *parameter_name,
	const type_e  type
) {
	//Splits string from value indicator onwards until new line symbol.
	char * parameter_copy = strdup(parameter_start);
	char * value_string   = strtok(parameter_copy, new_line);
	
	switch (type) {
		
		case (string_e):
			value_string = separateString(value_string, string_separator, parameter_name);
		break;
		
		case (char_e):
			value_string = separateString(value_string, char_separator, parameter_name);	
		break; 
		
		default:
			removeStringChars(value_string, " =", &value_string);
		break;
	}
	
	return value_string;
}

void* readConfig(
	 const int32_t          verbosity,
     const char            *file_name,  
	 const loader_config_s  config_setup,
	       int32_t*         ret_num_configs
    ){
	 
	// Paser settings:
	const char *comment             = "#";
	const char *new_line            = ";";
	const char *parameter_separator = "\"";
	const char *value_indicator     = "=";
	const char *new_config          = "{";
	const char *end_config          = "}";
	const char *string_separator    = "\"";
	const char *char_separator      = "\'";
	
	const int32_t initial_num_configs = 1;
		  int32_t config_index = 0;
		  int32_t num_configs = initial_num_configs;
		   
	//Derived Parameters:
	const size_t       largest_memory_alignment = sizeof(double);
	const int32_t      num_defined_parameters   = config_setup.num_defined_parameters;
	const size_t       compiled_struct_size     = config_setup.struct_size;
	const parameter_s *defined_parameters       = config_setup.defined_parameters;
	
	void **config_structures = NULL;
	
	dict_s* extra_parameters = makeDictionary(num_defined_parameters*100);
	
	// Create lists:
	type_e types[num_defined_parameters];
	char* names[num_defined_parameters];
	for (int32_t index = 0; index < num_defined_parameters; index++) {
		
		names[index] = defined_parameters[index].name;
		types[index] = defined_parameters[index].type;
	}
	
	// Create parameter name maping:
	map_s name_map = createMap(names, num_defined_parameters);
	
	// Calculate struct size from types list:
	const size_t struct_size = 
		getTotalTypeArraySize(types, num_defined_parameters);
	
	// Opening file:
	FILE* file;
	bool file_opened = 
		checkOpenFile(verbosity, file_name, "r", &file);
	
	// Error checking:
	if (!file_opened
		||
		!checkStructSize(
			verbosity, 
			struct_size, 
			compiled_struct_size, 
			largest_memory_alignment
		) ||
		!checkTypeOrder(
			verbosity, 
			types, 
			num_defined_parameters
		)) 
	{	
		num_configs = 0;
		if (verbosity > 0) 
		{
			fprintf(stderr, "Warning! Cannot load config. \n");
		}
	
	} else {
		
		config_structures = 
			malloc(sizeof(void*) * (size_t) num_configs);

		// Setup and reset config wide parameters:
		bool     in_config    = false;
		size_t   line_length  = 1; 
		char    *line_string  = NULL; //<-- String which will contain the read in line.
		int32_t  line_index   = 0;
		
		// Reading file, loop over lines:
		while (getline(&line_string, &line_length, file) != EOF) 
		{

			line_index++;
			
			// Setup and reset line wide parameters:
			bool      parameter_read  = false;
			bool      value_check     = false;
			int32_t   parameter_start = 0;

			int32_t   parameter_index = 0;
			char    * parameter_name  = calloc(line_length, sizeof(char));

			int32_t char_index = 0;
			
			// Reading Line, loop over characters:
			while(line_string[char_index] != 0) 
			{
			
				// Checks for config closing anywhere in line:
				if (in_config && strchr(end_config, line_string[char_index])) 
				{
					in_config = false; 
					config_index++; 
					break;
				} 
				else if (!in_config && strchr(end_config, line_string[char_index])) 
				{
					fprintf(stderr, "Warning! Unexpected config closing '}'\n"); 
					break;
				}

				// Checks for new config opening anywhere in line:
				if (!in_config && strchr(new_config, line_string[char_index]))
				{
					in_config = true; 
					
					// If number of configs greater than allocated in memory,
					// allocate more memory:
					if (config_index >= num_configs)
					{
						num_configs = (int32_t) ceil((float) num_configs * 1.5f);
						config_structures = 
							realloc(config_structures, sizeof(void*) * (size_t) num_configs);
					}
					
					config_structures[config_index] = 
						calloc(1, compiled_struct_size);
					break;	
				} 
				else if (in_config && strchr(new_config, line_string[char_index])) 
				{
					fprintf(stderr, "Warning! Unexpected config opening '{'"); 
					break;
				}

				// Checks for comment character and starts a new line if found:
				if (strchr(comment, line_string[char_index])) 
				{ 	
					break; 
				}

				// Finding start parameter names in line:
				if (strchr(parameter_separator, line_string[char_index]) && !parameter_read) 
				{ 	
					parameter_start = char_index + 1;
					parameter_read = !parameter_read;
				}

				// Recording parameter name into string:
				if (parameter_read && !strchr(parameter_separator, line_string[char_index])) 
				{	
					parameter_name[char_index - parameter_start] = line_string[char_index];
				}
				
				// On parameter name end:
				else if (strchr(parameter_separator, line_string[char_index]) && parameter_name[0]) 
				{
					
					parameter_index = getMapIndex(name_map, parameter_name);
										
					if (parameter_index > -1) 
					{
						value_check = true;
					} 
					else 
					{	
						fprintf(stderr, "Warning! Unrecognised parameter name: \"%s\".\n", parameter_name);
					}
				}

				//Finds the value after detecting value indicatior character
				if (value_check && strchr(value_indicator, line_string[char_index])) 
				{
									
					char* value_string = 
					pullValueFromLine(
						&line_string[char_index],
						new_line,
						string_separator,
						char_separator,
						parameter_name,
						types[parameter_index] 
					);
					
					castToVoidArray(
						value_string, 
						types, 
						parameter_index, 
						config_structures[config_index]
					);

					//Resets parameter name:
					memset(parameter_name, 0, strlen(parameter_name));

					value_check = false;
				}

				char_index++; 
			}
		}

		free(line_string);
		if(file != NULL)
		{
			fclose(file);
		}
	}
	
	if (!checkNumConfigs(verbosity, config_index, config_setup)) 
	{
		free(config_structures);
		config_structures = NULL; config_index = 0;
	}
	
	*ret_num_configs = config_index;
	return config_structures;
}

size_t *createStructureParameterMap(
	 const int32_t  verbosity,
     const type_e  *types, 
     const int32_t  length
    ) {
    
	checkTypeOrder(verbosity, types, length);
	
	size_t *structure_map = malloc(sizeof(size_t) * (size_t) length);
	
	for (int32_t index = 0; index < length; index++)
	{
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
	void *value = malloc(size);
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
	
	uint16_t *pointer = NULL;
	
	for (int32_t index = start; index < stop; index++) 
	{
		pointer = (uint16_t*) pullValueFromStruct(structure, construct->structure_map[index], sizeof(uint16_t));
		array[index - start] = *pointer;
	}
	
	free(pointer);
}

#endif