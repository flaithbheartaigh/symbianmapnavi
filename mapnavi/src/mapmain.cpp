// INCLUDE FILES
#include <eikstart.h>
#include "mapapplication.h"


LOCAL_C CApaApplication* NewApplication()
	{
	return new CMapApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

