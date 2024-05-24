#include "Plugin.h"


SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    Config::Init();
    auto builder = ImGui::Renderer::GetBuilder();
    builder->Install();
    delete builder;
    return true;
}
