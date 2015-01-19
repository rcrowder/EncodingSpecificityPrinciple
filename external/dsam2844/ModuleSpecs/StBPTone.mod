/* This is the module specification for the BPTone module
 */

mod_name		PureTone;
qualifier		Binaural;
proc_routine	GenerateSignal;
typedef struct {

	Float	leftFrequency;
	Float	rightFrequency;
	Float	leftIntensity;
	Float	rightIntensity;
	Float	phaseDifference;
	Float	duration;
	Float	dt;

} BPureTone;
