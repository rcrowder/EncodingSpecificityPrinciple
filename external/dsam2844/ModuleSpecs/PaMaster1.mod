/* This is the module specification for the parallel PrAudModel1 module
 */

mod_name		MPI;
qualifier		Master1;
proc_routine	DriveWorkers;

typedef int		DatumPtr;

typedef struct {

	int				numWorkers;
	NameSpecifier	diagnosticsMode;
	NameSpecifier	channelMode;
	char			*simSpecParFile;
	char			*cFListParFile;

} Master1;
