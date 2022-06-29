#ifndef TEST_CONFIG_STRUCT
#define TEST_CONFIG_STRUCT
//Basic test config:
typedef struct ConfigTest{
	
	char    *parameter_string;
	float    parameter_float;
	int32_t  parameter_int;
	bool     parameter_bool;
	char     parameter_char;

} test_config_s;

#else

const int32_t num_defined_parameters = 5;
parameter_s defined_parameters[] = 
{
	{"parameter_string", string_e, 0, INT32_MAX},
	{"parameter_float" , float_e , 0, INT32_MAX},
	{"parameter_int"   , int_e   , 0, INT32_MAX},
	{"parameter_bool"  , bool_e  , 0, INT32_MAX},	
	{"parameter_char"  , char_e  , 0, INT32_MAX}	
};

parameter_s default_parameter = 
	{"default_parameter", none_e, 0, 2};

loader_config_s loader_config = 
{
	.name                   = "simple_config_example",
	.name_necessity         = required_e,

	.is_superconfig         = false,
	.min                    = 1,
    .max                    = 1,

	.num_defined_parameters = num_defined_parameters,
	.min_inputed_parameters = 0,
	.max_inputed_parameters = num_defined_parameters,
	.defined_parameters     = defined_parameters,
	
	.min_extra_parameters   = 1,
	.max_extra_parameters   = 5,
	.default_parameter      = default_parameter,

	.struct_size            = sizeof(test_config_s)
};

#endif