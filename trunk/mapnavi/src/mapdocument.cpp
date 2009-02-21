// INCLUDE FILES
#include "MapAppUi.h"
#include "MapDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMapDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMapDocument* CMapDocument::NewL( CEikApplication& aApp )
	{
	CMapDocument* self = NewLC( aApp );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMapDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMapDocument* CMapDocument::NewLC( CEikApplication& aApp )
	{
	CMapDocument* self =
		new ( ELeave ) CMapDocument( aApp );

	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CMapDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMapDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CMapDocument::CMapDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMapDocument::CMapDocument( CEikApplication& aApp )
	: CAknDocument( aApp )
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CMapDocument::~CMapDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CMapDocument::~CMapDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CMapDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CMapDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return ( static_cast <CEikAppUi*> ( new ( ELeave )
										CMapAppUi ) );
	}

// End of File
