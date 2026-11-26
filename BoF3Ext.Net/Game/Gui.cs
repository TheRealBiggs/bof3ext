using System;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Gui {
	public struct UiPart {
		public sbyte X;
		public sbyte Y;
		public byte SpriteId;
	}

	public struct UiWindow {
		public bool IsActive;
		public byte Type;
		public byte State;
		public byte SubState;
		public short X;
		public short Y;
		public Array8<byte> StateParamsU8;
		public Array8<ushort> StateParamsU16;
		public nint StateParamP;
	}


	[GameFunction(0x404620, hookable: true)]
	public static partial void DrawOverworldRegionPanel(short x, short y);

	[GameFunction(0x408530, hookable: true)]
	public static partial void DrawOverworldCompassNeedle(short x, short y);

	[GameFunction(0x40E750, hookable: true)]
	public static partial void DrawBorderedTextPanel_0(short x, short y, ushort width, ushort height, byte mode);

	[GameFunction(0x497840, hookable: true)]
	public static partial void DrawDialogueText();

	[GameFunction(0x498D20, hookable: true)]
	public static partial void DrawDialoguePageIndicator(short x, short y);

	[GameFunction(0x573CE0, hookable: true)]
	public static partial void DrawSelectedFrameOutline(
		short x, short y, ushort width, ushort height, int pulse, int colour
	);

	[GameFunction(0x574AB0, hookable: true)]
	public static partial void DrawBorderedTextPanel_1(short x, short y, ushort width, ushort height, byte paletteIndex);

	[GameFunction(0x576960, hookable: true)]
	public static partial void DrawSaveEntry(byte index, short x, short y, nint saveFile);

	[GameFunction(0x57CF60)]
	public static partial void DrawWindowBackground(
		short x, short y, ushort width, ushort height, byte flags, byte paletteIndex
	);

	[GameFunction(0x57D860)]
	public static partial void DrawUiPart(short x, short y, byte id, byte flags);

	[GameFunction(0x57D910)]
	public static partial void DrawUiParts(short x, short y, nint parts, byte flags);

	[GameFunction(0x586160, hookable: true)]
	public static partial void DrawBorderedTextPanel_2(short x, short y, ushort width, ushort height, byte paletteIndex);

	[GameFunction(0x5888D0, hookable: true)]
	public static partial void DrawStartMenuItem(short y, byte index, bool greyed);

	[GameFunction(0x595C50, hookable: true)]
	public static partial void DrawDialogueWindow(short x, short y, byte width, byte height);

	[GameFunction(0x595F60, hookable: true)]
	public static partial void DrawDialogueWindowBorder(short x, short y, byte width, byte height);

	[GameFunction(0x5961C0, hookable: true)]
	public static partial GameString DrawDialogueChoice(short x, short y, GameString text);


	[GameVariable(0x66386C, arrayLength: 6)]
	public static partial Span<byte> EquipWindowItemIconIds { get; set; }

	[GameVariable(0x6BDF90, arrayLength: 3)]
	public static partial Span<byte> StartMenuItemWhiteness { get; set; }

	[GameVariable(0x905B84, isPointer: true)]
	public static partial ref UiWindow CurrentUiWindow { get; }
}