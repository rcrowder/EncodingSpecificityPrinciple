/* This is the module specification for the Zhang et al. IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Zhang;
proc_routine	RunModel;
process_vars	true;

typedef struct {

	Float	spont;		/* Spontaneous rate of fiber (spikes/s). */
	Float	aAss;		/* Steady-state rate (spikes/s). */
	Float	tauST;		/* Short-term time constant (s). */
	Float	tauR;		/* Rapid time constant (s). */
	Float	aAROverAST;	/* Rapid response amplitude to short-term response
						 * amplitude ratio. */
	Float	pIMax;		/* Permeability at high sound level. */
	Float	ks;			/* Species dependend voltage staturation parameter. */
	Float	vs1;		/* Species dependend voltage staturation parameter. */
	Float	vs2;		/* Species dependend voltage staturation parameter. */
	Float	vs3;		/* Species dependend voltage staturation parameter. */

} ZhangHC;
