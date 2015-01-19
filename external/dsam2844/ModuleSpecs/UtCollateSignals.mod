/* This is the module specification for the CollateSignals Utility module
 */

mod_name		Utility;
qualifier		CollateSignals;
typedef struct {

	NameSpecifier	summationMode;
	NameSpecifier	labelMode;
	Float			*labels;

} CollateSigs;
