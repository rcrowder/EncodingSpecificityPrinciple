/* This is the module specification for the Laurel H. Carney basilar
 * membrane module.
 */

mod_name		BasilarM;
qualifier		Carney;
proc_routine	RunModel;

typedef struct {

	int			cascade;	/* Defines the Q value of the gamma tone filters.*/
	Float		cutOffFrequency;	/* Fc */
	Float		hCOperatingPoint;	/* PDfb */
	Float		asymmetricalBias;	/* P0 */
	Float		maxHCVoltage;		/* Vmax */
	CFListPtr	cFList;			/* Pointer to centre frequency structure. */

} BMCarney;
