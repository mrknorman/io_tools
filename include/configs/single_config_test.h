//Basic test config:
typedef struct SingleConfigTest{
	
	char    *parameter_string;
	float    parameter_float;
	int32_t  parameter_int;
	bool     parameter_bool;
	char     parameter_char;

} single_config_test_s;

const int32_t num_defined_parameters = 5;
parameter_s defined_parameters[] = {
	{"parameter_string", string_e, required_e},
	{"parameter_float" , float_e , required_e},
	{"parameter_int"   , int_e   , required_e},
	{"parameter_bool"  , bool_e  , required_e},	
	{"parameter_char"  , char_e  , required_e}	
};

parameter_s default_parameter = 
	{"default_paramter", int_e, excluded_e};

loader_config_s test_config = {
	.name                   = "simple_config_example",
	.name_necessity         = required_e,

	.is_superconfig         = false,
	.necessity              = required_e,

	.num_defined_parameters = num_defined_parameters,
	.min_inputed_parameters = 0,
	.max_inputed_parameters = num_defined_parameters,
	.defined_parameters     = defined_parameters,
	.default_parameter      = default_parameter,
		
	.struct_size            = sizeof(single_config_test_s)
};
