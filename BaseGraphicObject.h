#include "common.h"
#include "VertexContainer.h"

class TBaseGraphicObject {
public:
    TBaseGraphicObject(LPDIRECT3DDEVICE9 device);
    virtual ~TBaseGraphicObject();
    void Render();
protected:
    LPDIRECT3DDEVICE9 device;
    TVertexContainer *geometry;
    LPD3DXEFFECT effect;
    virtual void PreRender();
    virtual void PostRender();
};
