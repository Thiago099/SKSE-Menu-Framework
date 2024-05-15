
#pragma once
#include "HookBuilder.h"
#include "Input.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <dxgi.h>
#include <imgui.h>
#include <imgui_internal.h>


namespace ImGui {
    class Renderer {

    public:
        static HookBuilder* GetBuilder();
        static void Render();
        static inline std::atomic<bool> initialized{false};
        static inline std::atomic<bool> isOpen{false};
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
}

