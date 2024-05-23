#pragma once

#include <imgui/imgui.h>

typedef void(__stdcall* RenderFunction)();

class MenuTree {
public:
    std::map<std::string, MenuTree*> Children;
    RenderFunction Render;
    std::string Title;
};
