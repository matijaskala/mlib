/*
 * This file is part of MLib
 * Copyright (C) 2016  Matija Skala <mskala@gmx.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <MDebug>
#include <MKeys>
#include <MVideoInterface>
#include <mwindow.h>
#include <GL/gl.h>

class DIBWindow : public MWindow
{
public:
    DIBWindow ( int width, int height, HWND hwnd, HDC dc );
    virtual void makeCurrent();
    virtual void raise();
    virtual void resize();
    virtual void swapBuffers();
    HWND hwnd;
    HDC dc;
    HGLRC rc;
};

class DIBVideoInterface : public MVideoInterface
{
public:
    virtual void handleEvents() override;
    virtual bool init() override;
    virtual void fini() override;
    virtual MWindow* createWindow ( int width, int height ) override;
    virtual void destroyWindow ( MWindow* window ) override;
    HWND hidden;
    HDC dc;
    HGLRC rc;
};

DIBWindow::DIBWindow ( int width, int height, HWND hwnd, HDC dc ) : MWindow{width,height}, hwnd{hwnd}, dc{dc}
{
    PIXELFORMATDESCRIPTOR descriptor;
    ZeroMemory(&descriptor, sizeof(descriptor));
    descriptor.nSize        = sizeof(descriptor);
    descriptor.nVersion     = 1;
    descriptor.iLayerType   = PFD_MAIN_PLANE;
    descriptor.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    descriptor.iPixelType   = PFD_TYPE_RGBA;
    descriptor.cColorBits   = static_cast<BYTE>(32);
    descriptor.cDepthBits   = static_cast<BYTE>(0);
    descriptor.cStencilBits = static_cast<BYTE>(0);
    descriptor.cAlphaBits   = 8;

    // Get the pixel format that best matches our requirements
    int bestFormat = ChoosePixelFormat(dc, &descriptor);
    PIXELFORMATDESCRIPTOR actualFormat;
    actualFormat.nSize    = sizeof(actualFormat);
    actualFormat.nVersion = 1;
    DescribePixelFormat(dc, bestFormat, sizeof(actualFormat), &actualFormat);
    SetPixelFormat(dc, bestFormat, &actualFormat);

    rc = wglCreateContext(dc);
    resize();
    makeCurrent();

    glViewport ( 0, 0, width, height );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
    glOrtho ( 0, width, height, 0, -1, 1 );
    glMatrixMode ( GL_MODELVIEW );

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DIBWindow::makeCurrent()
{
    wglMakeCurrent(dc, rc);
}

void DIBWindow::raise()
{
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, size.width(), size.height(), SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOSIZE);
}

void DIBWindow::resize()
{
    RECT bounds;
    bounds.left = 0;
    bounds.top = 0;
    bounds.right = size.width();
    bounds.bottom = size.height();
    AdjustWindowRect(&bounds, GetWindowLong(hwnd, GWL_STYLE), 0);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, bounds.right - bounds.left, bounds.bottom - bounds.top, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOACTIVATE);
}

void DIBWindow::swapBuffers()
{
    SwapBuffers(dc);
}

static const MKey windows_scancode_table[] = 
{ 
	/*	0						1							2							3							4						5							6							7 */
	/*	8						9							A							B							C						D							E							F */
	M_KEY_UNKNOWN,		M_KEY_ESCAPE,		M_KEY_1,				M_KEY_2,				M_KEY_3,			M_KEY_4,				M_KEY_5,				M_KEY_6,			/* 0 */
	M_KEY_7,				M_KEY_8,				M_KEY_9,				M_KEY_0,				M_KEY_MINUS,		M_KEY_EQUALS,		M_KEY_BACKSPACE,		M_KEY_TAB,		/* 0 */

	M_KEY_q,				M_KEY_w,				M_KEY_e,				M_KEY_r,				M_KEY_t,			M_KEY_y,				M_KEY_u,				M_KEY_i,			/* 1 */
	M_KEY_o,				M_KEY_p,				M_KEY_LEFTBRACKET,	M_KEY_RIGHTBRACKET,	M_KEY_RETURN,	M_KEY_LCTRL,			M_KEY_a,				M_KEY_s,			/* 1 */

	M_KEY_d,				M_KEY_f,				M_KEY_g,				M_KEY_h,				M_KEY_j,			M_KEY_k,				M_KEY_l,				M_KEY_SEMICOLON,	/* 2 */
	M_KEY_QUOTE,	M_KEY_GRAVE,			M_KEY_LSHIFT,		M_KEY_BACKSLASH,		M_KEY_z,			M_KEY_x,				M_KEY_c,				M_KEY_v,			/* 2 */

	M_KEY_b,				M_KEY_n,				M_KEY_m,				M_KEY_COMMA,			M_KEY_PERIOD,	M_KEY_SLASH,			M_KEY_RSHIFT,		M_KEY_PRINT,/* 3 */
	M_KEY_LALT,			M_KEY_SPACE,			M_KEY_CAPSLOCK,		M_KEY_F1,			M_KEY_F2,		M_KEY_F3,			M_KEY_F4,			M_KEY_F5,		/* 3 */

	M_KEY_F6,			M_KEY_F7,			M_KEY_F8,			M_KEY_F9,			M_KEY_F10,		M_KEY_NUMLOCK,	M_KEY_SCROLLLOCK,	M_KEY_HOME,		/* 4 */
	M_KEY_UP,			M_KEY_PAGEUP,		M_KEY_KP_MINUS,		M_KEY_LEFT,			M_KEY_KP5,		M_KEY_RIGHT,			M_KEY_KP_PLUS,		M_KEY_END,		/* 4 */

	M_KEY_DOWN,			M_KEY_PAGEDOWN,		M_KEY_INSERT,		M_KEY_DELETE,		M_KEY_UNKNOWN,	M_KEY_UNKNOWN,		/*M_KEY_NONUSBACKSLASH*/M_KEY_UNKNOWN,M_KEY_F11,		/* 5 */
	M_KEY_F12,			M_KEY_PAUSE,			M_KEY_UNKNOWN,		M_KEY_LGUI,			M_KEY_RGUI,		/*M_KEY_APPLICATION*/M_KEY_UNKNOWN,	M_KEY_UNKNOWN,		M_KEY_UNKNOWN,	/* 5 */

	M_KEY_UNKNOWN,		M_KEY_UNKNOWN,		M_KEY_UNKNOWN,		M_KEY_UNKNOWN,		M_KEY_F13,		M_KEY_F14,			M_KEY_F15,			M_KEY_F16,		/* 6 */
	M_KEY_F17,			M_KEY_F18,			M_KEY_F19,			M_KEY_UNKNOWN,		M_KEY_UNKNOWN,	M_KEY_UNKNOWN,		M_KEY_UNKNOWN,		M_KEY_UNKNOWN,	/* 6 */
	
	M_KEY_WORLD_2,		M_KEY_UNKNOWN,		M_KEY_UNKNOWN,		M_KEY_WORLD_1,		M_KEY_UNKNOWN,	M_KEY_UNKNOWN,		M_KEY_UNKNOWN,		M_KEY_UNKNOWN,	/* 7 */
	M_KEY_UNKNOWN,		M_KEY_WORLD_4,		M_KEY_UNKNOWN,		M_KEY_WORLD_5,		M_KEY_UNKNOWN,	M_KEY_WORLD_3,		M_KEY_UNKNOWN,		M_KEY_UNKNOWN	/* 7 */
};

