#include "SKSEModHub.h"


void AddSection(const char* path, RenderFunction rendererFunction) { 
    auto pathSplit = SplitString(path, '/');
    AddToTree(root, pathSplit, rendererFunction, pathSplit.back());
}
void AddWindow(const char* name, ImGuiWindowFlags flags, RenderFunction rendererFunction) {
    auto window = new Window();
    window->Flags = flags;
    window->Name = name;
    window->Render = rendererFunction;
    windows.push_back(window);
}
