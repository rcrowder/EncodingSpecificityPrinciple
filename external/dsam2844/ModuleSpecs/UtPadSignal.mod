/* This is the module specification for the PadSignal utility module.
 * It produces an output signal which has been padded at the beginning and at
 * the end of the signal.
 */

mod_name		Utility;
qualifier		PadSignal;
typedef struct {

	Float	beginDuration;
	Float	beginValue;
	Float	endDuration;
	Float	endValue;

} PadSignal;
