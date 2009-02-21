#include <map.rsg> // replace it to your own application RSG file

#include "mapgpsengine.h"


CGpsPositionRequest* CGpsPositionRequest::NewL(const TDesC& aAppName)
    {
    CGpsPositionRequest* self = new (ELeave) CGpsPositionRequest;
    CleanupStack::PushL(self);
    self->ConstructL(aAppName);
    CleanupStack::Pop(self);
    return self;
    }

CGpsPositionRequest::CGpsPositionRequest()
    : CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

CGpsPositionRequest::~CGpsPositionRequest()
    {
    Cancel();
    iPositioner.Close();
    iLocationServer.Close();
    DismissWaitNote();
    delete iAppName;
    }

void CGpsPositionRequest::ConstructL(const TDesC& aAppName)
    {
    iAppName = aAppName.AllocL();
    // connect to location server
    User::LeaveIfError(iLocationServer.Connect());
    // open positioner
    User::LeaveIfError(iPositioner.Open(iLocationServer));
    // set our application as location requestor
    User::LeaveIfError(iPositioner.SetRequestor(
        CRequestor::ERequestorService, CRequestor::EFormatApplication, *iAppName));
    // set maximum allowed time for a location request
    TTimeIntervalMicroSeconds timeOut(30000000); // 30 sec
    
  //  TTimeIntervalMicroSeconds timeOut(3000000000); // 50 minutes sec
    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateTimeOut(timeOut);
    User::LeaveIfError(iPositioner.SetUpdateOptions(updateOptions));
    }

/////////////////////////////////////////////////////////////////////////////

TBool CGpsPositionRequest::FetchCurrentPostionL(TReal& aLatitude, TReal& aLongitude)
    {
    // cancel previous request (just in case)
    Cancel();
    // request current location
    iPositioner.NotifyPositionUpdate(iPositionInfo, iStatus);
    SetActive();
    // start wait note and wait for request end
    //ShowWaitNoteL();
    // process result
    if (iError == KErrNone)
        {
        // success, return given position
        TPosition pos;
        iPositionInfo.GetPosition(pos);
        aLatitude = pos.Latitude();
        aLongitude = pos.Longitude();
        return ETrue;
        }
    // fail
    return EFalse;
    }

/////////////////////////////////////////////////////////////////////////////

void CGpsPositionRequest::ShowWaitNoteL()
    {
    // create wait note
    CAknWaitDialog* waitNote = new (ELeave) CAknWaitDialog(
        reinterpret_cast <CEikDialog**> (&iWaitNote), ETrue); // ETrue means show immedaitely
    CleanupStack::PushL(waitNote);
    waitNote->SetCallback(this);
    waitNote->PrepareLC(R_GPS_REQUEST_POSITION_WAIT_NOTE);
    CleanupStack::Pop(waitNote); // waitNote takes own ownership
    iWaitNote = waitNote;
    // start it
    iWaitNote->RunLD();
    }

void CGpsPositionRequest::DismissWaitNote()
    {
    if (iWaitNote)
        {
        TRAPD(error, iWaitNote->ProcessFinishedL());
        iWaitNote = 0;
        }
    }

/////////////////////////////////////////////////////////////////////////////

void CGpsPositionRequest::DoCancel()
    {
    // cancel location request
    iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
    DismissWaitNote();
    }

void CGpsPositionRequest::RunL()
    {
    // request ended, store error code and dismiss wait dialog
    iError = iStatus.Int();
    DismissWaitNote();
    }

/////////////////////////////////////////////////////////////////////////////

void CGpsPositionRequest::DialogDismissedL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyCancel)
        {
        // dialog is already deleted by framework
        iWaitNote = 0;
        Cancel();
        iError = KErrAbort;
        }
    }

