#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <aknlists.h> // CAknSinglePopupMenuStyleListBox
#include <uri8.h>
#include <http.h>
#include <stringloader.h>
#include <chttpformencoder.h>
#include <HttpStringConstants.h>
#include <http\RHTTPTransaction.h>
#include <http\RHTTPSession.h>
#include <http\RHTTPHeaders.h>

#include <COMMDB.H>     //Communications database
#include <CDBPREFTABLE.H> //Connection Preferences table
#include <CommDbConnPref.h>
#include <es_enum.h>  // TConnectionInfoBuf

#include "map.pan"
#include "map.hrh"
#include "maphttpengine.h"

// Used user agent for requests
_LIT8(KUserAgent, "SimpleClient 1.0");

// This client accepts all content types.
// (change to e.g. "text/plain" for plain text only)
_LIT8(KAccept, "*/*");

const TInt KStatustextBufferSize = 32;
const TInt KInfotextBufferSize = 64;
const TInt KURIBufferSize = 128;

// ----------------------------------------------------------------------------
// CClientEngine::NewL()
//
// Creates instance of CClientEngine.
// ----------------------------------------------------------------------------
CClientEngine* CClientEngine::NewL(MClientObserver& aObserver)
  {
    CClientEngine* self = CClientEngine::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
  }


// ----------------------------------------------------------------------------
// CClientEngine::NewLC()
//
// Creates instance of CClientEngine.
// ----------------------------------------------------------------------------
CClientEngine* CClientEngine::NewLC(MClientObserver& aObserver)
  {
    CClientEngine* self = new (ELeave) CClientEngine(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
  }


// ----------------------------------------------------------------------------
// CClientEngine::CClientEngine()
//
// First phase constructor.
// ----------------------------------------------------------------------------
CClientEngine::CClientEngine(MClientObserver& aObserver)
: iObserver(aObserver),
  iPostData(NULL),
  iRunning(EFalse)
  {
  }


// ----------------------------------------------------------------------------
// CClientEngine::~CClientEngine()
//
// Destructor.
// ----------------------------------------------------------------------------
CClientEngine::~CClientEngine()
    {    
    iSession.Close();

    // and finally close handles
    iConnection.Close();
    iSocketServ.Close();

    delete iPostData;
    }


// ----------------------------------------------------------------------------
// CClientEngine::ConstructL()
//
// Second phase construction.
// ----------------------------------------------------------------------------
void CClientEngine::ConstructL()
  {
  }


// ----------------------------------------------------------------------------
// CClientEngine::SetupConnectionL()
//
// 
// ----------------------------------------------------------------------------
void CClientEngine::SetupConnectionL()
    {
    User::LeaveIfError(iSocketServ.Connect());
    User::LeaveIfError(iConnection.Open(iSocketServ));
            
    CCommsDatabase *commsdb = CCommsDatabase::NewL();
    CleanupStack::PushL(commsdb);
    CCommsDbTableView *view = commsdb->OpenIAPTableViewMatchingBearerSetLC( ECommDbBearerGPRS,ECommDbConnectionDirectionOutgoing);
            
    TUint32 ap = 0;
    if(!view->GotoFirstRecord()) 
        {
        view->ReadUintL(TPtrC(COMMDB_ID), ap);
        }
                                                     
    CleanupStack::PopAndDestroy(view); // view and commsdb  
    CleanupStack::PopAndDestroy(commsdb);               

    TCommDbConnPref connectPref;

    // setup preferences 
    connectPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
    connectPref.SetDirection(ECommDbConnectionDirectionOutgoing);
    connectPref.SetIapId( ap );
    User::LeaveIfError(iConnection.Start(connectPref));
    
    iSession.OpenL();
    // Set the sessions connection info...
    RStringPool strPool = iSession.StringPool();
    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
    // ...to use our socket server and connection
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ,
            RHTTPSession::GetTable() ), THTTPHdrVal (iSocketServ.Handle()) );
    // ...to use our connection
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection,
            RHTTPSession::GetTable() ), 
            THTTPHdrVal (REINTERPRET_CAST(TInt, &(iConnection))) );      
    }


