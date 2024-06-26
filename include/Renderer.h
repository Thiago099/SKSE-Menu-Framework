
#pragma once
#include "HookBuilder.h"
#include "Application.h"
#include "Config.h"
#include "Input.h"
#include "Model.h"
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <dxgi.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace GameLock {
    enum State { None, Locked, Unlocked };
    extern State lastState;
    void SetState(State currentState);
}

namespace UI {

    class WindowInterface;
    inline WindowInterface* MainInterface;

    typedef void(__stdcall* RenderFunction)();

    class WindowInterface {
        public:
        std::atomic<bool> IsOpen{false};
    };
    class Window {
    public:
        Window() {
            Interface= new WindowInterface;
        }
        WindowInterface* Interface;
        RenderFunction Render;
    };

    extern std::vector<UI::Window*> Windows;

    class Renderer {
    public:
        static HookBuilder* GetBuilder();
        static void RenderWindows();
        static inline std::atomic<bool> initialized{false};
    };

    struct WndProcHook {
        static LRESULT thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static inline WNDPROC func;
    };

    struct D3DInitHook {
        static void thunk();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    struct DXGIPresentHook {
        static void thunk(std::uint32_t a_timer);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    struct ProcessInputQueueHook {
        static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    namespace UserInput {
        void UI::TranslateInputEvent(RE::InputEvent* const* a_event);
    }
    bool IsAnyWindowOpen();
    void TransparentStyle();
    void ModernStyle();
    void LoadFontAwesome(ImGuiIO& io);
    void LoadSkyrimFont(ImGuiIO& io);

    extern ImFont* faSolid;
    extern ImFont* faRegular;
    extern ImFont* faBrands;
    extern ImFont* defaultFont;
}



