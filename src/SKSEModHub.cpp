#include "SKSEModHub.h"


void AddSection(const char* path, RenderFunction rendererFunction) { 
    auto pathSplit = SplitString(path, '/');
    AddToTree(root, pathSplit, rendererFunction, pathSplit.back());
}
void AddWindow(const char* name, ImGuiWindowFlags flags, RenderFunction rendererFunction) {
    auto window = new Window();
    window->Flags = flags;
    window->Name = name;
    window->Render = rendererFunction;
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

