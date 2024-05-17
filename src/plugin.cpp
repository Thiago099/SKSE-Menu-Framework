#include "Plugin.h"

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        // Start
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame) {
        // Post-load
    }
}
SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    Config::Init();
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);

    auto builder = ImGui::Renderer::GetBuilder();
    builder->Install();
    delete builder;
    return true;
}