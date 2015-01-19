/* This is the module specification for the simulation specification utility
 * module
 */

mod_name		Utility;
qualifier		SimScript;
proc_routine	Process;
typedef	int		DatumPtr;

typedef struct {

	NameSpecifier	operationMode;
	NameSpecifier	parFilePathMode;
	DatumPtr		simulation;

} SimScript;
