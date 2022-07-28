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

// Parameters:
const int32_t num_defined_parameters = 5;
parameter_s defined_parameters[] = 
{
	{"parameter_string", string_e, 0, INT32_MAX, -FLT_MAX, FLT_MAX},
	{"parameter_float" , float_e , 0, INT32_MAX, -FLT_MAX, FLT_MAX},
	{"parameter_int"   , int_e   , 0, INT32_MAX, -FLT_MAX, FLT_MAX},
	{"parameter_bool"  , bool_e  , 0, INT32_MAX, -FLT_MAX, FLT_MAX},	
	{"parameter_char"  , char_e  , 0, INT32_MAX, -FLT_MAX, FLT_MAX}	
};

parameter_s default_parameter = 
	{"default_parameter", none_e, 0, 2, -FLT_MAX, FLT_MAX};

// Subconfigs:
loader_config_s subconfig = 
{
	.name                   = "variable_config_test",
	.name_necessity         = required_e,

	.is_superconfig         = false,
	.has_parameters         = true,
    .min                    = 0,
    .max                    = 1,
	.exit_on_index          = INT32_MAX,
    
	.num_defined_parameters = num_defined_parameters,
	.min_inputed_parameters = 0,
	.max_inputed_parameters = num_defined_parameters,
	.defined_parameters     = defined_parameters,
	
	.min_extra_parameters   = 1,
	.max_extra_parameters   = INT32_MAX,
	.default_parameter      = default_parameter,

	.struct_size            = sizeof(test_config_s)
};

const int32_t num_defined_subconfigs = 5;
loader_config_s defined_subconfigs[5];

for (int32_t index = 0; index < num_defined_subconfigs; index++)
{
    defined_subconfigs[index] = subconfig;
    
    char *name; asprintf(&name, "extra_parameter_config_test_%i", index);
    defined_subconfigs[index].name = name;
}

loader_config_s default_subconfig[] = {subconfig};

loader_config_s loader_config = 
{
	.name                   = "simple_config_example",
	.name_necessity         = required_e,

	.is_superconfig         = true,
	.has_parameters         = false,
    .reorder                = false,
    
    .min                    = 1,
    .max                    = 1,
	.exit_on_index          = INT32_MAX,
    
	.num_defined_parameters = num_defined_parameters,
	.min_inputed_parameters = 0,
	.max_inputed_parameters = num_defined_parameters,
	.defined_parameters     = defined_parameters,
	
	.min_extra_parameters   = 1,
	.max_extra_parameters   = 5,
	.default_parameter      = default_parameter,
    
    .num_defined_subconfigs = num_defined_subconfigs,
	.min_num_subconfigs     = 1,
	.max_num_subconfigs     = 5,
	.defined_subconfigs     = defined_subconfigs,
    
    .min_extra_subconfigs   = 0,
	.max_extra_subconfigs   = 0,
	.default_subconfig      = default_subconfig,

	.struct_size            = sizeof(test_config_s)
};

#endif