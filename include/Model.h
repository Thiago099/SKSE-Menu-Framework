#pragma once

#include <imgui.h>


class MenuTree {
public:
    std::map<std::string, MenuTree*> Children;
    std::function<void()> Render;
};

class Window {
public:
    std::string Name;
    ImGuiWindowFlags Flags;
    std::function<void()> Render;
};