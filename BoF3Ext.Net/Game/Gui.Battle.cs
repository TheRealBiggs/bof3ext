using System;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Gui {
	public enum HpApBarType : byte {
		Ap,
		Hp,
	}


	public struct BattleTextPanelData {
		byte byte_0;
		byte byte_1;
		public bool IsCharacterName;
		byte byte_3;
		public GameString Text;
		byte byte_8;
		byte byte_9;
		public byte PaletteIndex;
		byte byte_B;
	}


	[GameFunction(0x443610, hookable: true)]
	public static partial void DrawBattleMessagePanel(short x, short y);

	[GameFunction(0x4439A0, hookable: true)]
	public static partial void DrawBattleCommandTextPanel(byte index);

	[GameFunction(0x443B10, hookable: true)]
	public static partial void DrawBattleEnemyTargetPanel(short x, short y, byte enemyIndex);

	[GameFunction(0x443D90, hookable: true)]
	public static partial void DrawBattleEnemyPanel(short x, short y, byte enemyIndex);

	[GameFunction(0x444A90)]
	public static partial void DrawHpApBar(short x, short y, byte filledWidth, byte emptyWidth, HpApBarType type);

	[GameFunction(0x597230, hookable: true)]
	public static partial void DrawBattleActionTextPanel();

	[GameFunction(0x5985A0, hookable: true)]
	public static partial void DrawBattleXpResultPanel();

	[GameFunction(0x59E160, hookable: true)]
	public static partial void DrawBattleInventoryTabs(short x, short y, sbyte selectedTabIndex);


	[GameVariable(0x64E2C8, arrayLength: 7)]
	public static partial Span<Vec4s> BattleCommandTextPanelRects { get; set; }

	[GameVariable(0x656A34, arrayLength: 4)]
	public static partial Span<(ushort x, ushort y)> EnemyBattlePanelPositions { get; set; }

	[GameVariable(0x66B4F8, arrayLength: 2)]
	public static partial Span<UiPart> BattleInventoryTabUiParts { get; set; }

	[GameVariable(0x66B500, arrayLength: 2)]
	public static partial Span<UiPart> SelectedBattleInventoryTabUiParts { get; set; }

	[GameVariable(0x93B8E0, arrayLength: 8)]
	public static partial Span<BattleTextPanelData> BattleTextPanelDatas { get; set; }
}
