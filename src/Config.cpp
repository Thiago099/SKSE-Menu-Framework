#include "Config.h"

int Config::ToggleKey = 0x3B;

void Config::Init() {
	const auto ini = new Ini("SKSEModHub.ini");
    ini->SetSection("General");
    ToggleKey = GetKeyBinding(ini->GetString("ToggleKey", "f1"));
    delete ini;
}