// INCLUDE FILES
#include <coemain.h>
#include "mapappview.h"
#include "mapgpsengine.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLBSSampleAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMapAppView* CMapAppView::NewL( const TRect& aRect )
	{
	CMapAppView* self = CMapAppView::NewLC( aRect );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMapAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMapAppView* CMapAppView::NewLC( const TRect& aRect )
	{
	CMapAppView* self = new ( ELeave ) CMapAppView;
	CleanupStack::PushL( self );
	self->ConstructL( aRect );
	return self;
	}

// -----------------------------------------------------------------------------
// CMapAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMapAppView::ConstructL( const TRect& aRect )
	{

      iEngine = CClientEngine::NewL(*this);
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect( aRect );

	// Activate the window, which makes it ready to be drawn
	ActivateL();
    
	   latitude1 = 40;
	            longitude1 = 0;
	            
	iActiveObject = CMapActiveObject::NewL( this );  
	
	iOperationOngoing = EFalse;
	
	iZoom = 1;
	}

// -----------------------------------------------------------------------------
// CMapAppView::CMapAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMapAppView::CMapAppView()
	{
	// No implementation required
	}


// -----------------------------------------------------------------------------
// CMapAppView::~CMapAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CMapAppView::~CMapAppView()
	{
	// No implementation required
	delete iActiveObject;
	}


// -----------------------------------------------------------------------------
// CMapAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CMapAppView::Draw( const TRect& aRect ) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect( Rect());

	// Clears the screen
	gc.Clear( drawRect );


	// Display the Map
	if(iJPG_Reader)
	 	{
	 		if(iJPG_Reader->Bitmap())
	 		{
	 			if(iJPG_Reader->Bitmap()->Handle())
	 			{
	 				gc.DrawBitmap(aRect, iJPG_Reader->Bitmap());
	 			}
	 		}
	 	}

	}

// -----------------------------------------------------------------------------
// CMapAppView::GetGPSInfoL()
// Reytrieve the latitude and Longitude of the current position
// -----------------------------------------------------------------------------
//

void CMapAppView::GetGPSInfoL()
	    {
	    iOperationOngoing = ETrue;
	    // create CGpsPositionRequest object and put it into cleanup stack;
	    // pass application name as argument
	    CGpsPositionRequest* request = CGpsPositionRequest::NewL(
	        _L("LBS_0xEA962394"));

	    CleanupStack::PushL( request );
	    // get current location (this operation can be long up to 30 seconds);
	    // progress dialog is shown to user during this time
	    TBool result = request->FetchCurrentPostionL(latitude, longitude);
            latitude = 40;
            longitude = 0;


	    // delete request object
	    CleanupStack::PopAndDestroy(request);

	    if(result)
	    	{
				// Setup a file in the filesystem to download the map
	    	SetupFileDownload();
	    	// send HtTp request to download map
	    	SendHTTPRequestL();
	    	}


	    }

void CMapAppView::ZoomMap( TMapOpertion aOp )
    {
    iOperationOngoing = ETrue;
    if ( aOp == EMapZoomIn )
        {
        iZoom--;
        }
    
    if ( aOp == EMapZoomOut )
        {
        iZoom++;
        }
    // Setup a file in the filesystem to download the map
    SetupFileDownload();
    // send HtTp request to download map
    SendHTTPRequestL();
    }

//Prefix of google URL  to retrieve Map
// %S, %S will be replaced by latitude and longitude values respectively by the values retrieved from the method GetGPSInfoL
_LIT(KGoogleMapURL,"http://maps.google.com/staticmap?center=%S,%S&format=jpg&zoom=%S&size=240x320&key=");

// This is my key, Signup in Google maps and create your own key
_LIT(KGoogleMapKey,"ABQIAAAA7mTDJNgGP953Yt-jOVCpaRSSyDKWhj1AwFHP2_2SAaYvELj_yhTyAVWHzDWGNg0x2EqtccgujgzKwA");

// -----------------------------------------------------------------------------
// CMapAppView::SendHTTPRequestL()
// send Http request with the latitude and longitude values to google maps
// -----------------------------------------------------------------------------
//

void CMapAppView::SendHTTPRequestL()
{



const TInt KMaxFolatLength = 8;
const TInt KDecimalPos = 5;

TRealFormat format( KMaxFolatLength, KDecimalPos );
format.iType = KRealFormatFixed | KDoNotUseTriads;

		TBuf8<KDefaultBufferSize> uri8;

		uri8.Append( KGoogleMapURL );
		uri8.Append( KGoogleMapKey );



		HBufC8* longitudeDes = HBufC8::NewLC(257);
		longitudeDes->Des().Num(longitude1,format);

		HBufC8* latitudeDes = HBufC8::NewLC(257);
		latitudeDes->Des().Num(latitude1,format);

        HBufC8* zoom = HBufC8::NewLC(2);
        zoom->Des().Num( iZoom );
        
		TBuf8<KDefaultBufferSize> TempBuffer;
		TempBuffer.Format(uri8,latitudeDes,longitudeDes, zoom );

		CleanupStack::PopAndDestroy(3);

		// Start transaction

		TRAPD(err, iEngine->IssueHTTPGetL(TempBuffer));
		// TODO: Error handling
		if (err)
		    {
		    }
}

