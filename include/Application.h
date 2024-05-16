#pragma once
#include "Model.h"

extern std::vector<std::function<void(ImGuiContext*)>> contextSetFunctions;
extern MenuTree* root;
extern std::vector<Window*> windows;
void AddToTree(MenuTree* node, std::vector<std::string>& path, std::function<void()>& render);
std::vector<std::string> SplitString(const std::string& input, char delimiter);