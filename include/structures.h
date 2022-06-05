#ifndef IO_STRUCTURES_H
#define IO_STRUCTURES_H

typedef enum Type{
	
	/**
     * Enum to hold type information. Used in universal and multi unions,
	 * and in config loader.
     */
	
	bool_e        ,
	int_e         , 
	int_array_e   ,
	int_jagged_e  ,
	float_e       , 
	float_array_e , 
	float_jagged_e, 
	char_e        ,
	string_e      , 
	string_array_e
} type_e;

typedef enum Necessity{ 
	
	/**
     * Enum to hold neccesity information.
     */
	
	required_e, 
	optional_e, 
	excluded_e
} necessity_e;

typedef struct Parameter{
	
	/**
     * Structure to hold infomation about variable to be loaded from config.
     */
	
	char*         name;
	type_e        type;
	necessity_e   necessity;
	
} parameter_s;

typedef struct Config{
	
	/**
     * Structure to hold config variable grouping.
     */
	
	char*          name;
	necessity_e    name_necessity;
	
	bool           is_superconfig;          

	necessity_e    necessity;
	int32_t        num_defined_parameters;
	
	int32_t        min_inputed_parameters;
	int32_t        max_inputed_parameters;
	parameter_s*   defined_parameters;
	parameter_s    default_parameter;
	
	int32_t        min_num_subconfigs;
	int32_t        max_num_subconfigs;
	struct Config* subconfigs;
	struct Config* default_subconfig;

} config_s;

#endif
