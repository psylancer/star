#include "common.h"
#include "BaseGraphicObject.h"
#define STAR_EFFECT_FILENAME _T("star.fx")
#define STAR_CLOUDS_FILENAME _T("clouds.dds")
#define STAR_CROWN_FILENAME _T("crown.dds")
#define STAR_TEXTURE_WHOLESIZE 1024
#define STAR_TEXTURE_ACTUALSIZE 256
#define STAR_SPRITE_SIZE 10.0f
#define STAR_CreateTextureFast(lpTex,size,lpDevice) \
        D3DXCreateTexture(lpDevice,size,size, \
        D3DX_DEFAULT,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&lpTex)

class TStar : public TBaseGraphicObject {
public:
    TStar(LPDIRECT3DDEVICE9 device);
    ~TStar();
    void Init(D3DXCOLOR colorMain, D3DXCOLOR colorHighlight, clock_t periodClouds, clock_t periodCrown, float distance);
    void UpdateProgress(clock_t deltaTime);
    void UpdateViewerPosition(D3DXVECTOR3 viewerPosition);
    void UpdateViewProjection(D3DXMATRIX mViewProjection);
protected:
    //main star texture
    LPDIRECT3DTEXTURE9 texCloudsWhole;
    LPDIRECT3DTEXTURE9 texCloudsCurrent;
    LPDIRECT3DTEXTURE9 texCloudsNext;
    //crown texture
    LPDIRECT3DTEXTURE9 texCrownWhole;
    LPDIRECT3DTEXTURE9 texCrownCurrent;
    LPDIRECT3DTEXTURE9 texCrownNext;
    //gradient procedural texture
    LPDIRECT3DTEXTURE9 texGradient;
    //colors
    D3DXCOLOR colorMain;
    D3DXCOLOR colorHighlight;
    //animation periods
    clock_t periodClouds;
    clock_t periodCrown;
    //progresses monitoring
    float progressClouds; //0 - 1
    float progressCrown; //0 - 1
    //effect handles
    D3DXHANDLE hTechnic;
    D3DXHANDLE hWVP;
    D3DXHANDLE hColorMain;
    D3DXHANDLE hColorHighlight;
    D3DXHANDLE hTexCloudsCurrent;
    D3DXHANDLE hTexCloudsNext;
    D3DXHANDLE hTexCrownCurrent;
    D3DXHANDLE hTexCrownNext;
    D3DXHANDLE hTexGradient;
    D3DXHANDLE hProgressClouds;
    D3DXHANDLE hProgressCrown;
    //other params
    float distance;
    D3DXVECTOR3 viewerPosition;
    D3DXMATRIX mViewProjection;
    void UpdateMatrix();
    void FillNextTexture(LPDIRECT3DTEXTURE9 src, LPDIRECT3DTEXTURE9 dest);
    void PushNextTexture(LPDIRECT3DTEXTURE9 current, LPDIRECT3DTEXTURE9 next, LPDIRECT3DTEXTURE9 whole, D3DXHANDLE hCurrent, D3DXHANDLE hNext);
    void PreRender();
    void PostRender();
};
