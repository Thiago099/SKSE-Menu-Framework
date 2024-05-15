#include "SkyrimImgui.h"

 std::unordered_map<std::string, std::function<void()>> menus;

void AddRenderer(const char* menu, std::function<void()> const& rendererFunction) { 
	logger::info("{}", menu);
    menus[menu] = rendererFunction;
}

ImGuiContext * GetContext() {
    return ImGui::GetCurrentContext();
}

