#include "star.h"

void TStar::UpdateMatrix()
{
    D3DXMATRIX mWorld;
    D3DXMatrixTranslation(&mWorld,viewerPosition.x,viewerPosition.y,distance);
    D3DXMatrixMultiply(&mWorld,&mWorld,&mViewProjection);
    effect->SetMatrix(hWVP,&mWorld);
}

void TStar::FillNextTexture(LPDIRECT3DTEXTURE9 src, LPDIRECT3DTEXTURE9 dest)
{
    D3DLOCKED_RECT srcRect,destRect;
    RECT lockRect;
    lockRect.left=rand()/(float)RAND_MAX * (STAR_TEXTURE_WHOLESIZE-STAR_TEXTURE_ACTUALSIZE-1);
    lockRect.right=lockRect.left+STAR_TEXTURE_ACTUALSIZE;
    lockRect.top=rand()/(float)RAND_MAX * (STAR_TEXTURE_WHOLESIZE-STAR_TEXTURE_ACTUALSIZE-1);
    lockRect.bottom=lockRect.top+STAR_TEXTURE_ACTUALSIZE;
    src->LockRect(0,&srcRect,&lockRect,D3DLOCK_READONLY);
    dest->LockRect(0,&destRect,NULL,D3DLOCK_DISCARD);
    for(WORD i=0;i<STAR_TEXTURE_ACTUALSIZE;i++)
    {
        memcpy(destRect.pBits+i*destRect.Pitch,srcRect.pBits+i*srcRect.Pitch,STAR_TEXTURE_ACTUALSIZE*4);
    }
    dest->UnlockRect(0);
    src->UnlockRect(0);
}

void TStar::PushNextTexture(LPDIRECT3DTEXTURE9 current, LPDIRECT3DTEXTURE9 next, LPDIRECT3DTEXTURE9 whole, D3DXHANDLE hCurrent, D3DXHANDLE hNext)
{
    D3DLOCKED_RECT srcRect,destRect;
    next->LockRect(0,&srcRect,NULL,D3DLOCK_READONLY);
    current->LockRect(0,&destRect,NULL,D3DLOCK_DISCARD);
    for(WORD i=0;i<STAR_TEXTURE_ACTUALSIZE;i++)
    {
        memcpy(destRect.pBits+i*destRect.Pitch,srcRect.pBits+i*srcRect.Pitch,STAR_TEXTURE_ACTUALSIZE*4);
    }
    current->UnlockRect(0);
    next->UnlockRect(0);
    FillNextTexture(whole,next);
    effect->SetTexture(hCurrent,current);
    effect->SetTexture(hNext,next);
}

void TStar::PreRender()
{
    device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
}

void TStar::PostRender()
{
    device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
}

TStar::TStar(LPDIRECT3DDEVICE9 device): TBaseGraphicObject(device)
{
    D3DXMatrixIdentity(&mViewProjection);
    viewerPosition=D3DXVECTOR3(0.0f,0.0f,0.0f);
    distance=1.0f;
    D3DXCreateTextureFromFileEx(device,STAR_CLOUDS_FILENAME,STAR_TEXTURE_WHOLESIZE,STAR_TEXTURE_WHOLESIZE,D3DX_DEFAULT,0,
                                D3DFMT_UNKNOWN,D3DPOOL_SYSTEMMEM,D3DX_DEFAULT,D3DX_DEFAULT,0,NULL,NULL,&texCloudsWhole);
    D3DXCreateTextureFromFileEx(device,STAR_CROWN_FILENAME,STAR_TEXTURE_WHOLESIZE,STAR_TEXTURE_WHOLESIZE,D3DX_DEFAULT,0,
                                D3DFMT_UNKNOWN,D3DPOOL_SYSTEMMEM,D3DX_DEFAULT,D3DX_DEFAULT,0,NULL,NULL,&texCrownWhole);
    STAR_CreateTextureFast(texCloudsCurrent,device);
    STAR_CreateTextureFast(texCloudsNext,device);
    STAR_CreateTextureFast(texCrownCurrent,device);
    STAR_CreateTextureFast(texCrownNext,device);
    STAR_CreateTextureFast(texGradient,device);
    colorMain=0xffffffff;
    colorHighlight=0xffffffff;
    periodClouds=1000;
    periodCrown=1000;
    //geometry
    TTexturedVertex vertices[4]={{-STAR_SPRITE_SIZE,-STAR_SPRITE_SIZE,0.0f,0.0f,1.0f},
                         {-STAR_SPRITE_SIZE,STAR_SPRITE_SIZE,0.0f,0.0f,0.0f},
                         {STAR_SPRITE_SIZE,STAR_SPRITE_SIZE,0.0f,1.0f,0.0f},
                         {STAR_SPRITE_SIZE,-STAR_SPRITE_SIZE,0.0f,1.0f,1.0f}};
    WORD indices[6]={0,1,2,0,2,3};
    D3DVERTEXELEMENT9 elements[]=
    {{0,    0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0},
     {0,    12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0},
     D3DDECL_END()
    };
    geometry=new TVertexContainer(device);
    geometry->Init(4,6,2,elements,sizeof(TTexturedVertex));
    geometry->FillVertices(vertices);
    geometry->FillIndices(indices);
    //effect
    LPD3DXBUFFER errors=NULL;
    D3DXCreateEffectFromFile(device,STAR_EFFECT_FILENAME,NULL,NULL,D3DXSHADER_DEBUG,NULL,&effect,&errors);
    if(errors)
    {
        Die((char*)errors->GetBufferPointer());
        return;
    }
    hTechnic=effect->GetTechniqueByName("MainTech");
    hWVP=effect->GetParameterByName(0,"mWVP");
    hColorMain=effect->GetParameterByName(0,"colorMain");
    hColorHighlight=effect->GetParameterByName(0,"colorHighlight");
    hTexCloudsCurrent=effect->GetParameterByName(0,"texCloudsCurrent");
    hTexCloudsNext=effect->GetParameterByName(0,"texCloudsNext");
    hTexCrownCurrent=effect->GetParameterByName(0,"texCrownCurrent");
    hTexCrownNext=effect->GetParameterByName(0,"texCrownNext");
    hTexGradient=effect->GetParameterByName(0,"texGradient");
    hProgressClouds=effect->GetParameterByName(0,"progressClouds");
    hProgressCrown=effect->GetParameterByName(0,"progressCrown");
    FillNextTexture(texCloudsWhole,texCloudsNext);
    FillNextTexture(texCrownWhole,texCrownNext);
    PushNextTexture(texCloudsCurrent,texCloudsNext,texCloudsWhole,hTexCloudsCurrent,hTexCloudsNext);
    PushNextTexture(texCrownCurrent,texCrownNext,texCrownWhole,hTexCrownCurrent,hTexCrownNext);
}

