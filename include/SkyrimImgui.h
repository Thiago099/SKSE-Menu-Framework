#pragma once
#include <imgui.h>



class MenuTree {
public:
    std::vector<MenuTree*> Children;
    std::string Name;
    std::function<void()> Render;
};

#define EXPORT_FUNCTION extern "C" [[maybe_unused]] __declspec(dllexport)

EXPORT_FUNCTION void AddSection(const char* menu, std::function<void()> const& rendererFunction);

EXPORT_FUNCTION ImGuiContext* GetContext();

extern MenuTree* root;
