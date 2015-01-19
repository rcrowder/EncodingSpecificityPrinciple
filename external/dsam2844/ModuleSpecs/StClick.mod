/* This is the module specification for the StClick module
 */

mod_name		Click;
proc_routine	GenerateSignal;
typedef struct {

	Float	clickTime;
	Float	amplitude;
	Float	duration;
	Float	dt;

} Click;
