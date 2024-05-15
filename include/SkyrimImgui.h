#pragma once
#include <imgui.h>

extern std::unordered_map<std::string, std::function<void()>> menus;

extern "C"
    [[maybe_unused]] __declspec(dllexport) void AddRenderer(const char* menu, std::function<void()> const& rendererFunction);

extern "C" [[maybe_unused]] __declspec(dllexport) ImGuiContext* GetContext();