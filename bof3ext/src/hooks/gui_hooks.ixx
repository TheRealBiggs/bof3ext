module;

export module bof3ext.hooks:gui;
export import :gui.menu;
export import :gui.battle;


export void EnableGuiHooks() {
	EnableGuiMenuHooks();
	EnableGuiBattleHooks();
}