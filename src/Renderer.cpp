#include "Renderer.h"



LRESULT ImGui::WndProcHook::thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KILLFOCUS) {
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
    return func(hWnd, uMsg, wParam, lParam);
}

void ImGui::D3DInitHook::thunk() {
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
    ImGui::NewFrame();
    Renderer::Render();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ProcessOpenClose(RE::InputEvent* const* evns) {
     if (!*evns) return;

     for (RE::InputEvent* e = *evns; e; e = e->next) {
        if (e->eventType.get() != RE::INPUT_EVENT_TYPE::kButton) continue;
        const RE::ButtonEvent* a_event = e->AsButtonEvent();
        if (a_event->IsPressed() || a_event->IsHeld() || a_event->GetDevice() != RE::INPUT_DEVICE::kKeyboard) continue;
        if (a_event->GetIDCode() == Config::ToggleKey) {
            ImGui::Renderer::isOpen = !ImGui::Renderer::isOpen;
        }
        if ( a_event->GetIDCode() == REX::W32::DIK_ESCAPE) {
            ImGui::Renderer::isOpen = false;
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

static ImGuiTreeNodeFlags base_flags =
     ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
 static int selection_mask = (1 << 2);

size_t item_current_idx = 0;
size_t node_id = 0;
MenuTree* display_node;

void DummyRenderer(std::pair<const std::string, MenuTree*>& node) { 
    ++node_id; 
    for (auto& item : node.second->Children) {
        DummyRenderer(item);
    }
}


 void RenderNode(std::pair<const std::string, MenuTree*>& node) {
    ++node_id;
     ImGuiTreeNodeFlags node_flags = base_flags;
     //const bool is_selected = item_current_idx == i;
     if (item_current_idx == node_id) node_flags |= ImGuiTreeNodeFlags_Selected;

     if (node.second->Children.size() == 0) {
         node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; 
     }
     bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)node_id, node_flags, node.first.c_str(), node_id);
     if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
         if (node.second->Render) {
             item_current_idx = node_id;
             display_node = node.second;
         }
     }
     if (node_open && node.second->Children.size() != 0) {
         for (auto & item : node.second->Children) {
             RenderNode(item);
         }
         ImGui::TreePop();
     } else {
         for (auto& item : node.second->Children) {
             DummyRenderer(item);
         }
     }
 }
 static ImGuiTextFilter filter;
 int frame = 0;
 void ImGui::Renderer::Render() {
     auto viewport = ImGui::GetMainViewport();
     ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});
     ImGui::SetNextWindowSize(ImVec2{viewport->Size.x * 0.8f, viewport->Size.y * 0.8f}, ImGuiCond_Appearing);
     ImGuiWindowFlags window_flags = 0;
     window_flags |= ImGuiWindowFlags_NoCollapse;
     ImGui::Begin("SKSE Mod Hub", nullptr, window_flags);

        ImGui::BeginChild("TreeView2", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, 20), ImGuiChildFlags_None,
                       window_flags);
        filter.Draw("##SKSEModHubMenuFilter", -FLT_MIN);
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("SKSEModHubModMenuHeader", ImVec2(0, 20), ImGuiChildFlags_None,
                          window_flags);
        if (display_node) {
            auto windowWidth = ImGui::GetWindowSize().x ;
            auto textWidth = ImGui::CalcTextSize(display_node->Title.c_str()).x;
            float offsetX = (windowWidth - textWidth) * 0.5f;
            ImGui::SetCursorPosX(offsetX);
            ImGui::Text(display_node->Title.c_str());
        } else {
            //TODO: Default title
        }
        ImGui::EndChild();

        ImGui::BeginChild("SKSEModHubTreeView", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, -FLT_MIN), ImGuiChildFlags_Border,
                            window_flags);
        node_id = 0;
        for (const auto & item : root->Children) {
            if (filter.PassFilter(item.first.c_str())&&(ImGui::CollapsingHeader(std::format("{}##{}", item.first, node_id).c_str()))) {
                for (auto node : item.second->Children) {
                    RenderNode(node);
                }
            } else {
                for (auto node : item.second->Children) {
                    DummyRenderer(node);
                }
            }
        }
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("SKSEModHubMenuNode", ImVec2(0, -FLT_MIN), ImGuiChildFlags_Border, window_flags);
         if (display_node) {
            display_node->Render();
         } 
        ImGui::EndChild();

 
    ImGui::End();

    for (const auto window : windows) {
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});
        ImGui::SetNextWindowSize(ImVec2{viewport->Size.x * 0.8f, viewport->Size.y * 0.8f}, ImGuiCond_Appearing);
        ImGui::Begin(window->Name.c_str(), nullptr, window->Flags);
        ImGui::BeginChild("ModMenu2", ImVec2(0, -FLT_MIN), ImGuiChildFlags_None, window_flags);
        window->Render();
        ImGui::EndChild();
        ImGui::End();
    }
 }