// ----------------------------------------------------------------------------
// CClientEngine::SetHeaderL()
//
// Used to set header value to HTTP request
// ----------------------------------------------------------------------------
void CClientEngine::SetHeaderL(RHTTPHeaders aHeaders,
               TInt aHdrField,
               const TDesC8& aHdrValue)
  {
  RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
  CleanupClosePushL(valStr);
  THTTPHdrVal val(valStr);
  aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField,
    RHTTPSession::GetTable()), val);
  CleanupStack::PopAndDestroy();  // valStr
  }


// ----------------------------------------------------------------------------
// CClientEngine::IssueHTTPGetL()
//
// Start a new HTTP GET transaction.
// ----------------------------------------------------------------------------
void CClientEngine::IssueHTTPGetL(const TDesC8& aUri)
  {
  // Parse string to URI (as defined in RFC2396)
  TUriParser8 uri;
  uri.Parse(aUri);
  
  

  SetupConnectionL();
  
  // Get request method string for HTTP GET
  RStringF method = iSession.StringPool().StringF(HTTP::EGET,
    RHTTPSession::GetTable());

  // Open transaction with previous method and parsed uri. This class will
  // receive transaction events in MHFRunL and MHFRunError.
  iTransaction = iSession.OpenTransactionL(uri, *this, method);

  // Set headers for request; user agent and accepted content type
  RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
  SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
  SetHeaderL(hdr, HTTP::EAccept, KAccept);

  // Submit the transaction. After this the framework will give transaction
  // events via MHFRunL and MHFRunError.
  iTransaction.SubmitL();

  iRunning = ETrue;

  }


// ----------------------------------------------------------------------------
// CClientEngine::IssueHTTPPostL()
//
// Start a new HTTP POST transaction.
// ----------------------------------------------------------------------------
void CClientEngine::IssueHTTPPostL(const TDesC8& aUri,
                 const TDesC8& aContentType,
                 const TDesC8& aBody)
  {
  // Parse string to URI
  TUriParser8 uri;
  uri.Parse(aUri);

  // Copy data to be posted into member variable; iPostData is used later in
  // methods inherited from MHTTPDataSupplier.
  delete iPostData;
  iPostData = 0;
  iPostData = aBody.AllocL();

  // Create HTTP connection

  // User didn't select an IAP
  //if (err == KErrNotReady) {
    //HBufC* resTxCancelled = StringLoader::LoadLC(R_HTTP_TX_CANCELLED);
    //iObserver.ClientEvent(*resTxCancelled);
    //CleanupStack::PopAndDestroy(resTxCancelled);
    //return;
 // }

  // Get request method string for HTTP POST
  RStringF method = iSession.StringPool().StringF(HTTP::EPOST,
    RHTTPSession::GetTable());

  // Open transaction with previous method and parsed uri. This class will
  // receive transaction events in MHFRunL and MHFRunError.
  iTransaction = iSession.OpenTransactionL(uri, *this, method);

  // Set headers for request; user agent, accepted content type and body's
  // content type.
  RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
  SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
  SetHeaderL(hdr, HTTP::EAccept, KAccept);
  SetHeaderL(hdr, HTTP::EContentType, aContentType);

  // Set this class as an data supplier. Inherited MHTTPDataSupplier methods
  // are called when framework needs to send body data.
  MHTTPDataSupplier* dataSupplier = this;
  iTransaction.Request().SetBody(*dataSupplier);

  // Submit the transaction. After this the framework will give transaction
  // events via MHFRunL and MHFRunError.
  iTransaction.SubmitL();

  iRunning = ETrue;
 /* HBufC* resConnecting = StringLoader::LoadLC(R_HTTP_CONNECTING);
  iObserver.ClientEvent(*resConnecting);
  CleanupStack::PopAndDestroy(resConnecting);*/
  }

// ----------------------------------------------------------------------------
// CClientEngine::CancelTransaction()
//
// Cancels currently running transaction and frees resources related to it.
// ----------------------------------------------------------------------------
void CClientEngine::CancelTransaction()
  {
  if(!iRunning)
    return;

  // Close() also cancels transaction (Cancel() can also be used but
  // resources allocated by transaction must be still freed with Close())
  iTransaction.Close();

  // Not running anymore
  iRunning = EFalse;
  //HBufC* resTxCancelled = StringLoader::LoadLC(R_HTTP_TX_CANCELLED);
  //iObserver.ClientEvent(*resTxCancelled);
  //CleanupStack::PopAndDestroy(resTxCancelled);
  }


