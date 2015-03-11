#include "common.h"
#ifndef VERTEXCONTAINER_H
#define VERTEXCONTAINER_H
class TVertexContainer {
private:
    LPDIRECT3DDEVICE9 device;
    LPDIRECT3DVERTEXBUFFER9 vb;
    LPDIRECT3DINDEXBUFFER9 ib;
    WORD vertexCount;
    WORD indexCount;
    WORD primitiveCount;
    BYTE stride;
    LPDIRECT3DVERTEXDECLARATION9 vertexDesc;
public:
    TVertexContainer(LPDIRECT3DDEVICE9 device);
    ~TVertexContainer();
    void Init(WORD vertexCount, WORD indexCount, WORD primitiveCount, D3DVERTEXELEMENT9 *elements, BYTE stride);
    void FillVertices(void *vertices);
    void FillIndices(void *indices);
    void Render();
};
#endif // VERTEXCONTAINER_H
