module;

export module bof3ext.hooks:gui;
export import :gui.battle;
export import :gui.menu;
export import :gui.skilllist;
export import :gui.start;


export void EnableGuiHooks() {
	EnableGuiBattleHooks();
	EnableGuiMenuHooks();
	EnableGuiSkillListHooks();
	EnableGuiStartHooks();
}