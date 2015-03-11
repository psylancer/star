#include "common.h"
#include "BaseGraphicObject.h"
#define SPACEBOX_TEXTURE_FILENAME _T("spacebox.tga")
#define SPACEBOX_EFFECT_FILENAME _T("spacebox.fx")
#define SPACEBOX_SPRITE_SIZE 5.0f

class TSpaceBox : public TBaseGraphicObject {
public:
    TSpaceBox(LPDIRECT3DDEVICE9 device);
    ~TSpaceBox();
    void UpdateViewerPosition(D3DXVECTOR3 viewerPosition);
    void UpdateViewProjection(D3DXMATRIX mViewProjection);
protected:
    LPDIRECT3DTEXTURE9 texBox;
    D3DXHANDLE hTechnic;
    D3DXHANDLE hWVP;
    D3DXHANDLE hTexBox;
    D3DXVECTOR3 viewerPosition;
    D3DXMATRIX mViewProjection;
    void UpdateMatrix();
    void PreRender();
    void PostRender();
};
