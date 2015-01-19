/* This is the module specification for the AMTone module
 */

mod_name		PureTone;
qualifier		AM;
proc_routine	GenerateSignal;
typedef struct {

	Float	frequency;
	Float	modulationFrequency;
	Float	modulationDepth;
	Float	intensity;
	Float	duration;
	Float	dt;

} AMTone;
