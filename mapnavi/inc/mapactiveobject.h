

#ifndef __MAPACTIVEOBJECT_H__
#define __MAPACTIVEOBJECT_H__

#include <e32base.h>

class CMapAppView;

enum TMapOpertion
    {
    EMapUp = 1,
    EMapDown,
    EMapRight,
    EMapLeft,
    EMapZoomIn,
    EMapZoomOut
    };


class CMapActiveObject : public CActive
    {
public:
    static CMapActiveObject* NewL( CMapAppView* aAppView );
    ~CMapActiveObject();
    
    void BrowseMap( TMapOpertion aDirection );
    
    void ZoomMap( TMapOpertion aZoom );
    
private: 
    CMapActiveObject();
    void ConstructL( CMapAppView* aAppView );
    
    void DoCancel();
    void RunL();

private:
    enum TState
        {
        EMapNone = 0,
        EBrowseMap = 1,
        EZoomMap =2,
        
        };
private:
    
    TState iState;
    
    CMapAppView* iAppView;
    
    TMapOpertion iOperation;
    };

#endif
