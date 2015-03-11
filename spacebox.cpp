#include "spacebox.h"

TSpaceBox::TSpaceBox(LPDIRECT3DDEVICE9 device) : TBaseGraphicObject(device)
{
    viewerPosition=D3DXVECTOR3(0.0f,0.0f,0.0f);
    D3DXMatrixIdentity(&mViewProjection);
    D3DXCreateTextureFromFile(device,SPACEBOX_TEXTURE_FILENAME,&texBox);
    LPD3DXMESH meshBox=NULL;
    D3DXLoadMeshFromX(SPACEBOX_GEOMETRY_FILENAME,0,device,NULL,NULL,NULL,NULL,&meshBox);
    //geometry
    D3DVERTEXELEMENT9 elements[]={{0,    0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0},
                                  {0,   12,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0},
                                  D3DDECL_END()};
    geometry=new TVertexContainer(device);
    geometry->Init(meshBox->GetNumVertices(),meshBox->GetNumFaces()*3,meshBox->GetNumFaces(),elements,sizeof(TTexturedVertex));
    VOID *ptr;
    meshBox->LockVertexBuffer(D3DLOCK_READONLY,&ptr);
    geometry->FillVertices(ptr);
    meshBox->UnlockVertexBuffer();
    meshBox->LockIndexBuffer(D3DLOCK_READONLY,&ptr);
    geometry->FillIndices(ptr);
    meshBox->UnlockIndexBuffer();
    SafeRelease(meshBox);
    //effect
    LPD3DXBUFFER errors=NULL;
    D3DXCreateEffectFromFile(device,SPACEBOX_EFFECT_FILENAME,NULL,NULL,D3DXSHADER_DEBUG,NULL,&effect,&errors);
    if(errors)
    {
        Die((char*)errors->GetBufferPointer());
        return;
    }
    hTechnic=effect->GetTechniqueByName("MainTech");
    effect->SetTechnique(hTechnic);
    hWVP=effect->GetParameterByName(0,"mWVP");
    hTexBox=effect->GetParameterByName(0,"texBox");
    effect->SetTexture(hTexBox,texBox);
}

TSpaceBox::~TSpaceBox()
{
    SafeRelease(texBox);
}

void TSpaceBox::UpdateViewerPosition(D3DXVECTOR3 viewerPosition)
{
    this->viewerPosition=viewerPosition;
    UpdateMatrix();
}

void TSpaceBox::UpdateViewProjection(D3DXMATRIX mViewProjection)
{
    this->mViewProjection=mViewProjection;
    UpdateMatrix();
}

void TSpaceBox::UpdateMatrix()
{
    D3DXMATRIX mWorld;
    D3DXMatrixTranslation(&mWorld,viewerPosition.x,viewerPosition.y,viewerPosition.z);
    D3DXMatrixMultiply(&mWorld,&mWorld,&mViewProjection);
    effect->SetMatrix(hWVP,&mWorld);
}

void TSpaceBox::PreRender()
{
    device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
}

void TSpaceBox::PostRender()
{
    device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
}

