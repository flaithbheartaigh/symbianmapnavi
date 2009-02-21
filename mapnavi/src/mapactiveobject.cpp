#include "mapactiveobject.h"
#include "Mapappview.h"

CMapActiveObject* CMapActiveObject::NewL( CMapAppView* aAppView )
    {
    CMapActiveObject* self = new (ELeave) CMapActiveObject;
    CleanupStack::PushL(self);
    self->ConstructL( aAppView );
   
    CleanupStack::Pop(self);
    return self;
    }

CMapActiveObject::CMapActiveObject()
    : CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    iState = EMapNone;
    }

CMapActiveObject::~CMapActiveObject()
    {
    Cancel();
    }

void CMapActiveObject::ConstructL( CMapAppView* aAppView )
    {
    iAppView = aAppView;
    }

void CMapActiveObject::DoCancel()
    {
    }

void CMapActiveObject::RunL()
    {
    User::LeaveIfError ( iStatus.Int() );
    
    switch ( iState )
        {
        case EBrowseMap:
            iAppView->GetGPSInfoL();
            break;
        case EZoomMap:
            iAppView->ZoomMap( iOperation );
            break;
        default:
            break;
        };
    }

void CMapActiveObject::BrowseMap( TMapOpertion aOp )
    {
    if (IsActive())
        {
        return;
        }
    
    iOperation = aOp;
    
    iState = EBrowseMap;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }


void CMapActiveObject::ZoomMap( TMapOpertion aOp )
    {
    if (IsActive())
        {
        return;
        }
    
    iOperation = aOp;
    
    iState = EZoomMap;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }
