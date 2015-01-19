/* This is the module specification for the compression utility
 * module
 */

mod_name		Utility;
qualifier		Compression;
proc_routine	Process;

typedef struct {

	NameSpecifier	mode;
	Float	signalMultiplier;
	Float	powerExponent;
	Float	minResponse;

} Compression;
