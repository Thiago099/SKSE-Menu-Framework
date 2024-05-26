#include "SKSEMenuFramework.h"


void AddSectionItem(const char* path, UI::RenderFunction rendererFunction) { 
    auto pathSplit = SplitString(path, '/');
    AddToTree(UI::RootMenu, pathSplit, rendererFunction, pathSplit.back());
}

UI::WindowInterface* AddWindow(UI::RenderFunction rendererFunction) { 

    auto newWindow = new UI::Window();

    newWindow->Render = rendererFunction;

    UI::Windows.push_back(newWindow);

    return newWindow->Interface;

}
