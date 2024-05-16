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
EXPORT_FUNCTION void SetContextFetch(std::function<void(ImGuiContext*)> contextSetFunction) {
    if (ImGui::Renderer::initialized.load()) {
        contextSetFunction(ImGui::GetCurrentContext());
    } else {
        std::function<void(ImGuiContext*)> copyOfContextSetFunction = contextSetFunction;
        contextSetFunctions.push_back(copyOfContextSetFunction);
    }
}

