#pragma once
#include <imgui/imgui.h>
#include "Renderer.h"
namespace UI {
    class MenuTree {
    public:
        std::map<std::string, MenuTree*> Children;
        RenderFunction Render;
        std::string Title;
    };
    extern UI::MenuTree* RootMenu;
    void __stdcall RenderMenuWindow();
    void AddToTree(UI::MenuTree* node, std::vector<std::string>& path, UI::RenderFunction render, std::string title);
}