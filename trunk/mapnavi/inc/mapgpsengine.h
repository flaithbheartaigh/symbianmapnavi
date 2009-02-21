

#ifndef __GPSPOSITIONREQUEST_H__
#define __GPSPOSITIONREQUEST_H__

#include <e32base.h>
#include <aknprogressdialog.h>
#include <aknwaitdialog.h>
#include <aknnotewrappers.h>
#include <lbs.h>
#include <lbspositioninfo.h>


class CGpsPositionRequest : public CActive, public MProgressDialogCallback
    {
public:
    static CGpsPositionRequest* NewL(const TDesC& aAppName);
    ~CGpsPositionRequest();
    TBool FetchCurrentPostionL(TReal& aLatitude, TReal& aLongitude);
private: 
    CGpsPositionRequest();
    void ConstructL(const TDesC& aAppName);
    void ShowWaitNoteL();
    void DismissWaitNote();
    // from CActive
    void DoCancel();
    void RunL();
    // from MProgressDialogCallback
    void DialogDismissedL(TInt aButtonId);
private:
    TPositionInfo iPositionInfo;
    TInt iError;
    RPositionServer iLocationServer;
    RPositioner iPositioner;
    CAknWaitDialog* iWaitNote;
    HBufC* iAppName;
    };

#endif
