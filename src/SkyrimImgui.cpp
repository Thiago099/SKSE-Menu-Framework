#include "SkyrimImgui.h"


void AddSection(const char* menu, std::function<void()> const& rendererFunction) { 
    std::function<void()> copyOfRenderer = rendererFunction;
    auto path = SplitString(menu,'/');
    AddToTree(root, path, copyOfRenderer);
}
ImGuiContext * GetContext() {
    return ImGui::GetCurrentContext();
}

