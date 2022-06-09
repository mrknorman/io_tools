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

//Multi Config Test:
const int32_t num_defined_parameters = 5;
parameter_s defined_parameters[] = 
{
	{"parameter_string", string_e, 1, 1},
	{"parameter_float" , float_e , 1, 1},
	{"parameter_int"   , int_e   , 1, 1},
	{"parameter_bool"  , bool_e  , 1, 1},	
	{"parameter_char"  , char_e  , 1, 1}	
};

parameter_s default_parameter = 
	{"default_paramter", int_e, 0, 0};

loader_config_s loader_config = 
{
	.name                   = "simple_config_example",
	.name_necessity         = required_e,

	.is_superconfig         = false,
	.necessity              = required_e,

	.num_defined_parameters = num_defined_parameters,
	.min_inputed_parameters = 0,
	.max_inputed_parameters = num_defined_parameters,
	.defined_parameters     = defined_parameters,
	.default_parameter      = default_parameter,

	.max_num_subconfigs     = 3,
	.min_num_subconfigs     = 3,

	.struct_size            = sizeof(test_config_s)
};

#endif
