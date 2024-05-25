#include "Config.h"

unsigned int Config::ToggleKey = 0x3B;
bool Config::FreezeTimeOnMenu = true;

void Config::Init() {
	const auto ini = new Utils::Ini("SKSEModHub.ini");
    ini->SetSection("General");
    ToggleKey = GetKeyBinding(ini->GetString("ToggleKey", "f1"));
    FreezeTimeOnMenu = ini->GetBool("FreezeTimeOnMenu", true);
    delete ini;
}