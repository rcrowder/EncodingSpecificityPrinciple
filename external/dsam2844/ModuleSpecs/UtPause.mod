/* This is the module specification for the pause utility module.
 */

mod_name		Utility;
qualifier		Pause;
typedef struct {

	BoolSpecifier	alertMode;
	int		delay;
	char	message;

} Pause;
