#ifndef DATASET_CONFIG_STRUCTS
#define DATASET_CONFIG_STRUCTS

//Basic test config:

typedef struct Detector{

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	//
	// Structure to hold detector configuration. NOTE: Organise in order of type size. 
	//
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	
	char    *name;
	
	float   *position, *position_sphere;
	int32_t *latitude, *longitude;

	float   *noise_profile_strain, *noise_profile_frequency, *interpolated_noise_profile; 
	float   *x_arm_direction, *y_arm_direction, *up_direction;
	
	float    height_above_sea_level;

    float    sensitivity, noise_amplitude; 

	int32_t  noise_profile_length;
	int32_t  interpolated_noise_profile_length;

	int32_t  x_arm_bearing, y_arm_bearing;
	
	bool     active;
	
} detector_s;

typedef struct NetworkConfig{

	float   speed_of_light;
	float   equatorial_radius;
	float   polar_radius;

	int32_t num_detectors;
	int32_t num_dimensions;

} network_config_s;

#else
//Default Parameters
parameter_s default_parameter = 
	{"default_parameter", none_e, 0, 0};

//Detectors:
const int32_t num_detector_parameters = 19;
parameter_s detector_parameters[] = 
{
	{"name"                             , string_e     , 0, 0},
	{"position"                         , float_array_e, 0, 0},
	{"position_sphere"                  , float_array_e, 0, 0},
	{"latitude"                         , int_array_e  , 1, 1},
	{"longitude"                        , int_array_e  , 1, 1},
	{"noise_profile_strain"             , float_array_e, 0, 0},
    {"noise_profile_frequency"          , float_array_e, 0, 0},
	{"interpolated_noise_profile"       , float_array_e, 0, 0},
	{"x_arm_direction"                  , float_array_e, 0, 0},
	{"y_arm_direction"                  , float_array_e, 0, 0},
	{"up_direction"                     , float_array_e, 0, 0},
	{"height_above_sea_level"           , float_e      , 1, 1},
	{"sensitivity"                      , float_e      , 1, 1},
	{"noise_amplitude"                  , float_e      , 1, 1},
	{"noise_profile_length"             , int_e        , 0, 0},
	{"interpolated_noise_profile_length", int_e        , 0, 0},
	{"x_arm_bearing"                    , int_e        , 1, 1},
	{"y_arm_bearing"                    , int_e        , 1, 1},
	{"active"                           , bool_e       , 1, 1}
};

const int32_t min_num_detector_parameters = 8;
const int32_t max_num_detector_parameters = 8;

loader_config_s detector_subconfig = 
{
	.name                   = "default_detector_name",
	.name_necessity         = optional_e,

	.is_superconfig         = false,
	.has_parameters         = true,
	.inherit                = true,
    .min                    = 1,
    .max                    = INT32_MAX,
    
	.num_defined_parameters = num_detector_parameters,
	.min_inputed_parameters = min_num_detector_parameters,
	.max_inputed_parameters = max_num_detector_parameters,
	.defined_parameters     = detector_parameters,
	
	.min_extra_parameters   = 0,
	.max_extra_parameters   = 0,
	.default_parameter      = default_parameter,

	.struct_size            = sizeof(detector_s)
};

loader_config_s default_detector[] = {detector_subconfig};

loader_config_s detectors_subconfig = 
{
	.name                   = "detectors",
	.name_necessity         = required_e,

	.is_superconfig         = true,
	.has_parameters         = false,
	.inherit                = false,
    .min                    = 1,
    .max                    = INT32_MAX,
    
	.num_defined_parameters = num_detector_parameters,
	.min_inputed_parameters = 0,
	.max_inputed_parameters = 0,
	.defined_parameters     = detector_parameters,
	
	.min_extra_parameters   = 0,
	.max_extra_parameters   = 0,
	.default_parameter      = default_parameter,
	
	.num_defined_subconfigs = 0,
	.min_num_subconfigs     = 3,
	.max_num_subconfigs     = 3,
	.defined_subconfigs     = NULL,
    
    .min_extra_subconfigs   = 0,
	.max_extra_subconfigs   = INT32_MAX,
	.default_subconfig      = default_detector,

	.struct_size            = sizeof(detector_s)
};

loader_config_s default_detectors[] = {detectors_subconfig};

// Network:
const int32_t num_network_parameters = 4;
parameter_s network_parameters[] = 
{
	{"speed_of_light"   , float_e, 1, 1},
	{"equatorial_radius", float_e, 1, 1},
	{"polar_radius"     , float_e, 1, 1},
	{"num_dimensions"   , int_e  , 1, 1}	
};

loader_config_s network_subconfig = 
{
	.name                   = "network",
	.name_necessity         = required_e,

	.is_superconfig         = true,
	.has_parameters         = true,
	.inherit                = false,
	
    .min                    = 1,
    .max                    = 1,
    
	.num_defined_parameters = num_network_parameters,
	.min_inputed_parameters = num_network_parameters,
	.max_inputed_parameters = num_network_parameters,
	.defined_parameters     = network_parameters,
	
	.min_extra_parameters   = 0,
	.max_extra_parameters   = 0,
	.default_parameter      = default_parameter,
	
	.num_defined_subconfigs = 1,
	.min_num_subconfigs     = 1,
	.max_num_subconfigs     = 1,
	.defined_subconfigs     = default_detectors,
    
    .min_extra_subconfigs   = 0,
	.max_extra_subconfigs   = 0,
	.default_subconfig      = default_detectors,

	.struct_size            = sizeof(network_config_s)
};

const int32_t num_defined_subconfigs = 1;
loader_config_s defined_subconfigs[] = {
	network_subconfig
};

loader_config_s default_subconfig[] = {network_subconfig};

loader_config_s loader_config = 
{
	.name                   = "dataset",
	.name_necessity         = excluded_e,

	.is_superconfig         = true,
	.has_parameters         = false,
    .reorder                = true,
    
    .min                    = 1,
    .max                    = 1,
    
	.num_defined_parameters = 0,
	.min_inputed_parameters = 0,
	.max_inputed_parameters = 0,
	.defined_parameters     = NULL,
	
	.min_extra_parameters   = 0,
	.max_extra_parameters   = 0,
	.default_parameter      = default_parameter,
    
    .num_defined_subconfigs = num_defined_subconfigs,
	.min_num_subconfigs     = 1,
	.max_num_subconfigs     = 1,
	.defined_subconfigs     = defined_subconfigs,
    
    .min_extra_subconfigs   = 0,
	.max_extra_subconfigs   = 0,
	.default_subconfig      = default_subconfig,

	.struct_size            = 0
};

#endif