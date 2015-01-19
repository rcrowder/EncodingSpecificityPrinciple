/* This is the module specification for the multi-band pass pre-emphasis module.
 *
 *** 10-11-98 Change to just using arrays? ***
 */

mod_name		Filter;
qualifier		MultiBPass;
proc_routine	RunModel;
typedef struct {

	int_al	numFilters;
	int		*cascade;
	Float	*gain;
	Float	*lowerCutOffFreq;
	Float	*upperCutOffFreq;

} MultiBPassF;
