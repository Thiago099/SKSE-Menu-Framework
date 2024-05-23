#include "SKSEModHub.h"


void AddSection(const char* path, RenderFunction rendererFunction) { 
    auto pathSplit = SplitString(path, '/');
    AddToTree(root, pathSplit, rendererFunction, pathSplit.back());
}
