/* This is the module specification for the UtLocalChans module.
 * It calculates the localised measures of a multi-channel input, grouping the
 * channels according to a specified algorithm
 */

mod_name		Utility;
qualifier		LocalChans;
proc_routine	Calc;
process_vars	false;
typedef struct {

	NameSpecifier	mode;
	NameSpecifier	limitMode;
	Float	lowerLimit;
	Float	upperLimit;

} LocalChans;
