

// INCLUDE FILES
#include "map.hrh"
#include "mapdocument.h"
#include "mapapplication.h"


CApaDocument* CMapApplication::CreateDocumentL()
	{
	return (static_cast<CApaDocument*>
					( CMapDocument::NewL( *this ) ) );
	}


TUid CMapApplication::AppDllUid() const
	{
	return KUidMapApp;
	}

// End of File
