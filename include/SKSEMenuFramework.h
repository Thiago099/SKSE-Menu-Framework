#pragma once
#include <imgui/imgui.h>


#ifdef IS_HOST_PLUGIN
    #include "Application.h"
    #include "Renderer.h"
    #include "UI.h"
#endif

#ifdef IS_HOST_PLUGIN
    #define FUNCTION_PREFIX extern "C" [[maybe_unused]] __declspec(dllexport)
#else
    #define FUNCTION_PREFIX extern "C" __declspec(dllimport)
#endif

#ifdef IS_HOST_PLUGIN
FUNCTION_PREFIX void AddSectionItem(const char* path, UI::RenderFunction rendererFunction);
FUNCTION_PREFIX UI::WindowInterface* AddWindow(UI::RenderFunction rendererFunction);
#else
    #define MENU_WINDOW SKSEMenuFramework::Model::WindowInterface*
    namespace SKSEMenuFramework{
        namespace Model {
            class WindowInterface {
                public:
                std::atomic<bool> IsOpen{false};
            };
            typedef void(__stdcall* RenderFunction)();
        }

        namespace Internal {

            inline std::string key;

            FUNCTION_PREFIX void AddSectionItem(const char* path, Model::RenderFunction rendererFunction);
        }

        inline void AddSectionItem(std::string menu, Model::RenderFunction rendererFunction) {
            Internal::AddSectionItem((Internal::key + "/" + menu).c_str(), rendererFunction);
        }
        FUNCTION_PREFIX Model::WindowInterface* AddWindow(Model::RenderFunction rendererFunction);

        inline void SetSection(std::string key) { Internal::key = key; }
    }
#endif
