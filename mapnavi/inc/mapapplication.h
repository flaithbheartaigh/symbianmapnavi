
#ifndef __MAPAPPLICATION_H__
#define __MAPAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "map.hrh"


// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidMapApp = { _UID3 };


// CLASS DECLARATION

/**
* CMapApplication application class.
* Provides factory to create concrete document object.
* An instance of CMapApplication is the application part of the
* AVKON application framework for the LBSSample example application.
*/
class CMapApplication : public CAknApplication
	{
	public: // Functions from base classes

		/**
		* From CApaApplication, AppDllUid.
		* @return Application's UID (KUidLBSSampleApp).
		*/
		TUid AppDllUid() const;

	protected: // Functions from base classes

		/**
		* From CApaApplication, CreateDocumentL.
		* Creates CLBSSampleDocument document object. The returned
		* pointer in not owned by the CMapApplication object.
		* @return A pointer to the created document object.
		*/
		CApaDocument* CreateDocumentL();
	};

#endif // __MAPAPPLICATION_H__

// End of File
