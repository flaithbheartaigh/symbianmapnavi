// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>

#include <map.rsg>

#include "map.hrh"
#include "map.pan"
#include "mapapplication.h"
#include "mapappui.h"
#include "mapappview.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CMapAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMapAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);
	// Create view object
	iAppView = CMapAppView::NewL( ClientRect() );
    AddToStackL( iAppView );
	}
// -----------------------------------------------------------------------------
// CMapAppUi::CMapAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMapAppUi::CMapAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CMapAppUi::~CMapAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CMapAppUi::~CMapAppUi()
	{
	if ( iAppView )
		{
		RemoveFromStack( iAppView );
		delete iAppView;
		iAppView = NULL;
		}

	}

// -----------------------------------------------------------------------------
// CMapAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CMapAppUi::HandleCommandL( TInt aCommand )
	{
	switch( aCommand )
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case ECommand1:
			{
			iAppView->GetGPSInfoL();
			}
			break;
		case ECommand2:
			{
			iAppView->SetImageL();
			}
			break;
		case EHelp:
			{
			}
			break;
		case EAbout:
			{
			}
			break;
		default:
			break;
		}
	}
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CMapAppUi::HandleStatusPaneSizeChange()
	{
	iAppView->SetRect( ClientRect() );
	} 

CArrayFix<TCoeHelpContext>* CMapAppUi::HelpContextL() const
	{
	}

// End of File
