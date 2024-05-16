#include "SkyrimImgui.h"


void AddSection(const char* menu, std::function<void()> const& rendererFunction) { 
    std::function<void()> copyOfRenderer = rendererFunction;
    auto path = SplitString(menu,'/');
    AddToTree(root, path, copyOfRenderer);
}
void AddWindow(const char* name, ImGuiWindowFlags flags, std::function<void()> const& rendererFunction) {
    std::function<void()> copyOfRenderer = rendererFunction;
    auto window = new Window();
    window->Flags = flags;
    window->Name = name;
    window->Render = copyOfRenderer;
    windows.push_back(window);
}
ImGuiContext * GetContext() {
    return ImGui::GetCurrentContext();
}

