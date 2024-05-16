#include "Application.h"
#include "Model.h"

MenuTree* root = new MenuTree();
std::vector<Window*> windows;

void AddToTree(MenuTree* node, std::vector<std::string>& path, std::function<void()>& render) {
    if (!path.empty()) {
        auto currentName = path.front();
        path.erase(path.begin());

        auto foundItem = node->Children.find(currentName);
        if (foundItem != node->Children.end()) {
            AddToTree(foundItem->second, path, render);
        } else {
            auto newItem = new MenuTree();
            node->Children[currentName] = newItem;
            AddToTree(newItem, path, render);
        }
    } else {
        node->Render = render;
    }
}

std::vector<std::string> SplitString(const std::string& input, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(input);
    std::string part;

    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }

    return parts;
}

