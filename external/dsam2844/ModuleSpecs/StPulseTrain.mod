/* This is the module specification for the PulseTrain stimulus module
 */

mod_name		PulseTrain;
proc_routine	GenerateSignal;
typedef struct {

	Float	pulseRate;
	Float	pulseDuration;
	Float	amplitude;
	Float	duration;
	Float	dt;

} PulseTrain;
