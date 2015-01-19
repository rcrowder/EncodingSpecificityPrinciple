/* This is the module specification for the AnAutoCorr module
 */

mod_name		Analysis;
qualifier		ACF;
proc_routine	Calc;
process_vars	true;
typedef struct {

	NameSpecifier	normalisationMode;
	NameSpecifier	timeConstMode;
	Float	timeOffset;
	Float	timeConstant;
	Float	timeConstScale;
	Float	maxLag;

} AutoCorr;
