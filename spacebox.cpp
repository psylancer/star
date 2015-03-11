#include "spacebox.h"

TSpaceBox::TSpaceBox(LPDIRECT3DDEVICE9 device) : TBaseGraphicObject(device)
{
    viewerPosition=D3DXVECTOR3(0.0f,0.0f,0.0f);
    D3DXMatrixIdentity(&mViewProjection);
    D3DXCreateTextureFromFile(device,SPACEBOX_TEXTURE_FILENAME,&texBox);
    //geometry
    TTexturedVertex vertices[24]={{-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,0.5f,0.25f}, //front
                                  {-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,0.5f,0.0f},
                                  {SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,1.0f,0.0f},
                                  {SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,1.0f,0.25f},

                                  {-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,0.0f,0.25f}, //top
                                  {-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.0f,0.0f},
                                  {SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.5f,0.0f},
                                  {SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,0.5f,0.25f},

                                  {SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.0f,0.5f}, //back
                                  {SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.0f,0.25f},
                                  {-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.5f,0.25f},
                                  {-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.5f,0.5f},

                                  {-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.0f,0.75f}, //bottom
                                  {-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,0.0f,0.5f},
                                  {SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,0.5f,0.5f},
                                  {SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.5f,0.75f},

                                  {-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.5f,0.75f}, //left
                                  {-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,0.5f,0.5f},
                                  {-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,1.0f,0.5f},
                                  {-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,1.0f,0.75f},

                                  {SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,0.5f,0.5f}, //right
                                  {SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,0.5f,0.25f},
                                  {SPACEBOX_SPRITE_SIZE,SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,1.0f,0.25f},
                                  {SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,-SPACEBOX_SPRITE_SIZE,1.0f,0.5f}};
    WORD indices[36], idx;
    for(WORD i=0;i<6;i++)
    {
        idx=i*6;
        indices[idx]=i*4;
        indices[idx+1]=i*4+1;
        indices[idx+2]=i*4+2;
        indices[idx+3]=i*4;
        indices[idx+4]=i*4+2;
        indices[idx+5]=i*4+3;
    }
    D3DVERTEXELEMENT9 elements[]={{0,    0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0},
                                  {0,   12,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0},
                                  D3DDECL_END()};
    geometry=new TVertexContainer(device);
    geometry->Init(24,36,12,elements,sizeof(TTexturedVertex));
    geometry->FillVertices(vertices);
    geometry->FillIndices(indices);
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

