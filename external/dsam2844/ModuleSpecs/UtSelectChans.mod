/* This is the module specification for the select channels module.
 * This module copies the input signal to the output signal, but sets the
 * amplitudes of specified input channels to zero in the output signal.
 */

mod_name		Utility;
qualifier		SelectChannels;
process_vars	true;
typedef struct {
	
	NameSpecifier	mode;
	NameSpecifier	selectionMode;
	int_al	numChannels;
	Float		*selectionArray;

} SelectChan;
