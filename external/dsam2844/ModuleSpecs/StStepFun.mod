/* This is the module specification for the StMPPTone module
 */

mod_name		StepFunction;
proc_routine	GenerateSignal;
typedef struct {

	Float	amplitude;
	Float	beginEndAmplitude;
	Float	beginPeriodDuration;
	Float	endPeriodDuration;
	Float	duration, dt;

} StepFun;
