/* This is the module specification for the start/end envelope transform
 * --ramp or damp--module.
 * Transform modules do not create a signal, but change an existing one.
 * 
 * Parameters:
 *	-position mode:
 *		- "absolute" e.g. from t=0.
 *		- "relative" when signal starts to change, e.g. increase.
 *	-operation mode:
 *		- "ramp" up.
 *		- "damp down.
 *	-type mode:
 *		- "linear"
 *		- "sine"
 *		- "exp_decay"
 */

mod_name		Transform;
qualifier		Gate;
typedef struct {

	NameSpecifier	positionMode;
	NameSpecifier	operationMode;
	NameSpecifier	typeMode;
	Float	timeOffset;
	Float	timeInterval;
	Float	slopeParameter;

} Gate;
