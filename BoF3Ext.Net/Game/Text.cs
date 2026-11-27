using System;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Text {
	[GameFunction(0x44A8E0, hookable: true)]
	public static partial void LoadTextParameter_BattleAction(byte index, bool isCharacterName);

	[GameFunction(0x44A960, hookable: true)]
	public static partial void LoadTextParameter_EnemyName(byte index);

	[GameFunction(0x4976D0, hookable: true)]
	public static partial void LoadDialogue(ushort index);

	[GameFunction(0x497740, hookable: true)]
	public static partial GameString GetText(ushort index);

	[GameFunction(0x497770)]
	public static partial void InitialiseDialogue();

	[GameFunction(0x4B58F0, hookable: true)]
	public static partial void LoadItemName(byte index, byte type);

	[GameFunction(0x57D800, hookable: true)]
	public static partial byte GetStringLength(GameString text);

	[GameFunction(0x591680, hookable: true)]
	public static partial GameString GetItemName(byte type, byte index);

	[GameFunction(0x591940, hookable: true)]
	public static partial void LoadTextParameter(byte index, byte length, GameString text);


	[GameVariable(0x653818, arrayLength: 6)]
	public static partial Span<Array4<sbyte>> ConfigControllerKeyTexts { get; set; }

	[GameVariable(0x653EC0, arrayLength: 5)]
	public static partial Span<byte> ItemCountFormat { get; set; }

	[GameVariable(0x663970, arrayLength: 5)]
	public static partial Span<GameString> InventoryCategoryTexts { get; set; }

	[GameVariable(0x663984, arrayLength: 4)]
	public static partial Span<GameString> SkillCategoryTexts { get; set; }

	[GameVariable(0x663994, arrayLength: 5)]
	public static partial Span<GameString> EquipmentCategoryTexts { get; set; }

	[GameVariable(0x669CCC, arrayLength: 9)]
	public static partial Span<byte> EmptySaveSlotText { get; set; }

	[GameVariable(0x669DE0, arrayLength: 12)]
	public static partial Span<GameString> BattleActionTexts { get; set; }

	[GameVariable(0x66A118, arrayLength: 8)]
	public static partial Span<byte> InkText { get; set; }

	[GameVariable(0x66A1F0, arrayLength: 8)]
	public static partial Span<byte> MasterTitleText { get; set; }

	[GameVariable(0x66A2DC, arrayLength: 6)]
	public static partial Span<Array4<byte>> ButtonGlyphTexts { get; set; }

	[GameVariable(0x66A2FC, arrayLength: 8)]
	public static partial Span<GameString> ConfigControllerButtonTexts { get; set; }

	[GameVariable(0x66A31C)]
	public static partial ushort ZennyGlyph { get; set; }

	[GameVariable(0x66B36C, arrayLength: 4)]
	public static partial Span<GameString> SkillNoteSortTexts { get; set; }

	[GameVariable(0x66B58C, arrayLength: 5)]
	public static partial Span<GameString> BattleInventoryCategoryTexts { get; set; }

	[GameVariable(0x66B5A0, arrayLength: 4)]
	public static partial Span<GameString> BattleSkillCategoryTexts { get; set; }

	[GameVariable(0x7DEE48)]
	public static partial ushort DialogueIndex { get; set; }

	[GameVariable(0x7DEE4C, isPointer: true)]
	public static partial GameString DialoguePage { set; }

	[GameVariable(0x7DEE50, isPointer: true)]
	public static partial GameString DialogueCursor { set; }

	[GameVariable(0x7DEE60)]
	public static partial short DialogueTextX { get; set; }

	[GameVariable(0x904CE0, arrayLength: 16)]
	public static partial Span<Array32<byte>> TextParameterBuffers { get; set; }
}