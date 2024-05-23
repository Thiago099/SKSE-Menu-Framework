#pragma once
#include <imgui/imgui.h>
#include "Application.h"
#include "Renderer.h"


#define EXPORT_FUNCTION extern "C" [[maybe_unused]] __declspec(dllexport)

EXPORT_FUNCTION void AddSection(const char* path, RenderFunction rendererFunction);
EXPORT_FUNCTION void AddWindow(const char* key, ImGuiWindowFlags flags, std::function<void()> const& rendererFunction);