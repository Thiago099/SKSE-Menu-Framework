#pragma once
#include "Model.h"
extern MenuTree* root;

void AddToTree(MenuTree* node, std::vector<std::string>& path, std::function<void()>& render);
std::vector<std::string> SplitString(const std::string& input, char delimiter);