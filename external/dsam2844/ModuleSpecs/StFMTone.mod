/* This is the module specification for the FMTone module
 */

mod_name		PureTone;
qualifier		FM;
proc_routine	GenerateSignal;
typedef struct {

	Float	frequency;
	Float	intensity;
	Float	duration;
	Float	dt;
	Float	phase;
	Float	modulationDepth;
	Float	modulationFrequency;
	Float	modulationPhase;

} FMTone;
