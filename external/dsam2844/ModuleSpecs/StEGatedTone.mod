/* This is the module specification for the ExpGatedTone module
 */

mod_name		Stimulus;
qualifier		ExpGatedTone;
proc_routine	GenerateSignal;
typedef struct {

	int		typeMode;
	int		floorMode;
	Float	carrierFrequency;
	Float	amplitude;
	Float	phase;
	Float	beginPeriodDuration;
	Float	repetitionRate;
	Float	halfLife;
	Float	floor;
	Float	duration;
	Float	dt;

} EGatedTone;