// ----------------------------------------------------------------------------
// CClientEngine::MHFRunL()
//
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ----------------------------------------------------------------------------
void CClientEngine::MHFRunL(RHTTPTransaction aTransaction,
              const THTTPEvent& aEvent)
  {
  switch (aEvent.iStatus)
    {
    case THTTPEvent::EGotResponseHeaders:
      {
      // HTTP response headers have been received. Use
      // aTransaction.Response() to get the response. However, it's not
      // necessary to do anything with the response when this event occurs.

      // Get HTTP status code from header (e.g. 200)
      RHTTPResponse resp = aTransaction.Response();
      TInt status = resp.StatusCode();

      // Get status text (e.g. "OK")
      TBuf<KStatustextBufferSize> statusText;
      statusText.Copy(resp.StatusText().DesC());

   /*   HBufC* resHeaderReceived = StringLoader::LoadLC(R_HTTP_HEADER_RECEIVED, statusText, status);
      iObserver.ClientEvent(*resHeaderReceived);
      CleanupStack::PopAndDestroy(resHeaderReceived);*/
      }
      break;

    case THTTPEvent::EGotResponseBodyData:
      {
      // Part (or all) of response's body data received. Use
      // aTransaction.Response().Body()->GetNextDataPart() to get the actual
      // body data.

      // Get the body data supplier
      MHTTPDataSupplier* body = aTransaction.Response().Body();
      TPtrC8 dataChunk;

      // GetNextDataPart() returns ETrue, if the received part is the last
      // one.
      TBool isLast = body->GetNextDataPart(dataChunk);
      iObserver.ClientBodyReceived(dataChunk);



     // Always remember to release the body data.
      body->ReleaseData();
      }
      break;

    case THTTPEvent::EResponseComplete:
      {
      // Indicates that header & body of response is completely received.
      // No further action here needed.

      }
      break;

    case THTTPEvent::ESucceeded:
      {
      // transaction succeeded.
 

      // Transaction can be closed now. It's not needed anymore.
     
      iTransaction.Close();
       iSession.Close();
       iConnection.Close();
       iSocketServ.Close();
       
      iObserver.ClientBodyCompleted();
      iRunning = EFalse;
      }
      break;

    case THTTPEvent::EFailed:
      {
      // Transaction completed with failure.

      aTransaction.Close();
      iRunning = EFalse;
      }
      break;

    default:
      // There are more events in THTTPEvent, but they are not usually
      // needed. However, event status smaller than zero should be handled
      // correctly since it's error.
      {
      if (aEvent.iStatus < 0)
        {


          // Close the transaction on errors
          aTransaction.Close();
          iRunning = EFalse;
        }
      else
        {
        // Other events are not errors (e.g. permanent and temporary
        // redirections)

        }
      }
      break;
    }
  }


// ----------------------------------------------------------------------------
// CClientEngine::MHFRunError()
//
// Inherited from MHTTPTransactionCallback
// Called by framework when *leave* occurs in handling of transaction event.
// These errors must be handled, or otherwise HTTP-CORE 6 panic is thrown.
// ----------------------------------------------------------------------------
TInt CClientEngine::MHFRunError(TInt aError,
                RHTTPTransaction /*aTransaction*/,
                const THTTPEvent& /*aEvent*/)
  {
  // Just notify about the error and return KErrNone.
  //HBufC* resMHFRunError = StringLoader::LoadLC(R_HTTP_MHFRUN_ERROR, aError);
  //iObserver.ClientEvent(*resMHFRunError);
  //CleanupStack::PopAndDestroy(resMHFRunError);
  return KErrNone;
  }


// ----------------------------------------------------------------------------
// CClientEngine::GetNextDataPart()
//
// Inherited from MHTTPDataSupplier
// Called by framework when next part of the body is needed. In this
// this provides data for HTTP post.
// ----------------------------------------------------------------------------
TBool CClientEngine::GetNextDataPart(TPtrC8& aDataPart)
  {
  if(iPostData)
    {
    // Provide pointer to next chunk of data (return ETrue, if last chunk)
    // Usually only one chunk is needed, but sending big file could require
    // loading the file in small parts.
    aDataPart.Set(iPostData->Des());
    }
  return ETrue;
  }


