#include "common.h"
#define KEYBOARD_UPDATE_INTERVAL 5
#define MOUSE_UPDATE_INTERVAL 5

class TShell {
private:
    HINSTANCE hInstance;
    HWND hWindow;
    LPDIRECT3D9 lpD3d;
    LPDIRECT3DDEVICE9 lpDevice;
    BOOL bFullScreen;
    clock_t lastKeyboardUpdate;
    clock_t lastMouseUpdate;
    clock_t globalTime;
    clock_t deltaTime;
    void (*lpRedrawHandler)();
    void (*lpReshapeHandler)(RECT);
    void (*lpMouseMoveHandler)(INT, INT, WORD);
    void (*lpMouseClickHandler)(INT, INT, WORD);
    void (*lpKeyboardHandler)();
    void TickTimer();
    static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
    TShell();
    ~TShell();
    void Init(INT iWidth, INT iHeight, LPCSTR lpTitle, BOOL bFullScreen);
    LPDIRECT3DDEVICE9 GetDevice();
    void AssignRedraw(void (*lpRedraw)());
    void AssignReshape(void (*lpReshape)(RECT));
    void AssignMouseMove(void (*lpMouseMove)(INT,INT,WORD));
    void AssignMouseClick(void (*lpMouseClick)(INT,INT,WORD));
    void AssignKeyboard(void (*lpKeyboard)());
    clock_t GetDeltaTime();
    clock_t GetGlobalTime();
    INT Run();
};
