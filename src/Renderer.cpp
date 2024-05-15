#include "Renderer.h"
#include "SkyrimImgui.h"


LRESULT ImGui::WndProcHook::thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KILLFOCUS) {
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
    return func(hWnd, uMsg, wParam, lParam);
}

void ImGui::D3DInitHook::thunk() {
    originalFunction();

    const auto renderer = RE::BSGraphics::Renderer::GetSingleton();
    if (!renderer) {
        SKSE::log::error("couldn't find renderer");
        return;
    }

    const auto swapChain = (IDXGISwapChain*)renderer->data.renderWindows[0].swapChain;
    if (!swapChain) {
        SKSE::log::error("couldn't find swapChain");
        return;
    }

    DXGI_SWAP_CHAIN_DESC desc{};
    if (FAILED(swapChain->GetDesc(std::addressof(desc)))) {
        SKSE::log::error("IDXGISwapChain::GetDesc failed.");
        return;
    }

    const auto device = (ID3D11Device*)renderer->data.forwarder;
    const auto context = (ID3D11DeviceContext*)renderer->data.context;

    SKSE::log::info("Initializing ImGui...");

    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.IniFilename = nullptr;
    io.MouseDrawCursor = true;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
        SKSE::log::error("ImGui initialization failed (Win32)");
        return;
    }

    if (!ImGui_ImplDX11_Init(device, context)) {
        SKSE::log::error("ImGui initialization failed (DX11)");
        return;
    }

    Renderer::initialized.store(true);

    SKSE::log::info("ImGui initialized.");

    WndProcHook::func = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrA(desc.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook::thunk)));
    if (!WndProcHook::func) {
        SKSE::log::error("SetWindowLongPtrA failed!");
    }

}

void ImGui::DXGIPresentHook::thunk(std::uint32_t a_timer) {
    originalFunction(a_timer);

    if (!Renderer::initialized.load()) {
        return;
    }

    if (!Renderer::isOpen.load()) {
        return;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    {
        // trick imgui into rendering at game's real resolution (ie. if upscaled with Display Tweaks)
        static const auto screenSize = RE::BSGraphics::Renderer::GetScreenSize();

        auto& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(screenSize.width);
        io.DisplaySize.y = static_cast<float>(screenSize.height);
    }
    { 
        ImGui::NewFrame();
        Renderer::Render();
    }
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ProcessOpenClose(RE::InputEvent* const* evns) {
     if (!*evns) return;

     for (RE::InputEvent* e = *evns; e; e = e->next) {
        if (e->eventType.get() != RE::INPUT_EVENT_TYPE::kButton) continue;
        const RE::ButtonEvent* a_event = e->AsButtonEvent();
        if (a_event->IsPressed() || a_event->IsHeld()) continue;

        if (a_event->GetIDCode() == RE::BSKeyboardDevice::Key::kF1) {
            ImGui::Renderer::isOpen = !ImGui::Renderer::isOpen;
        }
     }
}

void ImGui::ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher,
                                         RE::InputEvent* const* a_event) {
    ProcessOpenClose(a_event);
    if (ImGui::Renderer::isOpen) {
        constexpr RE::InputEvent* const dummy[] = {nullptr};
        originalFunction(a_dispatcher, dummy);
    } else {
        originalFunction(a_dispatcher, a_event);
    }
    ImGui::TranslateInputEvent(a_event);
}


 HookBuilder* ImGui::Renderer::GetBuilder() {
    auto builder = new HookBuilder();
     builder->AddCall<D3DInitHook, 5, 15>(75595, 0x9, 77226, 0x275, 0xDC5530, 0x9);
    builder->AddCall<DXGIPresentHook, 5, 15>(75461, 0x9, 77246, 0x9, 0xDBBDD0, 0x15); 
    builder->AddCall<ProcessInputQueueHook, 5, 15>(67315, 0x7B, 68617, 0x7B, 0xC519E0, 0x81);
    return builder;
 }

bool modify_palyer_speed = false;
bool modify_horse_speed = false;
bool modify_npc_speed = false;
int FastTravelSpeedThreshold = 100;
int GodModeSpeedMutiplier = 200;

float bar = 0.5f;
int foo = 100;

void ImGui::Renderer::Render() {
     auto viewport = ImGui::GetMainViewport();
     ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});
     ImGui::SetNextWindowSize(ImVec2{viewport->Size.x * 0.8f, viewport->Size.y * 0.6f}, ImGuiCond_Appearing);
     ImGuiWindowFlags window_flags = 0;
     window_flags |= ImGuiWindowFlags_NoCollapse;
     ImGui::Begin("Dynamic Speed", nullptr, window_flags);

     menus["test"]();

    ImGui::End();
 }

