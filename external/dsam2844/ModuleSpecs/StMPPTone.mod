/* This is the module specification for the StMPPTone module
 */

mod_name		PureTone;
qualifier		MultiPulse;
proc_routine	GenerateSignal;
typedef struct {

	int_al	numPulses;
	Float	*frequencies;
	Float	intensity;
	Float	beginPeriodDuration;
	Float	pulseDuration;
	Float	repetitionPeriod;
	Float	duration, dt;

} PureTone4;
