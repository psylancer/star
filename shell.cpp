#include "shell.h"
#include "stdio.h"

void TShell::TickTimer()
{
    clock_t currentTime=clock();
    deltaTime=currentTime-globalTime;
    globalTime=currentTime;
}

LRESULT CALLBACK TShell::WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    TShell *inst=(TShell*)GetWindowLongPtr(hwnd,GWLP_USERDATA);
    switch(msg)
    {
    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    case WM_MOUSEMOVE:
        {
            clock_t currentTime;
            if(inst->lpMouseMoveHandler!=NULL)
            {
                currentTime=clock();
                if(currentTime-inst->lastMouseUpdate>MOUSE_UPDATE_INTERVAL)
                {
                    inst->lpMouseMoveHandler(LOWORD(lParam),HIWORD(lParam),wParam);
                    inst->lastMouseUpdate=currentTime;
                }
            }
            return 0;
        }
    case WM_LBUTTONDOWN:
        {
            if(inst->lpMouseClickHandler!=NULL)
            {
                inst->lpMouseClickHandler(LOWORD(lParam),HIWORD(lParam),MK_LBUTTON);
            }
            return 0;
        }
    case WM_RBUTTONDOWN:
        {
            if(inst->lpMouseClickHandler!=NULL)
            {
                inst->lpMouseClickHandler(LOWORD(lParam),HIWORD(lParam),MK_RBUTTON);
            }
            return 0;
        }
    case WM_SIZE:
        {
            if(inst->lpReshapeHandler!=NULL)
            {
                RECT winRect;
                GetWindowRect(hwnd,&winRect);
                inst->lpReshapeHandler(winRect);
            }
            return 0;
        }
    default:
        {
            return DefWindowProc(hwnd,msg,wParam,lParam);
        }
    }
}

TShell::TShell()
{
    hWindow=NULL;
    lpD3d=NULL;
    lpDevice=0;
    bFullScreen=FALSE;
    lastMouseUpdate=lastKeyboardUpdate=globalTime=clock();
    deltaTime=0;
    lpRedrawHandler=NULL;
    lpReshapeHandler=NULL;
    lpMouseMoveHandler=NULL;
    lpMouseClickHandler=NULL;
    lpKeyboardHandler=NULL;
}

TShell::~TShell()
{
    SafeRelease(lpD3d);
    SafeRelease(lpDevice);
}

void TShell::Init(INT iWidth, INT iHeight, LPCSTR lpTitle, BOOL bFullScreen)
{
    hInstance=(HINSTANCE)GetModuleHandle(NULL);
    WNDCLASSEX wcx;
    wcx.cbClsExtra=0;
    wcx.cbSize=sizeof(WNDCLASSEX);
    wcx.cbWndExtra=sizeof(LONG_PTR);
    wcx.hbrBackground=(HBRUSH)COLOR_BACKGROUND;
    wcx.hCursor=LoadCursor(NULL,IDC_ARROW);
    wcx.hIcon=LoadIcon(hInstance,IDI_APPLICATION);
    wcx.hIconSm=LoadIcon(hInstance,IDI_APPLICATION);
    wcx.hInstance=hInstance;
    wcx.lpfnWndProc=&TShell::WinProc;
    wcx.lpszClassName=_T("TShellApp");
    wcx.lpszMenuName=NULL;
    wcx.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    RegisterClassEx(&wcx);
    hWindow=CreateWindow(_T("TShellApp"),lpTitle,bFullScreen ? WS_SYSMENU : WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,iWidth,iHeight,NULL,NULL,hInstance,NULL);
    SetWindowLongPtr(hWindow,GWLP_USERDATA,(LONG_PTR)this);
    ShowWindow(hWindow,SW_NORMAL);
    UpdateWindow(hWindow);
    lpD3d=Direct3DCreate9(D3D_SDK_VERSION);
    D3DDISPLAYMODE mode;
    D3DCAPS9 caps;
    D3DPRESENT_PARAMETERS dpp;
    lpD3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode);
    lpD3d->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
    ZeroMemory(&dpp,sizeof(D3DPRESENT_PARAMETERS));
    dpp.AutoDepthStencilFormat=D3DFMT_D24S8;
    dpp.BackBufferCount=1;
    dpp.BackBufferFormat=mode.Format;
    if(bFullScreen)
    {
        dpp.BackBufferHeight=mode.Height;
        dpp.BackBufferWidth=mode.Width;
        dpp.FullScreen_RefreshRateInHz=mode.RefreshRate;
    }
    else
    {
        dpp.BackBufferHeight=iHeight;
        dpp.BackBufferWidth=iWidth;
    }
    dpp.EnableAutoDepthStencil=TRUE;
    dpp.hDeviceWindow=hWindow;
    dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
    dpp.Windowed=!bFullScreen;
    dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
    lpD3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWindow,(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING),&dpp,&lpDevice);
}

LPDIRECT3DDEVICE9 TShell::GetDevice()
{
    return lpDevice;
}

void TShell::AssignRedraw(void (*lpRedraw)())
{
    this->lpRedrawHandler=lpRedraw;
}

void TShell::AssignReshape(void (*lpReshape)(RECT))
{
    this->lpReshapeHandler=lpReshape;
}

void TShell::AssignMouseMove(void (*lpMouseMove)(INT, INT, WORD))
{
    this->lpMouseMoveHandler=lpMouseMove;
}

void TShell::AssignMouseClick(void (*lpMouseClick)(INT, INT, WORD))
{
    this->lpMouseClickHandler=lpMouseClick;
}

void TShell::AssignKeyboard(void (*lpKeyboard)())
{
    this->lpKeyboardHandler=lpKeyboard;
}

clock_t TShell::GetDeltaTime()
{
    return deltaTime;
}

clock_t TShell::GetGlobalTime()
{
    return globalTime;
}

INT TShell::Run()
{
    MSG msg;
    clock_t currentTime;
    if(lpReshapeHandler!=NULL)
    {
        RECT winRect;
        GetWindowRect(hWindow,&winRect);
        lpReshapeHandler(winRect);
    }
    do
    {
        TickTimer();
        if(lpKeyboardHandler!=NULL)
        {
            if(globalTime-lastKeyboardUpdate>KEYBOARD_UPDATE_INTERVAL)
            {
                lpKeyboardHandler();
                lastKeyboardUpdate=globalTime;
            }
        }
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if(lpRedrawHandler!=NULL)
        {
            lpRedrawHandler();
        }
    }while(msg.message!=WM_QUIT);
    return (INT)msg.wParam;
}
