/* This is the module specification for the FindBin analysis
 * module
 */

mod_name		Analysis;
qualifier		FindBin;
proc_routine	Calc;
typedef struct {

	NameSpecifier	mode;
	Float	binWidth;
	Float	timeOffset;
	Float	timeWidth;

} FindBin;
