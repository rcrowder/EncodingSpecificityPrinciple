/* This is the module specification for the mathematical operation utility
 * module.
 * It produces an output signal which has had simple mathematical operations
 * applied to it.
 */

mod_name		Utility;
qualifier		MathOp;
typedef struct {

	NameSpecifier	operatorMode;
	Float		operand;

} MathOp;
