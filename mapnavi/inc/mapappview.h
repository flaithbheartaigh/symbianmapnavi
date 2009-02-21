#ifndef __MAPAPPVIEW_h__
#define __MAPAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>

#include "mapactiveobject.h"
#include "maphttpengine.h"
#include "mapimageengine.h"


class CClientEngine;

class CImage_Reader;

// CLASS DECLARATION
class CMapAppView : public CCoeControl,public MClientObserver,MImageReadyCallBack
	{
	public: // New methods

		/**
		* NewL.
		* Two-phased constructor.
		* Create a CMapAppView object, which will draw itself to aRect.
		* @param aRect The rectangle this view will be drawn to.
		* @return a pointer to the created instance of CMapAppView.
		*/
		static CMapAppView* NewL( const TRect& aRect );

		/**
		* NewLC.
		* Two-phased constructor.
		* Create a CMapAppView object, which will draw itself
		* to aRect.
		* @param aRect Rectangle this view will be drawn to.
		* @return A pointer to the created instance of CMapAppView.
		*/
		static CMapAppView* NewLC( const TRect& aRect );

		/**
		* ~CMapAppView
		* Virtual Destructor.
		*/
		virtual ~CMapAppView();

	public:  // Functions from base classes

		/**
		* From CCoeControl, Draw
		* Draw this CMapAppView to the screen.
		* @param aRect the rectangle of this view that needs updating
		*/
		void Draw( const TRect& aRect ) const;

		/**
		* From CoeControl, SizeChanged.
		* Called by framework when the view size is changed.
		*/
		virtual void SizeChanged();
		
		virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

	private: // Constructors

		/**
		* ConstructL
		* 2nd phase constructor.
		* Perform the second phase construction of a
		* CMapAppView object.
		* @param aRect The rectangle this view will be drawn to.
		*/
		void ConstructL(const TRect& aRect);

		/**
		* CMapAppView.
		* C++ default constructor.
		*/
		CMapAppView();
		
	public :
	
	void ClientEvent(const TDesC& aEventDescription);
	
	void ClientBodyReceived(const TDesC8& aBodyData);
	
	void ClientBodyCompleted();
	void SetImageL();
	void ImageReadyL(const TInt&  state);
		
		
	public:
	
		void GetGPSInfoL();
		
		void ZoomMap( TMapOpertion aOp );
		
		void SendHTTPRequestL();
		
		void SetupFileDownload();
		
		void SetImage();
		

		
private:

// variables to hold current locations
TReal latitude, longitude;

TReal latitude1, longitude1;

CClientEngine* iEngine;

CImageReader* iJPG_Reader;

RFile iRLbsImage;

//RFs iFs;

// Buffer for storing values

TBuf<30> iValueBuffer;

CMapActiveObject* iActiveObject;

TBool iOperationOngoing;

TInt iZoom;

	};

#endif // __MAPAPPVIEW_h__

// End of File
