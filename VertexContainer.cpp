#include "VertexContainer.h"

TVertexContainer::TVertexContainer(LPDIRECT3DDEVICE9 device)
{
    this->device=device;
    vb=NULL;
    ib=NULL;
    vertexDesc=NULL;
    vertexCount=0;
    indexCount=0;
    primitiveCount=0;
}

TVertexContainer::~TVertexContainer()
{
    SafeRelease(vb);
    SafeRelease(ib);
    SafeRelease(vertexDesc);
}

void TVertexContainer::Init(WORD vertexCount, WORD indexCount, WORD primitiveCount, D3DVERTEXELEMENT9 *elements, BYTE stride)
{
    this->vertexCount=vertexCount;
    this->indexCount=indexCount;
    this->primitiveCount=primitiveCount;
    this->stride=stride;
    device->CreateVertexDeclaration(elements,&vertexDesc);
    device->CreateVertexBuffer(vertexCount*stride,D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&vb,NULL);
    device->CreateIndexBuffer(indexCount*sizeof(WORD),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&ib,NULL);
}

void TVertexContainer::FillVertices(void *vertices)
{
    void *ptr;
    vb->Lock(0,vertexCount*stride,&ptr,D3DLOCK_DISCARD);
    memcpy(ptr,vertices,vertexCount*stride);
    vb->Unlock();
}

void TVertexContainer::FillIndices(VOID* indices)
{
    void *ptr;
    ib->Lock(0,indexCount*sizeof(WORD),&ptr,D3DLOCK_DISCARD);
    memcpy(ptr,indices,indexCount*sizeof(WORD));
    ib->Unlock();
}

void TVertexContainer::Render()
{
    device->SetVertexDeclaration(vertexDesc);
    device->SetStreamSource(0,vb,0,stride);
    device->SetIndices(ib);
    device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,vertexCount,0,primitiveCount);
}
