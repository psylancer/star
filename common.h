#ifndef COMMON_H
#define COMMON_H
#include "windows.h"
#include "time.h"
#include "tchar.h"
#include "d3d9.h"
#include "d3dx9.h"
#define SafeRelease(lpCOM) if(lpCOM!=NULL){ lpCOM->Release(); lpCOM=NULL; }
#define SafeDelete(obj) if(obj!=NULL){ delete obj; obj=NULL; }
#define Die(msg) { MessageBox(NULL,msg,_T("Fatal error"),0); PostQuitMessage(0); }
struct TTexturedVertex {
    FLOAT x,y,z;
    FLOAT u,v;
};
#endif // COMMON_H