static MKey
WindowsScanCodeToMKey(LPARAM lParam, WPARAM wParam)
{
    MKey key;
    char bIsExtended;
    int nScanCode = (lParam >> 16) & 0xFF;

    /* 0x45 here to work around both pause and numlock sharing the same scancode, so use the VK key to tell them apart */
    if (nScanCode == 0 || nScanCode == 0x45) {
        switch(wParam) {
        case VK_CLEAR: return M_KEY_CLEAR;
        case VK_MODECHANGE: return M_KEY_MODE;
        case VK_SELECT: return M_KEY_SELECT;
        case VK_EXECUTE: return M_KEY_EXECUTE;
        case VK_HELP: return M_KEY_HELP;
        case VK_PAUSE: return M_KEY_PAUSE;
        case VK_NUMLOCK: return M_KEY_NUMLOCK;

        case VK_F13: return M_KEY_F13;
        case VK_F14: return M_KEY_F14;
        case VK_F15: return M_KEY_F15;
        case VK_F16: return M_KEY_F16;
        case VK_F17: return M_KEY_F17;
        case VK_F18: return M_KEY_F18;
        case VK_F19: return M_KEY_F19;
        case VK_F20: return M_KEY_F20;
        case VK_F21: return M_KEY_F21;
        case VK_F22: return M_KEY_F22;
        case VK_F23: return M_KEY_F23;
        case VK_F24: return M_KEY_F24;

        case VK_OEM_NEC_EQUAL: return M_KEY_KP_EQUALS;
        /*case VK_BROWSER_BACK: return M_KEY_AC_BACK;
        case VK_BROWSER_FORWARD: return M_KEY_AC_FORWARD;
        case VK_BROWSER_REFRESH: return M_KEY_AC_REFRESH;
        case VK_BROWSER_STOP: return M_KEY_AC_STOP;
        case VK_BROWSER_SEARCH: return M_KEY_AC_SEARCH;
        case VK_BROWSER_FAVORITES: return M_KEY_AC_BOOKMARKS;
        case VK_BROWSER_HOME: return M_KEY_AC_HOME;
        case VK_VOLUME_MUTE: return M_KEY_AUDIOMUTE;
        case VK_VOLUME_DOWN: return M_KEY_VOLUMEDOWN;
        case VK_VOLUME_UP: return M_KEY_VOLUMEUP;

        case VK_MEDIA_NEXT_TRACK: return M_KEY_AUDIONEXT;
        case VK_MEDIA_PREV_TRACK: return M_KEY_AUDIOPREV;
        case VK_MEDIA_STOP: return M_KEY_AUDIOSTOP;
        case VK_MEDIA_PLAY_PAUSE: return M_KEY_AUDIOPLAY;
        case VK_LAUNCH_MAIL: return M_KEY_MAIL;
        case VK_LAUNCH_MEDIA_SELECT: return M_KEY_MEDIASELECT;

        case VK_OEM_102: return M_KEY_NONUSBACKSLASH;*/

        case VK_ATTN: return M_KEY_SYSREQ;
        /*case VK_CRSEL: return M_KEY_CRSEL;
        case VK_EXSEL: return M_KEY_EXSEL;*/
        case VK_OEM_CLEAR: return M_KEY_CLEAR;

        /*case VK_LAUNCH_APP1: return M_KEY_APP1;
        case VK_LAUNCH_APP2: return M_KEY_APP2;*/

        default: return M_KEY_UNKNOWN;
        }
    }

    if (nScanCode > 127)
        return M_KEY_UNKNOWN;

    key = windows_scancode_table[nScanCode];

    bIsExtended = (lParam & (1 << 24)) != 0;
    if (!bIsExtended) {
        switch (key) {
        case M_KEY_HOME:
            return M_KEY_KP7;
        case M_KEY_UP:
            return M_KEY_KP8;
        case M_KEY_PAGEUP:
            return M_KEY_KP9;
        case M_KEY_LEFT:
            return M_KEY_KP4;
        case M_KEY_RIGHT:
            return M_KEY_KP6;
        case M_KEY_END:
            return M_KEY_KP1;
        case M_KEY_DOWN:
            return M_KEY_KP2;
        case M_KEY_PAGEDOWN:
            return M_KEY_KP3;
        case M_KEY_INSERT:
            return M_KEY_KP0;
        case M_KEY_DELETE:
            return M_KEY_KP_PERIOD;
        case M_KEY_PRINT:
            return M_KEY_KP_MULTIPLY;
        default:
            break;
        }
    } else {
        switch (key) {
        case M_KEY_RETURN:
            return M_KEY_KP_ENTER;
        case M_KEY_LALT:
            return M_KEY_RALT;
        case M_KEY_LCTRL:
            return M_KEY_RCTRL;
        case M_KEY_SLASH:
            return M_KEY_KP_DIVIDE;
        case M_KEY_CAPSLOCK:
            return M_KEY_KP_PLUS;
        default:
            break;
        }
    }

    return key;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MWindow* window = (MWindow*)GetProp(hwnd, "MWindow");
    switch(msg) {
        case WM_CLOSE:
            window->quit();
            break;
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            window->keyPressed(WindowsScanCodeToMKey(lParam, wParam), 0);
            break;
        case WM_SYSKEYUP:
        case WM_KEYUP:
            window->keyReleased(WindowsScanCodeToMKey(lParam, wParam), 0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

bool DIBVideoInterface::init()
{
    WNDCLASS wndClass;
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = "M_app";
    wndClass.hInstance = nullptr;
    wndClass.style = CS_OWNDC;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbWndExtra = 0;
    wndClass.cbClsExtra = 0;

    if ( !RegisterClass(&wndClass) ) {
        mDebug(ERROR) << "Couldn't register application class";
        return false;
    }

    hidden = CreateWindow("STATIC", "", WS_POPUP | WS_DISABLED, 0, 0, 1, 1, nullptr, nullptr, nullptr, nullptr);
    ShowWindow(hidden, SW_HIDE);
    dc = GetDC(hidden);

    PIXELFORMATDESCRIPTOR descriptor;
    ZeroMemory(&descriptor, sizeof(descriptor));
    descriptor.nSize        = sizeof(descriptor);
    descriptor.nVersion     = 1;
    descriptor.iLayerType   = PFD_MAIN_PLANE;
    descriptor.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    descriptor.iPixelType   = PFD_TYPE_RGBA;
    descriptor.cColorBits   = static_cast<BYTE>(32);
    descriptor.cDepthBits   = static_cast<BYTE>(0);
    descriptor.cStencilBits = static_cast<BYTE>(0);
    descriptor.cAlphaBits   = 8;

    // Get the pixel format that best matches our requirements
    int bestFormat = ChoosePixelFormat(dc, &descriptor);
    PIXELFORMATDESCRIPTOR actualFormat;
    actualFormat.nSize    = sizeof(actualFormat);
    actualFormat.nVersion = 1;
    DescribePixelFormat(dc, bestFormat, sizeof(actualFormat), &actualFormat);
    SetPixelFormat(dc, bestFormat, &actualFormat);

    rc = wglCreateContext(dc);
    wglMakeCurrent(dc, rc);

    return true;
}

void DIBVideoInterface::fini()
{
    if (wglGetCurrentContext() == rc)
        wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(rc);
    ReleaseDC(hidden, dc);
    DestroyWindow(hidden);
    UnregisterClass("M_app", nullptr);
}

void DIBVideoInterface::handleEvents()
{
    MSG msg;
    while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
        if ( msg.message == WM_QUIT ) break;
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
}

MWindow* DIBVideoInterface::createWindow ( int width, int height )
{
    HWND hwnd = CreateWindow("M_app", "M_app", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                             CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, nullptr, nullptr, nullptr, nullptr);
    if (!hwnd)
        return nullptr;
    auto dc = GetDC(hwnd);
    if (!dc)
        return nullptr;
    auto window = new DIBWindow{width,height,hwnd,dc};
    if (!SetProp(hwnd, "MWindow", window)) {
        destroyWindow ( window );
        return nullptr;
    }
    ShowWindow(window->hwnd, SW_SHOW);
    handleEvents();
    return window;
}

void DIBVideoInterface::destroyWindow ( MWindow* window )
{
    auto w = dynamic_cast<DIBWindow*> ( window );
    if ( !w )
        return;
    if (wglGetCurrentContext() == w->rc)
        wglMakeCurrent(dc, rc);
    wglDeleteContext(w->rc);
    ReleaseDC(w->hwnd, w->dc);
    DestroyWindow(w->hwnd);
    handleEvents();
    MVideoInterface::destroyWindow(window);
}

static DIBVideoInterface iface;

