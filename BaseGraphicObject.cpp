#include "BaseGraphicObject.h"

TBaseGraphicObject::TBaseGraphicObject(LPDIRECT3DDEVICE9 device)
{
    this->device=device;
    geometry=new TVertexContainer(device);
    effect=NULL;
}

TBaseGraphicObject::~TBaseGraphicObject()
{
    SafeDelete(geometry);
    SafeRelease(effect);
}

void TBaseGraphicObject::Render()
{
    PreRender();
    UINT passCount=0;
    effect->Begin(&passCount,0);
    for(UINT i=0;i<passCount;i++)
    {
        effect->BeginPass(0);
        geometry->Render();
        effect->EndPass();
    }
    effect->End();
    PostRender();
}

void TBaseGraphicObject::PreRender()
{

}

void TBaseGraphicObject::PostRender()
{

}
