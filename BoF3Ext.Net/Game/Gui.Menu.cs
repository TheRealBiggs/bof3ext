using System;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Gui {
	public struct MenuTabGroup {
		public byte Count;
		public Array4<byte> Tabs;
	};


	[GameFunction(0x4434C0, hookable: true)]
	public static partial void DrawLargeMenuIcon(byte id, short x, short y, byte width, byte height, byte whiteness);

	[GameFunction(0x4616F0, hookable: true)]
	public static partial void DrawConfigMenuTabs(byte index, short x, short y);

	[GameFunction(0x461710, hookable: true)]
	public static partial void DrawConfigMenu(short x, short y);

	[GameFunction(0x461800, hookable: true)]
	public static partial void DrawConfigCategory(short x, short y, byte index, byte rowExpand);

	[GameFunction(0x461970, hookable: true)]
	public static partial void DrawConfigValues(short x, short y, byte index, byte selectedIndex, int rowExpand);

	[GameFunction(0x461A50, hookable: true)]
	public static partial void DrawConfigControllerWindow(short x, short y);

	[GameFunction(0x5738A0, hookable: true)]
	public static partial void DrawStatusWindow(short x, short y, byte characterId);

	[GameFunction(0x573A80, hookable: true)]
	public static partial void DrawStatusEquipmentPanel(short x, short y, short characterId);

	[GameFunction(0x573BF0, hookable: true)]
	public static partial void DrawStatusXpPanel(short x, short y, byte characterId, byte nextLevel);

	[GameFunction(0x573F70, hookable: true)]
	public static partial void DrawFormationPanel(ushort x, ushort y, int index, byte flags, int pulse);

	[GameFunction(0x574610, hookable: true)]
	public static partial void DrawZennyPanel(short x, short y, int _, int amount);

	[GameFunction(0x5746C0, hookable: true)]
	public static partial void DrawTimePanel(short x, short y);

	[GameFunction(0x574890, hookable: true)]
	public static partial void DrawMenuTabs(short x, short y, byte index, byte selectedTabIndex);

	[GameFunction(0x574EC0, hookable: true)]
	public static partial void DrawEquipmentWindow(
		byte charId, short x, short y, nint equipment, bool noCompare, ref UiWindow window
	);

	[GameFunction(0x575F50, hookable: true)]
	public static partial void DrawSkillWindow(ref UiWindow window);

	[GameFunction(0x5763F0, hookable: true)]
	public static partial void DrawEquipmentInventoryWindow(ref UiWindow window);

	[GameFunction(0x57D360)]
	public static partial void DrawItemIcon(short x, short y, byte id, bool greyed);

	[GameFunction(0x57DC90, hookable: true)]
	public static partial void DrawSkillInfo(
		short x, short y, int paletteIndex, byte iconId, GameString text, byte cost, bool greyed
	);

	[GameFunction(0x588D20, hookable: true)]
	public static partial void DrawSaves(short x, short y, uint colour);

	[GameFunction(0x5903F0, hookable: true)]
	public static partial void DrawMenuIcon(byte id, short x, short y, byte width, byte height, byte whiteness);

	[GameFunction(0x5905D0, hookable: true)]
	public static partial void DrawCursor(short x, short y);

	[GameFunction(0x599B90, hookable: true)]
	public static partial void DrawMenuCharacterPanel(ref UiWindow window);


	[GameVariable(0x653808, arrayLength: 6)]
	public static partial Span<byte> ConfigValueStartIndices { get; set; }

	[GameVariable(0x653810, arrayLength: 6)]
	public static partial Span<byte> ConfigValueCounts { get; set; }

	[GameVariable(0x663358, arrayLength: 14)]
	public static partial Span<UiPart> StatusWindowLeftUiParts { get; set; }

	[GameVariable(0x663384, arrayLength: 3)]
	public static partial Span<UiPart> StatusWindowMiddleUiParts { get; set; }

	[GameVariable(0x663390, arrayLength: 8)]
	public static partial Span<UiPart> StatusWindowRightUiParts { get; set; }

	[GameVariable(0x6633A8, arrayLength: 4)]
	public static partial Span<UiPart> ZennyPanelLeftUiParts { get; set; }

	[GameVariable(0x6633B4, arrayLength: 3)]
	public static partial Span<UiPart> ZennyPanelMiddleUiParts { get; set; }

	[GameVariable(0x6633C0, arrayLength: 4)]
	public static partial Span<UiPart> ZennyPanelRightUiParts { get; set; }

	[GameVariable(0x663428, arrayLength: 8)]
	public static partial Span<UiPart> MenuTabUiParts { get; set; }

	[GameVariable(0x663440, arrayLength: 8)]
	public static partial Span<UiPart> SelectedMenuTabUiParts { get; set; }

	[GameVariable(0x663458, arrayLength: 14)]
	public static partial Span<UiPart> EquipmentWindowUiParts { get; set; }

	[GameVariable(0x66383C, arrayLength: 9)]
	public static partial Span<MenuTabGroup> MenuTabGroups { get; set; }

	[GameVariable(0x66B090, arrayLength: 4)]
	public static partial Span<Vec2s> MenuOutlineSizes { get; set; }

	[GameVariable(0x8034D0)]
	public static partial uint FirstVisibleSaveIndex { get; set; }

	[GameVariable(0x929F02)]
	public static partial byte ConfigMenuState { get; set; }
}