#include "shell.h"
#include "star.h"
#include "stdio.h"
#define MOUSE_SENSITIVITY 0.02
#define MOVE_SPEED 0.05
#define MOUSE_MINEDGE 100

TShell *shell=NULL;
LPDIRECT3DDEVICE9 device=NULL;
LPD3DXFONT font=NULL;
TStar *star=NULL;
D3DXVECTOR3 pos(0.0f,0.0f,0.0f),at(0.0f,0.0f,1.0f);
POINT mouseCoords;
DWORD frameCount=0;
clock_t frameMilliseconds=0;
float fps=0.0f;
TCHAR buf[255]="0";
RECT fontRect;
D3DXMATRIX mWorld,mView,mProj;


void SetupDevice()
{
    device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
    device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
    device->SetRenderState(D3DRS_LIGHTING,FALSE);
    device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
}

void SetupGeometry()
{
    star=new TStar(device);
    star->Init(D3DXCOLOR(0xffff0000),D3DXCOLOR(0xffffff00),5000,6000,15.0f);
}

void SetupFont()
{
    if(FAILED(D3DXCreateFont(device,20,15,FW_NORMAL,0,FALSE,DEFAULT_CHARSET,0,0,DEFAULT_PITCH|FF_MODERN,_T("Arial"),&font)))
    {
        Die("Font error");
        return;
    }
    fontRect.left=10;
    fontRect.right=50;
    fontRect.top=10;
    fontRect.bottom=30;
}

void UpdateView()
{
    D3DXMatrixLookAtLH(&mView,&pos,&at,new D3DXVECTOR3(0.0f,1.0f,0.0f));
    star->UpdateViewProjection(mView*mProj);
    star->UpdateViewerPosition(pos);
}

void Reshape(RECT winRect)
{
    D3DXMatrixPerspectiveFovLH(&mProj,D3DX_PI/4.0f,(winRect.right-winRect.left)/((float)winRect.bottom-winRect.top),1.0f,1000.0f);
    star->UpdateViewProjection(mView*mProj);
}

void MouseMove(INT x, INT y, WORD buttons)
{
    INT screenX=GetSystemMetrics(SM_CXSCREEN),
        screenY=GetSystemMetrics(SM_CYSCREEN),
        deltaX=x-mouseCoords.x,
        deltaY=y-mouseCoords.y;
    BOOL modified=false;
    mouseCoords.x=x;
    mouseCoords.y=y;
    //rotate in horizontal plane
    D3DXMATRIX mRot,mTransf,mTransb,mView;
    D3DXMatrixTranslation(&mTransf,-pos.x,-pos.y,-pos.z);
    D3DXMatrixRotationY(&mRot,deltaX*MOUSE_SENSITIVITY*D3DX_PI/180.0f);
    D3DXMatrixTranslation(&mTransb,pos.x,pos.y,pos.z);
    D3DXMatrixMultiply(&mView,&mTransf,&mRot);
    D3DXMatrixMultiply(&mView,&mView,&mTransb);
    D3DXVec3TransformCoord(&at,&at,&mView);
    //rotate in vertical plane
    D3DXVECTOR3 axis,
                dir=at-pos;
    D3DXVec3Cross(&axis,&dir,new D3DXVECTOR3(0.0f,1.0f,0.0f));
    D3DXMatrixRotationAxis(&mRot,&axis,-deltaY*MOUSE_SENSITIVITY*D3DX_PI/180.0f);
    D3DXMatrixMultiply(&mView,&mTransf,&mRot);
    D3DXMatrixMultiply(&mView,&mView,&mTransb);
    D3DXVec3TransformCoord(&at,&at,&mView);
    UpdateView();
    if((mouseCoords.x<MOUSE_MINEDGE)||(mouseCoords.x>screenX-MOUSE_MINEDGE))
    {
        mouseCoords.x=screenX/2;
        modified=true;
    }
    if((mouseCoords.y<MOUSE_MINEDGE)||(mouseCoords.y>screenY-MOUSE_MINEDGE))
    {
        mouseCoords.y=screenY/2;
        modified=true;
    }
    if(modified)
    {
        SetCursorPos(mouseCoords.x,mouseCoords.y);
    }
}

void Keyboard()
{
    //A 0x41, D 0x44, W 0x57, S 0x53
    BOOL fwd=GetAsyncKeyState(0x57)&0x8000,
         bwd=GetAsyncKeyState(0x53)&0x8000,
         lft=GetAsyncKeyState(0x41)&0x8000,
         rht=GetAsyncKeyState(0x44)&0x8000,
         stillMoving=fwd||bwd||lft||rht;
    D3DXVECTOR3 dir;
    if(stillMoving)
    {
        dir=at-pos;
    }
    if(fwd||bwd)
    {
        D3DXVec3Normalize(&dir,&dir);
        if(bwd)
        {
            dir=-dir;
        }
        pos+=dir*MOVE_SPEED*shell->GetDeltaTime();
        at+=dir*MOVE_SPEED*shell->GetDeltaTime();
    }
    if(lft||rht)
    {
        D3DXVECTOR3 axis;
        D3DXVec3Cross(&axis,&dir,new D3DXVECTOR3(0.0f,1.0f,0.0f));
        D3DXVec3Normalize(&axis,&axis);
        if(rht)
        {
            axis=-axis;
        }
        pos+=axis*MOVE_SPEED*shell->GetDeltaTime();
        at+=axis*MOVE_SPEED*shell->GetDeltaTime();
    }
    if(stillMoving)
    {
        UpdateView();
    }
}

void Redraw()
{
    frameCount++;
    frameMilliseconds+=shell->GetDeltaTime();
    device->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,D3DCOLOR_XRGB(0,0,0),1.0f,0);
    star->UpdateProgress(shell->GetDeltaTime());
    device->BeginScene();
    star->Render();
    if(frameMilliseconds>1000)
    {
        fps=1000.0f*frameCount/frameMilliseconds;
        _stprintf(buf,"%4.2f",fps);
        frameMilliseconds=0;
        frameCount=0;
    }
    font->DrawText(NULL,buf,-1,&fontRect,DT_NOCLIP,0xffffffff);
    device->EndScene();
    device->Present(NULL,NULL,NULL,NULL);
}

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
    srand(clock());
    shell=new TShell();
    shell->Init(800,600,_T("Test"),true);
    shell->AssignRedraw(&Redraw);
    shell->AssignReshape(&Reshape);
    shell->AssignMouseMove(&MouseMove);
    shell->AssignKeyboard(&Keyboard);
    device=shell->GetDevice();
    SetupDevice();
    SetupGeometry();
    SetupFont();
    UpdateView();
    mouseCoords.x=GetSystemMetrics(SM_CXSCREEN)/2;
    mouseCoords.y=GetSystemMetrics(SM_CYSCREEN)/2;
    SetCursorPos(mouseCoords.x,mouseCoords.y);
    ShowCursor(false);
    return shell->Run();
}
