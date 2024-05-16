#include "SkyrimImgui.h"



void AddToTree(MenuTree* node, std::vector<std::string>& path, std::function<void()>& render) {
    if (!path.empty()) {
        auto currentName = path.front();
        path.erase(path.begin());
        for (auto item : node->Children) {
            if (item->Name == currentName) {
                AddToTree(item, path, render);
                return;
            }
        }
        auto newItem = new MenuTree();
        newItem->Name = currentName;
        node->Children.push_back(newItem);
        AddToTree(newItem, path, render);
    } else {
        node->Render = render;
    }
}

std::vector<std::string> splitString(const std::string& input, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(input);
    std::string part;

    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }

    return parts;
}

MenuTree* root = new MenuTree();

void AddSection(const char* menu, std::function<void()> const& rendererFunction) { 
    std::function<void()> copyOfRenderer = rendererFunction;
    auto path = splitString(menu,'/');
    AddToTree(root, path, copyOfRenderer);
}
ImGuiContext * GetContext() {
    return ImGui::GetCurrentContext();
}

