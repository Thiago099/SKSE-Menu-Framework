#pragma once
class MenuTree {
public:
    std::map<std::string, MenuTree*> Children;
    std::function<void()> Render;
};
