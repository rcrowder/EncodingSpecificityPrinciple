/* This is the module specification for the distributed
 * low pass filter module.
 */

mod_name		Filter;
qualifier		DLowPass;
proc_routine	RunModel;
typedef struct {

	int		mode;
	Float	signalMultiplier;	/* potential mV */
	ParArrayPtr	freqDistr;

} DLowPassF;

