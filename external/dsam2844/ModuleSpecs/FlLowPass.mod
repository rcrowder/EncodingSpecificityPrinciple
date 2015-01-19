/* This is the module specification for the dendrite LP 
 * model module.
 */

mod_name		Filter;
qualifier		LowPass;
proc_routine	RunModel;
typedef struct {

	int		mode;
	Float	cutOffFrequency;
	Float	signalMultiplier;	/* potential mV */

} LowPassF;
