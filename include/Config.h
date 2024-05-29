#pragma once
#include "Application.h"

enum MenuStyle {
    Skyrim,
    Modern,
    Classic
};

class Config {
    public:
    static void Init();
    static unsigned int ToggleKey;
    static bool FreezeTimeOnMenu;
    static bool BlurBackground;
    static MenuStyle MenuStyle;
};