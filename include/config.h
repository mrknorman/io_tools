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

void castToVoid(
	const int32_t verbosity,
	const char   *string,
	const type_e  type,
	      void   *structure
	) {
	
	const size_t size = getSizeOfType(type);
	
	multi_s value_m = StringToMultiS(verbosity, string, type);
	void *value = (void*) &value_m;
	memcpy(structure, value, size);
}


void castToVoidArray(
	const int32_t  verbosity,
	const char    *value_string,
    const type_e  *types, 
    const int32_t  index, 
          void    *structure
    ){
	
	const type_e type = types[index];
	const size_t structure_position = getTotalTypeArraySize(types, index);	
	
	castToVoid(
		verbosity,
		value_string, 
		type, 
		(uint8_t*) &((char*) structure)[structure_position]
	);
}

char* separateString(
	      char *string,
	const char *separator,
	const char *name
	) {
	strtok(string, separator);
	string = strtok(NULL, separator);

	if (string == NULL) {

		fprintf(
            stderr, 
            "Error! Could not find closing %s, for parameter %s. \n", 
            separator, 
            name
        );
		
		string = "";
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
	
	switch (type) 
	{	
		case (none_e):
		
			strtok(value_string, string_separator);
			char* string_c = strtok(NULL, string_separator);
			
            if (string_c == NULL) 
            {	
                strtok(value_string, char_separator);
				string_c = strtok(NULL, string_separator);
				
                if (string_c == NULL) 
                {
					removeStringChars(
                        value_string, 
                        " =", 
                        &value_string
                    );
				} 
                else 
                {
					value_string = string_c;
				}
			} 
            else 
            {
				value_string = string_c;
			} 
		break;
	
		case (string_e):
			value_string = 
				separateString(
                    value_string, 
                    string_separator, 
                    parameter_name
                );
		break;
		
		case (char_e):
			value_string =
				separateString(
                    value_string, 
                    char_separator, 
                    parameter_name
                );	
		break; 
		
		default:
			removeStringChars(
                value_string, 
                " =", 
                &value_string
            );
		break;
	}
	
	return value_string;
}

bool checkNumParameters(
	const int32_t  verbosity,
	const int32_t *num_read,
	const int32_t  num_parameters,
	const int32_t  min,
	const int32_t  max
	) {
	
	bool pass = true;
	
	int32_t sum = 0;
	for (int32_t index = 0; index < num_parameters; index++) 
	{
		sum += num_read[index];
	}
	
	if (sum > max) 
	{
		pass *= false;
		
		if (verbosity > 0)
		{
			fprintf(
				stderr, 
				"Error! Total num variables (%i) greater than max allowed (%i)! \n",
				sum, max
			); 
		}
	}
	else if (sum < min) 
	{
		pass *= false;
		
		if (verbosity > 0)
		{
			fprintf(
				stderr, 
				"Error! Total num variables (%i) smaller than minimum required (%i)! \n",
				sum, min
			); 
		}
	} 
	
	return pass;
}

bool checkNumConfigs(
	const int32_t  verbosity,
	const int32_t  total_num_read,
	const int32_t  min,
	const int32_t  max
	) {
	
	bool pass = true;
	
	if (total_num_read > max) 
	{
		pass *= false;
		
		if (verbosity > 0)
		{
			fprintf(
				stderr, 
				"Error! Total num configs (%i) greater than max allowed (%i)! \n",
				total_num_read, max
			); 
		}
	}
	else if (total_num_read < min) 
	{
		pass *= false;
		
		if (verbosity > 0)
		{
			fprintf(
				stderr, 
				"Error! Total num configs (%i) smaller than minimum required (%i)! \n",
				total_num_read, min
			); 
		}
	} 
	
	return pass;
}

bool checkNumExtraConfigs(
	const int32_t  verbosity,
	const int32_t  total_num_read,
	const int32_t  min,
	const int32_t  max
	) {
	
	bool pass = true;
	
	if (total_num_read > max) 
	{
		pass *= false;
		
		if (verbosity > 0)
		{
			fprintf(
				stderr, 
				"Error! Total num extra configs (%i) greater than max allowed (%i)! \n",
				total_num_read, max
			); 
		}
	}
	else if (total_num_read < min) 
	{
		pass *= false;
		
		if (verbosity > 0)
		{
			fprintf(
				stderr, 
				"Error! Total num extra configs (%i) smaller than minimum required (%i)! \n",
				total_num_read, min
			); 
		}
	} 
	
	return pass;
}

bool checkNumExtraParameters(
	const int32_t  verbosity,
	const dict_s  *num_extra_parameters,
	const int32_t  min,
	const int32_t  max
	) {
	
	bool pass = true;
	
	dict_entry_s **entries = 
		returnAllEntries(num_extra_parameters); 
	const int32_t num_entries = 
		num_extra_parameters->num_entries;
	
	int32_t sum = 0;
	for (int32_t index = 0; index < num_entries; index++) 
	{
		sum += entries[index]->data.value.i;
	}
	
	if (sum > max) 
	{
		pass *= false;
		
		if (verbosity > 0)
		{
			fprintf(
				stderr, 
				"Error! Total num extra variables (%i) greater than max allowed (%i)! \n",
				sum, max
			); 
		}
	} 
	else if (sum < min) 
	{
		pass *= false;
		
		if (verbosity > 0)
		{
			fprintf(
				stderr, 
				"Error! Total num extra variables (%i) smaller than minimum required (%i)! \n",
				sum, min
			); 
		}
	} 
	
	return pass;
}

bool checkParameterRequirements(
	const int32_t          verbosity,
	const int32_t         *num_read,
	const parameter_s     *parameters,
	const int32_t          num_parameters,
    const loader_config_s  config
	) {
	
    const int32_t total_min = config.min_inputed_parameters;
    const int32_t total_max = config.max_inputed_parameters;
    
	bool pass = true;
	
	for (int32_t index = 0; index < num_parameters; index++) 
	{
	
		const parameter_s  parameter = parameters[index];
		const int32_t      min       = parameter.min;
		const int32_t      max       = parameter.max;
		const char        *name      = parameter.name;
		const int32_t      read      = num_read[index];
		
		if (read < min) 
		{
			fprintf(
				stderr, 
				"Warning! Num instances (%i) of variable \"%s\" lower than required (%i)! \n", 
				read, name, min
			);
			
			pass *= false;
		}
		else if (read > max)
		{
			fprintf(
				stderr, 
				"Warning! Num instances (%i) of variable \"%s\" higher than allowed (%i) \n", 
				read, name, max
			);
			
			pass *= false;
		}
	}
	
	pass *= 
		checkNumParameters(
			verbosity,
			num_read,
			num_parameters,
			total_min,
			total_max
		);
	
	return pass;
}

bool checkExtraParameterRequirments(
	const int32_t          verbosity,
	const parameter_s      parameter,
	const dict_s          *num_extra_parameters,
    const loader_config_s  config
	) {
    
    const int32_t total_min = config.min_extra_parameters;
    const int32_t total_max = config.max_extra_parameters;
	
	bool pass = true;
	
	const int32_t min = parameter.min;
	const int32_t max = parameter.max;
	
	dict_entry_s **entries = 
		returnAllEntries(num_extra_parameters); 
	const int32_t num_entries = 
		num_extra_parameters->num_entries;
	
	for (int32_t index = 0; index < num_entries; index++) {
		
		const dict_entry_s *entry = entries[index];
		const char         *name  = entry->string_key;
		const int32_t       read  = entry->data.value.i;
				
		if (read < min) 
		{
			fprintf(
				stderr, 
				"Warning! Num instances (%i) of variable \"%s\" lower than required (%i)! \n", 
				read, name, min
			);
			
			pass *= false;
		}
		else if (read > max)
		{
			fprintf(
				stderr, 
				"Warning! Num instances (%i) of variable \"%s\" higher than allowed (%i) \n", 
				read, name, max
			);
			
			pass *= false;
		}
	}
	
	pass *= 
		checkNumExtraParameters(
			 verbosity,
			 num_extra_parameters,
			 total_min,
			 total_max
		);
	
	return pass;
}

bool checkNameRequirments(
    const bool            verbosity,
    const bool            name_read,
    const loader_config_s config
    ) {
    
    const necessity_e  necessity = config.name_necessity;

    bool pass = false;
    
    switch (necessity) 
    {
        case(required_e):
            pass = name_read;
            
            if ((verbosity > 0) && !pass) 
            {
                fprintf(stderr, "Error! Config Name required. \n");
            }
        break;
        
        case(optional_e):
            pass = true;
        break;
        
        case(excluded_e):
            pass = !name_read;
            
            if ((verbosity > 0) && !pass) 
            {
                fprintf(stderr, "Error! Unnamed configs are set to be excluded. \n");
            }
        break;
        
        default:
            pass = true;
        break;
    }

    return pass;
}

bool checkConfigRequirements(
    const int32_t          verbosity,
    const bool             is_superconfig, 
	const int32_t         *num_read,
    const int32_t          total_num_read,
	const loader_config_s *subconfigs,
	const int32_t          num_subconfigs,
	const loader_config_s  config
    ) {
    
    int32_t total_min = config.min_num_subconfigs;
    int32_t total_max = config.max_num_subconfigs;
    
    int32_t extra_min = config.min_extra_subconfigs;
    int32_t extra_max = config.max_extra_subconfigs;
    
    int32_t sum = 0;
	for (int32_t index = 0; index < num_subconfigs*is_superconfig; index++) 
	{
		sum += num_read[index];
	}
    
    if (!is_superconfig)
    {
        total_min = 0;
        total_max = 1;
        
        extra_min = 0;
        extra_max = 1;
    }
    
	bool pass = true;
	
	for (int32_t index = 0; index < num_subconfigs*is_superconfig; index++) 
	{
	
		const loader_config_s  config = subconfigs[index];
		const int32_t          min    = config.min;
		const int32_t          max    = config.max;
		const char            *name   = config.name;
		const int32_t          read   = num_read[index];
		
		if (read < min) 
		{
			fprintf(
				stderr, 
				"Warning! Num instances (%i) of config \"%s\" lower than required (%i)! \n", 
				read, name, min
			);
			
			pass *= false;
		}
		else if (read > max)
		{
			fprintf(
				stderr, 
				"Warning! Num instances (%i) of config \"%s\" higher than allowed (%i) \n", 
				read, name, max
			);
			
			pass *= false;
		}
	}
	
	pass *= 
		checkNumConfigs(
			verbosity,
			total_num_read,
			total_min,
			total_max
		);
        
    pass *=
        checkNumExtraConfigs(
			verbosity,
			total_num_read - sum,
			extra_min,
			extra_max
		);
    	    
    return pass;
}

type_e guessDataTypeFromString(
	const char *string
	) {

	type_e guess = float_e;
	
	if isdigit(string[0])
	{
		if ( strchr(string, '.') ) 
		{
			guess = float_e;
		} 
		else 
		{
			guess = int_e;
		}
	} 
	else if ((!strcmp(string, "true")) || (!strcmp(string, "false"))) 
	{
		guess = bool_e;	
	} 
	else 
	{
		guess = string_e;
	}
	
	return guess;
}

type_e guessTypeFromConfigContext(
	      char *string,
	const char* string_separator,
	const char* char_separator
	) {
	
	type_e estimated_type;
	if (strchr(string, string_separator[0])) 
	{
		estimated_type = string_e;
		removeStringChars(string, string_separator, &string);
	}
	if (strchr(string, char_separator[0])){
		estimated_type = char_e;
		removeStringChars(string, char_separator, &string);
	} 
	else {
		estimated_type = 
			guessDataTypeFromString(string);
	}
	
	return estimated_type;
}

void addExtraParameter(
	const int32_t  verbosity, 
	      dict_s  *extra_parameters,
	      char    *name,
	const type_e   type,
	const int32_t  index, 
	const int32_t  start_index,
	const int32_t  num_read,
	      char    *value_string
	) {
	
	char* extra_parameter_name = name;
	if (num_read > start_index) 
	{
		asprintf(
			&extra_parameter_name, 
			"%s_%i", 
			name, 
			index - start_index
		);
	} 
	
	multi_s data = 
		StringToMultiS(verbosity, value_string, type);
	insertDictEntry(extra_parameters, data, extra_parameter_name);
}

bool checkDefaultParameter(
	const int32_t     verbosity,
	const parameter_s parameter
	) {
	
	bool pass = true;
	
	const int32_t min  = parameter.min;
	const int32_t max  = parameter.max;
	const char   *name = parameter.name;

	if (min > max) {

		if (verbosity > 0) 
		{
			fprintf(
				stderr, 
				"Error! Min num parameter instances greater than max for parameter: \"%s\"! Returning Null!", 
				name
			);
		}

		pass *= false;
	}

	return pass;
}

bool checkParameters(
	const int32_t      verbosity,
	const parameter_s *parameters,
	const int32_t      num_parameters
	) {
	
	bool pass = true;
	
	for (int32_t index = 0; index < num_parameters; index++) {
		
		const parameter_s parameter = parameters[index];
		
		const int32_t min  = parameter.min;
		const int32_t max  = parameter.max;
		const type_e  type = parameter.type;
		const char   *name = parameter.name;
		
		if (min > max) {
			
			if (verbosity > 0) 
			{
				fprintf(
					stderr, 
					"Error! Min num parameter instances greater than max for parameter: \"%s\"! Returning Null!", 
					name
				);
			}
			
			pass *= false;
		}
		
		if (type == none_e)
		{
			if (verbosity > 0) 
			{
				fprintf(
					stderr, 
					"Error! Defined parameter \"%s\" cannot have type none_e! Returning Null!", 
					name
				);
			}
			
			pass *= false;
		}
	}
	
	return pass;
}

bool checkDefaultConfig(
	const int32_t         verbosity,
    const bool            is_superconfig,
	const loader_config_s config
	) {
	
	bool pass = true;
	
    if (is_superconfig)
    {
    
        const int32_t min  = config.min;
        const int32_t max  = config.max;
        const char   *name = config.name;

        if (min > max) {

            if (verbosity > 0) 
            {
                fprintf(
                    stderr, 
                    "Error! Min num config instances greater than max for config: \"%s\"! Returning Null!", 
                    name
                );
            }

            pass *= false;
        }
    }
    
	return pass;
}

bool checkConfigs(
	const int32_t          verbosity,
    const bool             is_superconfig,
	const loader_config_s *configs,
	const int32_t          num_configs
	) {
	
	bool pass = true;
	
    if (is_superconfig)
    {
        for (int32_t index = 0; index < num_configs; index++) {

            const loader_config_s config = configs[index];

            const int32_t min  = config.min;
            const int32_t max  = config.max;
            const char   *name = config.name;

            if (min > max) {

                if (verbosity > 0) 
                {
                    fprintf(
                        stderr, 
                        "Error! Min num config instances greater than max for config: \"%s\"! Returning Null!", 
                        name
                    );
                }

                pass *= false;
            }
        }
    }
	
	return pass;
}

bool checkLoaderConfig(
    const int32_t         verbosity,
    const loader_config_s config
    ) {
    
    const size_t           largest_memory_alignment = sizeof(double);
    const size_t           compiled_struct_size     = config.struct_size;

    const bool             is_superconfig           = config.is_superconfig;

    const int32_t          num_defined_parameters   = config.num_defined_parameters;
          parameter_s     *defined_parameters       = config.defined_parameters;
          parameter_s      default_parameter        = config.default_parameter;

          int32_t          num_defined_subconfigs   = config.num_defined_subconfigs;
    const loader_config_s *defined_subconfigs       = config.defined_subconfigs;
    
    // Calculate struct size from types list:
    
    type_e  types[num_defined_parameters];
    for (int32_t index = 0; index < num_defined_parameters; index++) 
    {	
        types[index]           = defined_parameters[index].type;
    }
    
	const size_t struct_size = 
		getTotalTypeArraySize(types, num_defined_parameters);
    
    bool pass = true;
    
    loader_config_s  default_config;
    if (is_superconfig)
    {
        default_config = *config.default_subconfig;
    }
    else
    {
        default_config = config;
    }
    	
	pass *=	checkStructSize(
        verbosity, 
        struct_size, 
        compiled_struct_size, 
        largest_memory_alignment
    );

	pass *= checkTypeOrder(
        verbosity, 
        types, 
        num_defined_parameters
    );
    
	pass *= checkDefaultParameter(
        verbosity,
        default_parameter
    );

    pass *= checkParameters(
        verbosity,
        defined_parameters, 
        num_defined_parameters
    ); 
        
    pass *= checkDefaultConfig(
        verbosity,
        is_superconfig,
        default_config
    );
    
    pass *= checkConfigs(
        verbosity,
        is_superconfig,
        defined_subconfigs, 
        num_defined_subconfigs
    );
    
    return pass;
}

map_s createParameterMap(
    const int32_t      num_parameters,
    const parameter_s *parameters
    ) {

	char   *names[num_parameters];
	for (int32_t index = 0; index < num_parameters; index++) 
	{	
		names[index] = parameters[index].name;
	}
    
    // Create parameter name maping:
	map_s map = 
        createMap(names, num_parameters);
    
    return map;
}

map_s createConfigMap(
    const loader_config_s *subconfigs,
    const int32_t          num_subconfigs,
    const loader_config_s  config
    ) {
    
     char **names;
     if (config.is_superconfig == true) 
     {   
        names = malloc(sizeof(char*) * (size_t) num_subconfigs);
        
        for (int32_t index = 0; index < num_subconfigs; index++) 
        {	
            names[index] = subconfigs[index].name;
        } 
    }
    else
    {
        names = malloc(sizeof(char*));
        *names = config.name;
    }
    
    // Create config name maping:
    map_s map = 
        createMap(names, num_subconfigs);
    
    return map;
}

parameter_s *overwriteParameters(
    const parameter_s*     default_parameters,
    const map_s            default_parameter_map,
    const loader_config_s  priority_config
    ) {
    
    const int32_t      num_priority_parameters  = priority_config.num_defined_parameters;
    const parameter_s *priority_parameters      = priority_config.defined_parameters;
    
    parameter_s *new_parameters = malloc(sizeof(parameter_s) * (size_t) default_parameter_map.length);
    memcpy(new_parameters, default_parameters, sizeof(parameter_s) * (size_t) default_parameter_map.length);

    // Create config lists:    
    map_s priority_name_map = 
        createParameterMap(
            num_priority_parameters,
            priority_parameters
        );

    for (int32_t index = 0; index < num_priority_parameters; index++) 
    {
        const char *parameter_name = getMapKey(priority_name_map, index);

        if (parameter_name != NULL) 
        {
            int32_t default_index = getMapIndex(default_parameter_map, parameter_name);

            if (default_index > -1) 
            {
                new_parameters[default_index] = priority_parameters[index];
            } 
            else
            {
                fprintf(stderr, "Warning! Subparameter %s name at not found in defined parameters! \n.", parameter_name);
                break;
            }
        }
        else
        {
            fprintf(stderr, "Warning! Subparameter name at index %i not found, check num defined parameters \n.", index);
            break;
        }
    }
    
    return new_parameters;
}

void freeConfigData(
    loader_data_s superconfig
    ) {
    
    for (int32_t index = 0; index < superconfig.total_num_subconfigs_read; index++) 
    {
        if (superconfig.subconfigs[index].structure != NULL) 
        {
            free(superconfig.subconfigs[index].structure);
        }
        if( superconfig.subconfigs[index].name != NULL)
        {
            free(superconfig.subconfigs[index].name);
        }
    }
    
    if (superconfig.extra_parameters != NULL)
    {
        free(superconfig.extra_parameters);
    }
    
    free(superconfig.subconfigs);
    superconfig.subconfigs = NULL;
}

void **reorderConfigs(
    const loader_config_s    config,
    const loader_data_s superconfig
    ) {
    
    const int32_t num_defined = config.num_defined_subconfigs;

    const map_s             name_map       = superconfig.subconfig_name_map;
    const int32_t          *num_read       = superconfig.num_subconfigs_read;
    const int32_t           total_num_read = superconfig.total_num_subconfigs_read;
    const loader_data_s *subconfig_data = superconfig.subconfigs;
    
    void **config_structs = calloc((size_t) total_num_read, sizeof(void*));
    
    int32_t unique_defined_read = 0;
    int32_t *cumulative_missing = malloc(sizeof(int32_t) * (size_t) (num_defined + 1));
    cumulative_missing[0] = 0;

    for (int32_t index = 0; index < num_defined; index++)
    {
        unique_defined_read += (bool) num_read[index];
        cumulative_missing[index + 1] = 
            cumulative_missing[index] + !((bool) num_read[index]);
    }

    int32_t undefined_index = unique_defined_read;
    for (int32_t index = 0; index < total_num_read; index++) 
    {
        const char  *name = subconfig_data[index].name;

        int32_t defined_index = -1;
        if( name != NULL)
        {
            defined_index = getMapIndex(name_map, name);
        }

        if (defined_index > -1) 
        {
            int32_t adjusted_index = defined_index - cumulative_missing[defined_index];
            config_structs[adjusted_index] = subconfig_data[index].structure; 
        }
        else
        {
            config_structs[undefined_index] = subconfig_data[index].structure; 
            undefined_index++;
        }
    }
    
    return config_structs;
}

void **keepConfigs(
    const loader_data_s superconfig
    ) {
    
    const int32_t        total_num_read = superconfig.total_num_subconfigs_read;
    const loader_data_s *subconfig_data = superconfig.subconfigs;
    
    void **config_structs = calloc((size_t) total_num_read, sizeof(void*));
    
    for (int32_t index = 0; index < total_num_read; index++) 
    {
        config_structs[index] = subconfig_data[index].structure; 
    }
    
    return config_structs;
}

void **setConfigStructs(
    const loader_config_s     config,
    const loader_data_s  superconfig
    ) {
    
    const bool is_superconfig = config.is_superconfig;
    const bool reorder        = config.reorder;
    
    void **config_structs = NULL;
    
    if (is_superconfig)
    {
        if (reorder) 
        {
            config_structs = 
                reorderConfigs(config, superconfig);
        }
        else
        {
            config_structs = 
                keepConfigs(superconfig);
        }
    }
    else
    {
        config_structs = calloc((size_t) 1, sizeof(void*));
        config_structs[0] = superconfig.subconfigs[0].structure; 
    }
    
    return config_structs;
}

loader_data_s setupSuperconfig(
    const loader_config_s config,
    const int32_t         num_configs
    ) {
    
    loader_data_s superconfig = 
    {
        .subconfigs = NULL,
        .config     = config
    };

    // Create config lists:    
    superconfig.subconfig_name_map = 
        createConfigMap(
            config.defined_subconfigs,
            config.num_defined_subconfigs,
            config
        ); 
      
    // Allocate memory for subconfigs:
    superconfig.subconfigs = 
        calloc((size_t) num_configs, sizeof(loader_data_s));
        
    // Create bins to hold number of each parameter name read:
    superconfig.num_subconfigs_read = 
        calloc((size_t) config.num_defined_parameters, sizeof(int32_t));

    return superconfig;
}

loader_config_s setupDefaultConfig(
    const loader_config_s    config 
    ) {

    loader_config_s  default_config;
    if (config.is_superconfig)
    {
        default_config = *config.default_subconfig;
    }
    else
    {
        default_config = config;
    }

    return default_config;
}

typedef struct LoaderSyntax{
    const char *comment;
    const char *new_line;
    const char *value_indicator;
    const char *start_config;
    const char *end_config;
    const char *string_separator;
    const char *char_separator;
    const char *start_name;
    const char *end_name;
} loader_syntax_s;

loader_data_s readSubconfig(
    const int32_t            verbosity,
          loader_syntax_s    syntax,
          int32_t            num_configs,
          int32_t            config_index,
          loader_config_s    config,
          FILE              *file
    ) {
    
    //Derived Parameters:

    const size_t           compiled_struct_size     = config.struct_size;

    const bool             is_superconfig           = config.is_superconfig;

    const int32_t          num_defined_parameters   = config.num_defined_parameters;
          parameter_s     *defined_parameters       = config.defined_parameters;
          parameter_s      default_parameter        = config.default_parameter;

          int32_t          num_defined_subconfigs   = config.num_defined_subconfigs;
    const loader_config_s *defined_subconfigs       = config.defined_subconfigs;
    
    loader_data_s superconfig  = 
            setupSuperconfig(config, num_configs);
    
    if (!checkLoaderConfig(verbosity, config) ) 
    {	
		num_configs = 0;
		if (verbosity > 0) 
		{
			fprintf(stderr, "Warning! Cannot load config. \n");
		}
	} 
	else 
	{
        // Setup Default Configs:
        loader_config_s default_config = 
            setupDefaultConfig(config);
        
        // Create parameter lists:
        type_e  types[num_defined_parameters];
        for (int32_t index = 0; index < num_defined_parameters; index++) 
        {	
            types[index]           = defined_parameters[index].type;
        }

		// Setup and reset config wide parameters:
		bool             in_config     = false;
        bool             name_read     = false;
		size_t           line_length   = 1; 
		char            *line_string   = NULL; //<-- String which will contain the read in line.
		int32_t          line_index    = 0;
        char            *config_name   = NULL;
        
        dict_s  *num_extra_configs    = 
            makeDictionary(num_defined_subconfigs*100);

		// Reading file, loop over lines:
		while (getline(&line_string, &line_length, file) != EOF) 
		{
			line_index++;
			
			// Setup and reset line wide parameters:
			bool     parameter_read       = false;
            bool     parameter_end        = false;
			bool     get_value            = false;
			int32_t  parameter_start      = 0;

			int32_t  parameter_index      = 0;
			type_e   parameter_type       = bool_e;
			char    *parameter_name       = calloc(line_length, sizeof(char));
			bool     parameter_recognised = false;

			int32_t  char_index = 0;
			
			// Reading Line, loop over characters:
			while(line_string[char_index] != 0) 
			{                 
				// Checks for config closing anywhere in line:
				if (in_config && strchr(syntax.end_config, line_string[char_index])) 
				{				
					// Check neccesity requirements:
					if (!checkParameterRequirements(
							verbosity,
							superconfig.subconfigs[config_index].num_parameters_read,
							defined_parameters,
							num_defined_parameters,
							config
						) 
						||
						!checkExtraParameterRequirments(
							verbosity,
							default_parameter,
							superconfig.subconfigs[config_index].num_extra_parameters,
							config
						)
                        ||
                        !checkNameRequirments(
                            verbosity,
                            name_read,
                            config
                        ))
					{
                        superconfig.total_num_subconfigs_read = 0;
						return superconfig;
					}
                    
                    in_config = false; 
					config_index++; 
					
					break;
				} 
				else if (!in_config && strchr(syntax.end_config, line_string[char_index])) 
				{
					fprintf(stderr, "Warning! Unexpected config closing '}'\n"); 
					break;
				}

				// Checks for new config opening anywhere in line:
				if (!in_config && strchr(syntax.start_config, line_string[char_index]))
				{
					in_config   = true; 
                    name_read   = false;
                    config_name = NULL;

					// If number of configs greater than allocated in memory,
					// allocate more memory:
					if (config_index >= num_configs)
					{
                        num_configs = (int32_t) ceil((float) num_configs * 1.5f);
                        superconfig.subconfigs = 
                            realloc(superconfig.subconfigs, sizeof(loader_data_s) * (size_t) num_configs);
					}
                    
                    superconfig.subconfigs[config_index].name = NULL;
					
					superconfig.subconfigs[config_index].structure = 
						calloc(1, compiled_struct_size);
                        
                    // Create parameter name maping:
                    superconfig.subconfigs[config_index].parameter_name_map = 
                        createParameterMap(
                            num_defined_parameters,
                            defined_parameters
                        );
                        
                    superconfig.subconfigs[config_index].num_parameters_read = 
                        calloc((size_t) num_defined_parameters, sizeof(int32_t));
						
					superconfig.subconfigs[config_index].extra_parameters = 
                        makeDictionary(num_defined_parameters*100);   
										
					// Create dictionary to count instances of extra parameters:
					superconfig.subconfigs[config_index].num_extra_parameters = 
						makeDictionary(num_defined_parameters*100);
                    
                    // Intilise subconfig to default:
                    config = 
                        default_config;
                        
                    // Set default parameters:    
                    defined_parameters = overwriteParameters(
                        superconfig.config.defined_parameters,
                        superconfig.subconfigs[config_index].parameter_name_map,
                        config
                    );
					
					break;	
				} 
				else if (in_config && strchr(syntax.start_config, line_string[char_index])) 
				{
					fprintf(stderr, "Warning! Unexpected config opening '{' \n"); 
					break;
				}
                
                // Checks for config name character and reads name if found:
                if (strchr(syntax.start_name, line_string[char_index])) 
				{ 	
                    if (name_read == false) 
                    {
                        config_name = strtok(&line_string[char_index + 1], syntax.end_name);
                        superconfig.subconfigs[config_index].name = strdup(config_name);
                        
                        name_read = true;
                        
                        if (is_superconfig)
                        {
                            const int32_t config_name_index = 
                                getMapIndex(superconfig.subconfig_name_map, config_name);
										
                            if (config_name_index > -1) 
                            {
                                superconfig.num_subconfigs_read[config_name_index]++;
                                config = defined_subconfigs[config_name_index];
                                
                                // Set default parameters:    
                                defined_parameters = overwriteParameters(
                                    superconfig.config.defined_parameters,
                                    superconfig.subconfigs[config_index].parameter_name_map,
                                    config
                                );
                            }
                            else
                            {
                                dict_entry_s* entry = 
                                    findDictEntry_(
                                        num_extra_configs, 
                                        config_name
                                    );

                                if (entry == NULL) 
                                {								
                                    multi_s data;
                                    data.value.i = 1;
                                    data.type = int_e;

                                    insertDictEntry(
                                        num_extra_configs, 
                                        data, 
                                        config_name
                                    );		

                                    data.value.i++;	
                                }
                                else
                                {
                                    entry->data.value.i++;
                                }
                            }
                        }            
                    }
                    else if (verbosity > 1)
                    {
                        fprintf(
                            stderr, 
                            "Warning! More than one name detected inside config %s, ignoring all but first. \n", 
                            config_name
                        );
                    }
					break; 
				}
    
				// Checks for comment character and starts a new line if found:
				if (strchr(syntax.comment, line_string[char_index])) 
				{ 	
					break; 
				}

				// Finding start parameter names in line:
				if (!isspace(line_string[char_index]) && !parameter_read) 
				{ 	
					parameter_start = char_index;
					parameter_read = !parameter_read;
				}
                
				// Recording parameter name into string:
				if (parameter_read && !isspace(line_string[char_index])) 
				{	
					parameter_name[char_index - parameter_start] = 
						line_string[char_index];
				}
				
				// On parameter name end:
				else if (isspace(line_string[char_index]) && (!parameter_end && parameter_read))
				{
                    parameter_end = true;
                    
					parameter_index = 
						getMapIndex(superconfig.subconfigs[config_index].parameter_name_map, parameter_name);
										
					if (parameter_index > -1) 
					{
						parameter_type = superconfig.config.defined_parameters[parameter_index].type;
						superconfig.subconfigs[config_index].num_parameters_read[parameter_index]++;
						parameter_recognised = true;
					} 
					else 
					{	
						parameter_type = default_parameter.type;
						
						dict_entry_s* entry = 
							findDictEntry_(
								superconfig.subconfigs[config_index].num_extra_parameters, 
								parameter_name
							);
						
						if (entry == NULL) 
						{								
							multi_s data;
							data.value.i = 1;
							data.type = int_e;
							
							insertDictEntry(
								superconfig.subconfigs[config_index].num_extra_parameters, 
								data, 
								parameter_name
							);		
							
							data.value.i++;	
						}
						else
						{
							entry->data.value.i++;
						}						
						parameter_recognised = false;
						parameter_index = 0;
						
						if ((config.max_extra_parameters == 0) 
							&&
							(verbosity > 1))
						{
							fprintf(
								stderr, 
								"Warning! Unrecognised parameter name: \"%s\".\n", 
								parameter_name
							);
						}
					}
					
					get_value = true;
				}

				//Finds the value after detecting value indicatior character
				if (get_value && strchr(syntax.value_indicator, line_string[char_index])) 
				{	
					char* value_string = 
					pullValueFromLine(
						&line_string[char_index],
						syntax.new_line,
						syntax.string_separator,
						syntax.char_separator,
						parameter_name,
						parameter_type 
					);
					
					if (parameter_type == none_e) 
					{
						parameter_type = guessTypeFromConfigContext(
							value_string, 
							syntax.string_separator, 
							syntax.char_separator
						);
					}
					
					if (value_string != NULL) 
					{	
						int32_t extra_parameter_start_index = 0;
						
						if     ((parameter_recognised) 
							&& (superconfig.subconfigs[config_index].num_parameters_read[parameter_index] > 1))
						{
							extra_parameter_start_index = 2;
						} 
						else
						{
							extra_parameter_start_index = 1;
						}
						
						if (   (parameter_recognised) 
							&& (superconfig.subconfigs[config_index].num_parameters_read[parameter_index] < 2)) 
						{
							castToVoidArray(
								verbosity,
								value_string, 
								types, 
								parameter_index, 
								superconfig.subconfigs[config_index].structure
							);
						}
						else 
						{
							addExtraParameter(
								verbosity,
								superconfig.subconfigs[config_index].extra_parameters,
								parameter_name,
								parameter_type,
								parameter_index, 
								extra_parameter_start_index, 
								superconfig.subconfigs[config_index].num_parameters_read[parameter_index],
								value_string
							);
						}
					} 
                    else 
                    {
                        if (verbosity > 0)
                        {
                            fprintf(
                                stderr, 
                                "Warning! No value string read! \n"
                            );
                        }
					}

					//Resets parameter name:
					memset(parameter_name, 0, strlen(parameter_name));

					get_value = false;
				}

				char_index++; 
			}
		}
        
        superconfig.total_num_subconfigs_read = config_index;
    }
        
    return superconfig;
}

void* readConfig(
	 const int32_t            verbosity,
     const char              *file_name,  
	 const loader_config_s    config,
	       loader_data_s     *ret_cofig_data
    ){
	
	// Paser settings:
    loader_syntax_s syntax = { 
        .comment          = "#",
        .new_line         = ";",
        .value_indicator  = "=",
        .start_config     = "{",
        .end_config       = "}",
        .string_separator = "\"",
        .char_separator   = "\'",
        .start_name       = "[",
        .end_name         = "]"
    };

        const int32_t initial_num_configs = 1;
		   
	//Derived Parameters:
    
              int32_t          config_index             = 0;
              int32_t          num_configs              = initial_num_configs;

        const bool             is_superconfig           = config.is_superconfig;

              int32_t          num_defined_subconfigs   = config.num_defined_subconfigs;
        const loader_config_s *defined_subconfigs       = config.defined_subconfigs;
        
    // Initlise empty pointer:
        void    *config_structs       = NULL;
        
	// Opening file:
	FILE* file;	
	// Error checking:
    loader_data_s superconfig;
    if (checkOpenFile(verbosity, file_name, "r", &file))
    {
        superconfig = readSubconfig(
            verbosity,
            syntax,
            num_configs,
            config_index,
            config,
            file
        );
        
        config_index = superconfig.total_num_subconfigs_read;
    } 
    
    if ((
        !checkConfigRequirements(
            verbosity,
            is_superconfig,
            superconfig.num_subconfigs_read,
            config_index,
            defined_subconfigs,
            num_defined_subconfigs,
            superconfig.config
        ) )
        ||
        (config_index <= 0))
    {
        freeConfigData(superconfig);

        config_structs = NULL;
        config_index = 0;
    }
    else
    {
        config_structs = setConfigStructs(
            superconfig.config,
            superconfig
        );
        
    }
		
	*ret_cofig_data = superconfig;
	
	return config_structs;
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
