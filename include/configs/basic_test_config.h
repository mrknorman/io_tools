//Basic test config:
typedef struct BasicConfigTest{

	char   *parameter_1; 

	float   parameter_2;
	float   parameter_3;

	int32_t parameter_4;
} basic_test_config_s;

const int32_t num_defined_parameters = 4;
parameter_s defined_parameters[] = {
	{"parameter_1", string_e, required_e},
	{"parameter_2", float_e , optional_e},
	{"parameter_3", float_e , excluded_e},
	{"parameter_4", int_e   , required_e},	
};

parameter_s default_parameter = 
	{"default_paramter", int_e, excluded_e};

config_s test_config = {
	.name                   = "simple_config_example",
	.name_necessity         = required_e,

	.is_superconfig         = false,
	.necessity              = required_e,

	.num_defined_parameters = num_defined_parameters,
	.min_inputed_parameters = 0,
	.max_inputed_parameters = num_defined_parameters,
	.defined_parameters     = defined_parameters,
	.default_parameter      = default_parameter
};