TStar::~TStar()
{
    SafeRelease(texCloudsCurrent);
    SafeRelease(texCloudsNext);
    SafeRelease(texCloudsWhole);
    SafeRelease(texCrownCurrent);
    SafeRelease(texCrownNext);
    SafeRelease(texCrownWhole);
}

void TStar::Init(D3DXCOLOR colorMain, D3DXCOLOR colorHighlight, clock_t periodClouds, clock_t periodCrown, float distance)
{
    this->colorMain=colorMain;
    this->colorHighlight=colorHighlight;
    this->periodClouds=periodClouds;
    this->periodCrown=periodCrown;
    this->distance=distance;
    progressClouds=0.0f;
    progressCrown=0.0f;
    effect->SetValue(hColorMain,&colorMain,sizeof(D3DXCOLOR));
    effect->SetValue(hColorHighlight,&colorHighlight,sizeof(D3DXCOLOR));
    //make procedural texture
    D3DLOCKED_RECT lockRect;
    texGradient->LockRect(0,&lockRect,NULL,D3DLOCK_DISCARD);
    D3DCOLOR *ptr;
    float x,y,radiusVector,alpha,r,g,b;
    for(WORD i=0;i<STAR_TEXTURE_ACTUALSIZE;i++)
    {
        ptr=(D3DCOLOR*)(lockRect.pBits+i*lockRect.Pitch);
        x=i/(float)(STAR_TEXTURE_ACTUALSIZE-1)-0.5f;
        for(WORD j=0;j<STAR_TEXTURE_ACTUALSIZE;j++)
        {
            y=j/(float)(STAR_TEXTURE_ACTUALSIZE-1)-0.5f;
            radiusVector=sqrt(x*x+y*y);
            if((radiusVector<=0.16f)||(radiusVector>0.48f))
            {
                ptr[j]=0x00000000;
            }
            else if((radiusVector>0.16f)&&(radiusVector<=0.4f))
            {
                alpha=(radiusVector-0.16f)/0.24f;
                ptr[j]=D3DCOLOR_COLORVALUE(colorHighlight.r,colorHighlight.g,colorHighlight.b,alpha);
            }
            else if((radiusVector>0.4f)&&(radiusVector<=0.42f))
            {
                alpha=(radiusVector-0.4f)/0.02f;
                r=colorHighlight.r*(1.0f-alpha)+colorMain.r*alpha;
                g=colorHighlight.g*(1.0f-alpha)+colorMain.g*alpha;
                b=colorHighlight.b*(1.0f-alpha)+colorMain.b*alpha;
                ptr[j]=D3DCOLOR_COLORVALUE(r,g,b,1.0f);
            }
            else if((radiusVector>0.42f)&&(radiusVector<0.48f))
            {
                alpha=(0.48f-radiusVector)/0.06f;
                ptr[j]=D3DCOLOR_COLORVALUE(colorMain.r,colorMain.g,colorMain.b,alpha);
            }
        }
    }
    texGradient->UnlockRect(0);
    effect->SetTexture(hTexGradient,texGradient);
}

void TStar::UpdateProgress(clock_t deltaTime)
{
    progressClouds+=deltaTime/(float)periodClouds;
    if(progressClouds>1.0f)
    {
        progressClouds=0.0f;
        PushNextTexture(texCloudsCurrent,texCloudsNext,texCloudsWhole,hTexCloudsCurrent,hTexCloudsNext);
    }
    effect->SetValue(hProgressClouds,&progressClouds,sizeof(float));
    progressCrown+=deltaTime/(float)periodClouds;
    if(progressCrown>1.0f)
    {
        progressCrown=0.0f;
        PushNextTexture(texCrownCurrent,texCrownNext,texCrownWhole,hTexCrownCurrent,hTexCrownNext);
    }
    effect->SetValue(hProgressCrown,&progressCrown,sizeof(float));
}

void TStar::UpdateViewerPosition(D3DXVECTOR3 viewerPosition)
{
    this->viewerPosition=viewerPosition;
    UpdateMatrix();
}

void TStar::UpdateViewProjection(D3DXMATRIX mViewProjection)
{
    this->mViewProjection=mViewProjection;
    UpdateMatrix();
}
