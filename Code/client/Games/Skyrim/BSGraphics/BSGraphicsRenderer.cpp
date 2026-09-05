
#include "Services/InputService.h"
#include "Systems/RenderSystemD3D11.h"

#include "World.h"

#include "BSGraphics/BSGraphicsRenderer.h"
#include "BSRandom/BSRandom.h"

// shared resource by launcher
extern HICON g_SharedWindowIcon;

namespace BSGraphics
{
namespace
{

static RenderSystemD3D11* g_sRs = nullptr;
static WNDPROC RealWndProc = nullptr;
static RendererWindow* g_RenderWindow = nullptr;

static constexpr char kTogetherWindowName[]{"Skyrim Together"};

} // namespace
RendererWindow* GetMainWindow()
{
    return g_RenderWindow;
}

bool RendererWindow::IsForeground()
{
    return GetForegroundWindow() == hWnd;
}

void (*Renderer_Init)(Renderer*, BSGraphics::RendererInitOSData*, const BSGraphics::ApplicationWindowProperties*, BSGraphics::RendererInitReturn*) = nullptr;

// WNDPROC seems to be part of the renderer
LRESULT CALLBACK Hook_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (InputService::WndProc(hwnd, uMsg, wParam, lParam) != 0)
        return 0;

    return RealWndProc(hwnd, uMsg, wParam, lParam);
}

void Hook_Renderer_Init(Renderer* self, BSGraphics::RendererInitOSData* aOSData, const BSGraphics::ApplicationWindowProperties* aFBData, BSGraphics::RendererInitReturn* aOut)
{
    // we feed this a shared icon as the resource directory of our former launcher data is already overwritten with the
    // game.
    aOSData->hIcon = g_SharedWindowIcon;
    // Append our window name.
    aOSData->pClassName = kTogetherWindowName;

    RealWndProc = aOSData->pWndProc;
    aOSData->pWndProc = Hook_WndProc;

    Renderer_Init(self, aOSData, aFBData, aOut);

    g_sRs = &World::Get().ctx().at<RenderSystemD3D11>();
    // This how the game does it too
    g_RenderWindow = &self->Data.RenderWindowA[0];

    g_sRs->OnDeviceCreation(self->Data.RenderWindowA[0].pSwapChain);
}

void (*StopTimer)(int) = nullptr;

// SANDMAN FORK: added. See NOTICE-SANDMAN.md.
//
// Skyrim confines the mouse pointer to its own window with ClipCursor and never
// lets go, so once the game is running the cursor cannot reach a second monitor
// even after you alt-tab away. That is the whole "the game is hogging my mouse"
// complaint, and it is the engine doing it, not Skyrim Together: ClipCursor
// appears nowhere else in this codebase.
//
// Note what is ALREADY handled and is therefore NOT the cause:
//   - Input polling is skipped while we are not foreground
//     (BSInput/BSInputDeviceManager.cpp), so the game is not reading the mouse.
//   - DirectInput exclusive mode is patched out below (DISCL_NONEXCLUSIVE), so
//     it is not a device grab either.
// The game is not consuming the pointer. It is merely still holding the clip
// rectangle it set while it had focus.
//
// So: release the clip on every frame we are not the foreground window. This is
// deliberately level-triggered rather than fired once on the focus-loss edge,
// because the engine re-applies its clip from its own render path and a
// one-shot release would just be overwritten on the next frame. ClipCursor with
// a null rectangle is a trivial user32 call and this only runs while you are
// tabbed out, when frame cost is irrelevant anyway.
//
// Nothing is done on the way back in, on purpose. The game re-establishes its
// own clip when it regains focus, which is what you want while actually
// playing. This only ever gives the pointer back; it never takes it.
static void ReleaseCursorWhileBackgrounded()
{
    auto* pWindow = GetMainWindow();
    // Null until Hook_Renderer_Init runs; there is no window to be clipped to yet.
    if (!pWindow)
        return;

    if (!pWindow->IsForeground())
        ClipCursor(nullptr);
}

// Insert us at the End
void Hook_StopTimer(int type)
{
    if (g_sRs)
        g_sRs->OnRender();

    ReleaseCursorWhileBackgrounded();

    StopTimer(type);
}

static TiltedPhoques::Initializer s_viewportHooks(
    []()
    {
        const VersionDbPtr<void> initWindowLoc(77226);
        // patch dwStyle in BSGraphics::InitWindows
        TiltedPhoques::Put(mem::pointer(initWindowLoc.GetPtr()) + 0x174 + 1, WS_OVERLAPPEDWINDOW);

        const VersionDbPtr<void> windowLoc(68781);
        // TODO: move me to input patches.
        // don't let the game steal the media keys in windowed mode
        TiltedPhoques::Put(
            mem::pointer(windowLoc.GetPtr()) + 0x55 + 2,
            /*strip DISCL_EXCLUSIVE bits and append DISCL_NONEXCLUSIVE*/ 3);

        const VersionDbPtr<void> timerLoc(77246);
        const VersionDbPtr<void> renderInit(77226);

        TiltedPhoques::SwapCall(mem::pointer(timerLoc.GetPtr()) + 9, StopTimer, &Hook_StopTimer);

        Renderer_Init = static_cast<decltype(Renderer_Init)>(renderInit.GetPtr());

        // Once we find a proper way to locate it for different versions, go back to swapcall
        // TiltedPhoques::SwapCall(mem::pointer(initLoc.GetPtr()) + 0xD1A, Renderer_Init, &Hook_Renderer_Init);
        TP_HOOK_IMMEDIATE(&Renderer_Init, &Hook_Renderer_Init);
    });
} // namespace BSGraphics
