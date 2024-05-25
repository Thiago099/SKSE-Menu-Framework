#include "Renderer.h"

std::vector<UI::Window*> UI::Windows;

LRESULT UI::WndProcHook::thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KILLFOCUS) {
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
    return func(hWnd, uMsg, wParam, lParam);
}

void UI::D3DInitHook::thunk() {
    logger::debug("[D3DInitHook] START");

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
    logger::debug("[D3DInitHook] FINISH");
}
void UI::DXGIPresentHook::thunk(std::uint32_t a_timer) {
    originalFunction(a_timer);

    if (!Renderer::initialized.load()) {
        return;
    }

    if (!IsAnyWindowOpen()) {
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
    ImGui::NewFrame();
    Renderer::RenderWindows();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
void SetGameLock(bool doLock) {

    if (Config::FreezeTimeOnMenu) {
        const auto main = RE::Main::GetSingleton();
        if (doLock) {
            main->freezeTime = true;
        } else {
            main->freezeTime = false;
        }
    }
    if (!doLock) {
        auto& io = ImGui::GetIO();
        memset(io.KeysData, 0, sizeof(io.KeysData));
    }
}

void ProcessOpenClose(RE::InputEvent* const* evns) {
    if (!*evns) return;

    for (RE::InputEvent* e = *evns; e; e = e->next) {
        if (e->eventType.get() != RE::INPUT_EVENT_TYPE::kButton) continue;
        const RE::ButtonEvent* a_event = e->AsButtonEvent();
        if (!a_event->IsDown() || a_event->GetDevice() != RE::INPUT_DEVICE::kKeyboard) continue;
        if (a_event->GetIDCode() == Config::ToggleKey) {
            UI::MainInterface->IsOpen = !UI::MainInterface->IsOpen.load();
        }
        if (a_event->GetIDCode() == REX::W32::DIK_ESCAPE) {
            UI::MainInterface->IsOpen = false;
        }

    }
}


void UI::ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher,
                                      RE::InputEvent* const* a_event) {
    ProcessOpenClose(a_event);
    if (IsAnyWindowOpen()) {
        constexpr RE::InputEvent* const dummy[] = {nullptr};
        originalFunction(a_dispatcher, dummy);
        TranslateInputEvent(a_event);
    } else {
        originalFunction(a_dispatcher, a_event);
    }
}

HookBuilder* UI::Renderer::GetBuilder() {
    auto builder = new HookBuilder();
    builder->AddCall<D3DInitHook, 5, 15>(75595, 0x9, 77226, 0x275, 0xDC5530, 0x9);
    builder->AddCall<DXGIPresentHook, 5, 15>(75461, 0x9, 77246, 0x9, 0xDBBDD0, 0x15);
    builder->AddCall<ProcessInputQueueHook, 5, 15>(67315, 0x7B, 68617, 0x7B, 0xC519E0, 0x81);
    return builder;
}

void UI::Renderer::RenderWindows() {
    for (const auto window : Windows) {
        if (window->Interface->IsOpen) {
            window->Render();
        }
    }
}

bool UI::IsAnyWindowOpen() {
    auto it = std::find_if(Windows.begin(), Windows.end(), [](UI::Window* x) { return x->Interface->IsOpen.load(); });
    return it != Windows.end();
}
