#ifndef __MAPAPPUI_h__
#define __MAPAPPUI_h__

// INCLUDES
#include <aknappui.h>


// FORWARD DECLARATIONS
class CMapAppView;


// CLASS DECLARATION
/**
* CLBSSampleAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CMapAppUi : public CAknAppUi
	{
	public: // Constructors and destructor

		/**
		* ConstructL.
		* 2nd phase constructor.
		*/
		void ConstructL();

		/**
		* CMapAppUi.
		* C++ default constructor. This needs to be public due to
		* the way the framework constructs the AppUi
		*/
		CMapAppUi();

		/**
		* ~CMapAppUi.
		* Virtual Destructor.
		*/
		virtual ~CMapAppUi();

	private:  // Functions from base classes

		/**
		* From CEikAppUi, HandleCommandL.
		* Takes care of command handling.
		* @param aCommand Command to be handled.
		*/
		void HandleCommandL( TInt aCommand );

		/**
		*  HandleStatusPaneSizeChange.
		*  Called by the framework when the application status pane
 		*  size is changed.
		*/
		void HandleStatusPaneSizeChange();
		
		/**
		*  From CCoeAppUi, HelpContextL.
		*  Provides help context for the application.
 		*  size is changed.
		*/
	 	CArrayFix<TCoeHelpContext>* HelpContextL() const;

	private: // Data

		/**
		* The application view
		* Owned by CMapAppUi
		*/
		CMapAppView* iAppView;
		
		
	};

#endif // __LBSSAMPLEAPPUI_h__

// End of File
