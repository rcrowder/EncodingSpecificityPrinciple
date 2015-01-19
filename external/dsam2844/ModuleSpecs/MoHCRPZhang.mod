/* This is the module specification for the Zhang et al. receptor potential
 * transduction process module.
 */

mod_name		IHCRP;
qualifier		Zhang;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	Float	a0;		/* Scaler in IHC nonlinear function. */
	Float	b;		/* Parameter in IHC nonlinear function. */
	Float	c;		/* Parameter in IHC nonlinear function. */
	Float	d;		/* Parameter in IHC nonlinear function. */
	Float	cut;	/* Cutoff frequency of IHC low-pass filter (Hz). */
	int		k;		/* Order of IHC lowe-pass filter. */

} IHCRPZhang;
