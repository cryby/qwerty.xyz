// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"

bool __fastcall hooks::hooked_drawfog(void* ecx, void* edx)
{
	return !config_system.g_cfg.esp.removals[REMOVALS_FOGS] || config_system.g_cfg.esp.fog;
}