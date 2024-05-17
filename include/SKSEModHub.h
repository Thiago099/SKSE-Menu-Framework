#pragma once
#include <imgui.h>
#include "Application.h"
#include "Renderer.h"


#define EXPORT_FUNCTION extern "C" [[maybe_unused]] __declspec(dllexport)

EXPORT_FUNCTION void AddSection(const char* path, std::function<void()> const& rendererFunction);
EXPORT_FUNCTION void AddWindow(const char* key, ImGuiWindowFlags flags, std::function<void()> const& rendererFunction);
EXPORT_FUNCTION void SetContextFetch(std::function<void(ImGuiContext*)> contextSetFunction);