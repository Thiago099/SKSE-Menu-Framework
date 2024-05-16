#pragma once
#include <imgui.h>
#include "Application.h"



#define EXPORT_FUNCTION extern "C" [[maybe_unused]] __declspec(dllexport)

EXPORT_FUNCTION void AddSection(const char* menu, std::function<void()> const& rendererFunction);

EXPORT_FUNCTION ImGuiContext* GetContext();

