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
FUNCTION_PREFIX void PushSolid();
FUNCTION_PREFIX void PushRegular();
FUNCTION_PREFIX void PushBrands();
FUNCTION_PREFIX void Pop();
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
    namespace FontAwesome {
        FUNCTION_PREFIX void PushSolid();
        FUNCTION_PREFIX void PushRegular();
        FUNCTION_PREFIX void PushBrands();
        FUNCTION_PREFIX void Pop();
        inline std::string UnicodeToUtf8(unsigned int codepoint) {
            std::string utf8;
            if (codepoint <= 0x7F) {
                utf8.push_back(static_cast<char>(codepoint));
            } else if (codepoint <= 0x7FF) {
                utf8.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
                utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
            } else if (codepoint <= 0xFFFF) {
                utf8.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
                utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
            } else if (codepoint <= 0x10FFFF) {
                utf8.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
                utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
            }
            return utf8;
        }
    }
#endif
