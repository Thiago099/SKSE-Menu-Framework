#pragma once
#include <imgui/imgui.h>

#ifdef IS_HOST_PLUGIN
    #include "Application.h"
    #include "Renderer.h"
#endif

#ifdef IS_HOST_PLUGIN
    #define FUNCTION_PREFIX extern "C" [[maybe_unused]] __declspec(dllexport)
#else
    #define FUNCTION_PREFIX extern "C" __declspec(dllimport)
    typedef void(__stdcall* RenderFunction)();
#endif

#ifdef IS_HOST_PLUGIN
FUNCTION_PREFIX void AddSection(const char* path, RenderFunction rendererFunction);
FUNCTION_PREFIX void AddWindow(const char* key, ImGuiWindowFlags flags, std::function<void()> const& rendererFunction);
#else
    namespace SKSEModHubInternal {
        inline std::string key;

        FUNCTION_PREFIX void AddSection(const char* path, RenderFunction rendererFunction);
    }
    namespace SKSEModHub {
        FUNCTION_PREFIX void AddWindow(const char* key, ImGuiWindowFlags flags,
                                       std::function<void()> const& rendererFunction);


        inline void AddSection(std::string menu, RenderFunction rendererFunction) {
            SKSEModHubInternal::AddSection((SKSEModHubInternal::key + "/" + menu).c_str(), rendererFunction);
        }

        inline void Init(std::string key) { SKSEModHubInternal::key = key; }
    }
#endif