// ----------------------------------------------------------------------------
// CClientEngine::ReleaseData()
//
// Inherited from MHTTPDataSupplier
// Called by framework. Allows us to release resources needed for previous
// chunk. (e.g. free buffers)
// ----------------------------------------------------------------------------
void CClientEngine::ReleaseData()
  {
  // It's safe to delete iPostData now.
  delete iPostData;
  iPostData = NULL;
  }

// ----------------------------------------------------------------------------
// CClientEngine::Reset()
//
// Inherited from MHTTPDataSupplier
// Called by framework to reset the data supplier. Indicates to the data
// supplier that it should return to the first part of the data.
// In practise an error has occured while sending data, and framework needs to
// resend data.
// ----------------------------------------------------------------------------
TInt CClientEngine::Reset()
  {
  // Nothing needed since iPostData still exists and contains all the data.
  // (If a file is used and read in small parts we should seek to beginning
  // of file and provide the first chunk again in GetNextDataPart() )
  return KErrNone;
  }


// ----------------------------------------------------------------------------
// CClientEngine::OverallDataSize()
//
// Inherited from MHTTPDataSupplier
// Called by framework. We should return the expected size of data to be sent.
// If it's not know we can return KErrNotFound (it's allowed and does not cause
// problems, since HTTP protocol allows multipart bodys without exact content
// length in header).
// ----------------------------------------------------------------------------
TInt CClientEngine::OverallDataSize()
  {
  if(iPostData)
    return iPostData->Length();
  else
    return KErrNotFound ;
  }

// ----------------------------------------------------------------------------
// CClientEngine::GetCredentialsL()
//
// Inherited from MHTTPAuthenticationCallback
// Called by framework when we requested authenticated page and framework
// needs to know username and password.
// ----------------------------------------------------------------------------
TBool CClientEngine::GetCredentialsL(const TUriC8& aURI,
                   RString aRealm,
                   RStringF aAuthenticationType,
                   RString& aUsername,
                   RString& aPassword)
  {
  // aURI, aReal and aAuthenticationType are informational only. We only need
  // to set aUsername and aPassword and return ETrue, if aUsername and
  // aPassword are provided by user.

  // Buffer for string parameters that may change order
  CDesCArrayFlat* strings = new CDesCArrayFlat(2);
  CleanupStack::PushL(strings);

  // Initialize parameter buffer

  // First parameter
  HBufC* uri16 = HBufC::NewLC(aURI.UriDes().Length());
  uri16->Des().Copy(aURI.UriDes());
  strings->AppendL(*uri16);
  CleanupStack::PopAndDestroy(uri16);

  // Second parameter
  HBufC* authType16 = HBufC::NewLC(aAuthenticationType.DesC().Length());
  authType16->Des().Copy(aAuthenticationType.DesC());
  strings->AppendL(*authType16);
  CleanupStack::PopAndDestroy(authType16);

  //HBufC* resAuthNote = StringLoader::LoadLC(R_HTTP_AUTH_NOTE, *strings);
  //HBufC* resAuthRequired = StringLoader::LoadLC(R_HTTP_AUTH_REQUIRED);
  //CEikonEnv::Static()->InfoWinL(*resAuthRequired, *resAuthNote);

  //CleanupStack::PopAndDestroy(resAuthNote);
  //CleanupStack::PopAndDestroy(resAuthRequired);

  // Query user name and password
  TBuf<KDefaultBufferSize> userName;
  TBuf<KDefaultBufferSize> password;
  CAknMultiLineDataQueryDialog* dlg =
    CAknMultiLineDataQueryDialog::NewL(userName, password);

  //if (!dlg->ExecuteLD(R_DIALOG_USER_PASSWORD_QUERY))
   // return EFalse; // No credentials given; must return EFalse

  // Set aUsername and aPassword
  TBuf8<KDefaultBufferSize> temp;
  temp.Copy(userName);
  TRAPD(err, aUsername = aRealm.Pool().OpenStringL(temp));
  if (!err)
    {
    temp.Copy(password);
    TRAP(err, aPassword = aRealm.Pool().OpenStringL(temp));
    if (!err) return ETrue;
    }

  // Return ETrue if user has given credentials (username and password),
  // otherwise EFlase
  return EFalse;
  }

// end of file
