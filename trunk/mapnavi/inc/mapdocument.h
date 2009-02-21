
#ifndef __MAPDOCUMENT_h__
#define __MAPDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CMapAppUi;
class CEikApplication;


// CLASS DECLARATION


class CMapDocument : public CAknDocument
	{
	public: // Constructors and destructor


		static CMapDocument* NewL( CEikApplication& aApp );


		static CMapDocument* NewLC( CEikApplication& aApp );


		virtual ~CMapDocument();

	public: // Functions from base classes

		CEikAppUi* CreateAppUiL();

	private: // Constructors

		void ConstructL();

		CMapDocument( CEikApplication& aApp );

	};

#endif // __MAPDOCUMENT_h__

// End of File
