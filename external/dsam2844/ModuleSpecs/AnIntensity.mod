/* This is the module specification for the AnIntensity module
 * The intensity is calculated starting from the TimeOffset position.
 */

mod_name		Analysis;
qualifier		Intensity;
proc_routine	Calc;
typedef struct {

	Float	timeOffset;
	Float	extent;

} Intensity;