// -----------------------------------------------------------------------------
// CMapAppView::SetupFileDownload()
// wset up a file in the filesystem where to download and save the corresponding map
// -----------------------------------------------------------------------------
//

void CMapAppView::SetupFileDownload()
	{
	

	_LIT(KXMLFilePath, "c:\\Data\\Images\\");
	 TFileName iCurrentFileName;
	 iCurrentFileName.Append(KXMLFilePath);
	iCurrentFileName.Append(_L("LbsSample.jpg"));
	TInt err=iRLbsImage.Open(CCoeEnv::Static()->FsSession(),iCurrentFileName,EFileWrite);
	

	if (err==KErrNotFound) // file does not exist - create it
	{

	err=iRLbsImage.Create(CCoeEnv::Static()->FsSession(),iCurrentFileName,EFileWrite);
	}
	
	else if ( err == KErrAlreadyExists )
	    {
	    err = iRLbsImage.Replace( CCoeEnv::Static()->FsSession(),iCurrentFileName,EFileWrite);
	    }
	else
	    {
	    
	    }

	}


	// -----------------------------------------------------------------------------
	//
	// 	// Listener from the client engine
	// -----------------------------------------------------------------------------
	//



void CMapAppView::ClientEvent(const TDesC& aEventDescription)
	{

	}


	// -----------------------------------------------------------------------------
	//
	// 	// Listener from the client engine to save map
	// -----------------------------------------------------------------------------
	//
void CMapAppView::ClientBodyReceived(const TDesC8& aBodyData)
	{

		TInt aPos=0;
		iRLbsImage.Seek(ESeekCurrent, aPos);
		iRLbsImage.Write(aBodyData); //save the file being downloaded

	}

	// -----------------------------------------------------------------------------
	//
	// 	// Listener from the client engine saying that map download is completed
	// -----------------------------------------------------------------------------
	//
void CMapAppView::ClientBodyCompleted()
	{
	iRLbsImage.Close();
//delete	iRLbsImage;
	SetImage();


	}


void CMapAppView::SetImageL()
	{
	DrawNow();
	}

	// -----------------------------------------------------------------------------
	//
	// 	// Listener from the CImageReader that creation of the map to be displayed is completed.
	// -----------------------------------------------------------------------------
	//
void CMapAppView::ImageReadyL(const TInt&  state)
	{
	DrawNow();
	iOperationOngoing = EFalse;
	}


	// -----------------------------------------------------------------------------
	//
	// 	//Start creation of the map to be displayed
	// -----------------------------------------------------------------------------
	//
_LIT(KTxtFileName, "c:\\Data\\Images\\LbsSample.jpg");
void CMapAppView::SetImage()
	{
	if ( iJPG_Reader == NULL )
	    {
	iJPG_Reader = new(ELeave)CImageReader(*this);
	iJPG_Reader->ConstructL(KTxtFileName);
	    }
	}



// -----------------------------------------------------------------------------
// CMapAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CMapAppView::SizeChanged()
	{
	DrawNow();
	}

TKeyResponse CMapAppView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    
    if ( !iOperationOngoing  )
        {
        switch ( aKeyEvent.iScanCode )
            {
            //left
            case 14 :
                {
                iOperationOngoing = ETrue;
                longitude1= longitude1-0.3*(13-iZoom);
    
                delete iJPG_Reader;
                iJPG_Reader = NULL;
            
               iActiveObject->BrowseMap( EMapRight );
           
               return EKeyWasConsumed;
                }
                break;
            //right    
            case 15 :
                {
                iOperationOngoing = ETrue;
                longitude1= longitude1+0.3*(13-iZoom);
                delete iJPG_Reader;
                iJPG_Reader = NULL;
            
               iActiveObject->BrowseMap( EMapRight );
           
               return EKeyWasConsumed;
                }
                break;
            // up   
            case 16 : 
                {
                iOperationOngoing = ETrue;
                latitude1=latitude1+0.3*(13-iZoom);
                delete iJPG_Reader;
                iJPG_Reader = NULL;
            
               iActiveObject->BrowseMap( EMapRight );
           
               return EKeyWasConsumed;
                }
                break;
            // down
            case 17:
                {
                iOperationOngoing = ETrue;
                latitude1=latitude1-0.3*(13-iZoom);
                delete iJPG_Reader;
                iJPG_Reader = NULL;
                     
                iActiveObject->BrowseMap( EMapRight );
                    
                return EKeyWasConsumed;
                }
                break;
            //zoom in
            case 127:
                {
                iOperationOngoing = ETrue;
                delete iJPG_Reader;
                iJPG_Reader = NULL;
                                  
                iActiveObject->ZoomMap( EMapZoomIn );
                                 
                return EKeyWasConsumed;
                }
                break;
            //zoom out
            case 133:
                {
                iOperationOngoing = ETrue;
                delete iJPG_Reader;
                iJPG_Reader = NULL;
                                  
                iActiveObject->ZoomMap( EMapZoomOut );
                                 
                return EKeyWasConsumed;
                }
                break;
            default:
                break;
            };
        
        
        }
   
    return EKeyWasNotConsumed;
    }
// End of File
