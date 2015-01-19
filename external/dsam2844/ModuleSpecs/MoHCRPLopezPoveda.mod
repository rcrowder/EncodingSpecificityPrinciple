/*
 * The hair-cell receptor potential from Lopez-Poveda E.A.,
 * Eustaquio-Martın A, (2006) "A biophysical model of the inner hair cell:
 * the contribution of potassium current to peripheral compression", 
 * J. Assoc. Res. Otolaryngol., 7, 218–235.
 * The fast and slow variables use the same variables as arrays, i.e. 
 * elements 0 and 1 are the 'Fast' and 'Slow' parameters respectively.
 * This reduces the number of parameter names.
 */

mod_name IHCRP;
qualifier LopezPoveda;
proc_routine RunModel;
process_vars true;

typedef struct {

	BoolSpecifier	bMCiliaCouplingMode;
	Float	ciliaTC;	/* BM/cilia displacement time constant, s. */
	Float	ciliaCGain;	/* Cilia coupling gain. */
	Float	Et;			/* Endocochlear potential, Et (V). */
	Float	Rp;			/* Epithelium resistance, Rp (Ohm). */
	Float	Rt;			/* Epithelium resistance, Rt (Ohm). */
	Float	gL;			/* Apical leakage conductance, gL (S). */
	Float	Gm;			/* Maximum mechanical conductance, Gm (S). */
	Float	s0;			/* Displacement sensitivity, s0 (1/m). */
	Float	s1;			/* Displacement sensitivity, s1 (1/m). */
	Float	u0;			/* Displacement offset, u0 (m). */
	Float	u1;			/* Displacement offset, u1 (m). */
	Float	Ca;			/* Apical capacitance, Ca (F). */
	Float	Cb;			/* Apical capacitance, Cb (F). */
	int_al	numCurrents;	/* The number of currents. */
	WChar	**label;	/* Current label */
	Float	*Ek;		/* Conductance reversal potential, Ek (V). */
	Float	*G;			/* Maximum conductance, G (S). */
	Float	*V1;		/* Conductance half-activation potential, V1 (V). */
	Float	*VS1;		/* Conductance voltage sensitivity constant, S1 (V). */
	Float	*V2;		/* Conductance half-activation potential, V2 (V). */
	Float	*VS2;		/* Conductance voltage sensitivity constant, S2 (V). */
	Float	*tau1Max;	/* Conductance voltage time constant, tau1 max (s). */
	Float	*A1;		/* Conductance voltage time constant, A1 (s). */
	Float	*B1;		/* Conductance voltage time constant, B1 (s). */
	Float	*tau1Min;	/* Conductance voltage time constant, tau1Min (s). */
	Float	*tau2Max;	/* Conductance voltage time constant, tau2Max (s). */
	Float	*A2;		/* Conductance voltage time constant, A2 (s). */
	Float	*B2;		/* Conductance voltage time constant, B2 (s). */
	Float	*tau2Min;	/* Conductance voltage time constant, tau2Min (s). */
	Float	VMRest;		/* Resting membrane potential (V). */

} LopezPoveda;

