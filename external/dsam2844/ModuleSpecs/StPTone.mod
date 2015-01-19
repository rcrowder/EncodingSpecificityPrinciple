/* This is a module specifiction file for the pure tone stimulus */
mod_name		PureTone;
proc_routine	GenerateSignal;
typedef struct {

	Float	frequency;
	Float	intensity;
	Float	duration, dt;

} PureTone;
	
