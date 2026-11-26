using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using unsafe GetBattleLearnedSkillNameFixSig = delegate* unmanaged[Cdecl]<void>;
using unsafe PrintLevelUpSkillNameFixSig = delegate* unmanaged[Cdecl]<nint, nint, BoF3Ext.GameString, int>;

namespace BoF3Ext.Patches;


static class Text {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void GetBattleLearnedSkillNameFix() {
		var enemyId = Game.Battle.CurrentAttackerIndex - 3;
		var id = Game.Battle.EnemyBattleDatas[enemyId].SkillId;

		TextManager.Instance.TryGetSkillName(id, out var name);

		Span<byte> buffer = Game.Text.TextParameterBuffers[1];
		Span<byte> nameSpan = new(name.GameString.Pointer, name.ByteCount + 1);

		nameSpan.CopyTo(buffer);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe int PrintLevelUpSkillNameFix(nint buffer, nint format, GameString name) {
		var id = (uint)name.Pointer - (uint)Unsafe.AsPointer(ref Game.Skill.SkillDatas[0]);
		id /= (uint)Unsafe.SizeOf<Game.Skill.SkillData>();

		ReadOnlySpan<byte> newName = TextManager.Instance.TryGetSkillName(id, out var text)
			? new(text.GameString.Pointer, text.ByteCount)
			: MemoryMarshal.CreateReadOnlySpanFromNullTerminated(name.Pointer);

		Span<byte> bufferSpan = new((void*)buffer, 32);

		newName.CopyTo(bufferSpan);
		bufferSpan[newName.Length] = 0;

		return newName.Length;
	}


	public static unsafe void Apply() {
		ProtectedMemory.WriteCall(0x42FED7, (GetBattleLearnedSkillNameFixSig)(&GetBattleLearnedSkillNameFix), 73);
		ProtectedMemory.WriteCall(0x598246, (PrintLevelUpSkillNameFixSig)(&PrintLevelUpSkillNameFix));
		ProtectedMemory.WriteCall(0x5982A9, (PrintLevelUpSkillNameFixSig)(&PrintLevelUpSkillNameFix));

		// Fix glyph advances
		var advance = (int)Math.Round(GlyphManager.Instance.ScaledGlyphAdvance);

		ProtectedMemory.Write(0x4978DD, (byte)12);          // Dialogue line height
		ProtectedMemory.Write(0x4979B7, (short)-advance);   // Dialogue X offset for `"`
		ProtectedMemory.Write(0x497A47, (byte)advance);     // Dialogue advance
		ProtectedMemory.Write(0x5962E7, (byte)advance);     // Dialogue choices advance

		ProtectedMemory.Write(0x4979A3, (byte)'"');         // Replace `<` with `"` for dialogue text positioning

		// Set default character names
		// TODO: Read these from txt files?
		var characters = Game.World.DefaultCharacterDatas;

		"Ryu\0"u8.CopyTo(characters[0].Name);
		"Nina\0"u8.CopyTo(characters[1].Name);
		"Garr\0"u8.CopyTo(characters[2].Name);
		"Teepo\0"u8.CopyTo(characters[3].Name);
		"Rei\0"u8.CopyTo(characters[4].Name);
		"Momo\0"u8.CopyTo(characters[5].Name);

		// Fix item count format string
		Game.Text.ItemCountFormat[0] = (byte)'x'; // *%2d -> x%2d

		var textMgr = TextManager.Instance;

		// Translate item category texts
		for (uint i = 0; i < 5; ++i) {
			if (!textMgr.TryGetCategoryText(i, out var text))
				break;

			Game.Text.InventoryCategoryTexts[(int)i] = text.GameString;
			Game.Text.EquipmentCategoryTexts[(int)i] = text.GameString;
			Game.Text.BattleInventoryCategoryTexts[(int)i] = text.GameString;
		}

		// Translate skill category texts
		for (uint i = 0; i < 4; ++i) {
			if (!textMgr.TryGetCategoryText(5 + i, out var text))
				break;

			Game.Text.BattleSkillCategoryTexts[(int)i] = text.GameString;
			Game.Text.SkillCategoryTexts[(int)i] = text.GameString;
		}

		// Translate battle action texts because the game is stupid and reads it multiple ways
		for (uint i = 0; i < 12; ++i) {
			if (!textMgr.TryGetBattleActionText(i, out var text))
				break;

			Game.Text.BattleActionTexts[(int)i] = text.GameString;
		}

		// Remove text from empty save slots that was added for the PC port
		Game.Text.EmptySaveSlotText[0] = 0;   // Set first char as null-terminator

		Game.Text.ZennyGlyph = TextManager.EncodeCharacter('ƶ');

		// Master window category text
		if (textMgr.TryGetMasterText(out var masterText)) {
			new ReadOnlySpan<byte>(
				masterText.GameString.Pointer,
				masterText.ByteCount + 1
			).CopyTo(Game.Text.MasterTitleText);
		}

		// Skill note window text
		var skillNoteSortTexts = Game.Text.SkillNoteSortTexts;

		for (uint i = 0; i < 4; ++i) {
			if (!textMgr.TryGetSkillNoteText(i, out var text))
				break;

			skillNoteSortTexts[(int)i] = text.GameString;
		}

		if (textMgr.TryGetSkillNoteText(4, out var inkText))
			new ReadOnlySpan<byte>(inkText.GameString.Pointer, inkText.ByteCount + 1).CopyTo(Game.Text.InkText);

		// Button glyphs
		var buttonGlyphs = Game.Text.ButtonGlyphTexts;
		Unsafe.WriteUnaligned(ref buttonGlyphs[2][0], TextManager.EncodeCharacter('△'));
		Unsafe.WriteUnaligned(ref buttonGlyphs[0][0], TextManager.EncodeCharacter('◯'));
		Unsafe.WriteUnaligned(ref buttonGlyphs[1][0], TextManager.EncodeCharacter('☓'));
		Unsafe.WriteUnaligned(ref buttonGlyphs[3][0], TextManager.EncodeCharacter('⬜'));
		Unsafe.WriteUnaligned(ref buttonGlyphs[4][0], TextManager.EncodeCharacter('⮪')); // L1
		Unsafe.WriteUnaligned(ref buttonGlyphs[5][0], TextManager.EncodeCharacter('⮫')); // R1


		// const char charNameRyu[] = "Ryu";
		// const char charNameNina[] = "Nina";
		// const char charNameGarr[] = "Garr";
		// const char charNameTeepo[] = "Teepo";
		// const char charNameRei[] = "Rei";
		// const char charNameMomo[] = "Momo";
		// const char* defaultCharNames[] = { charNameRyu, charNameNina, charNameGarr, charNameTeepo, charNameRei, charNameMomo, "" };
		// WriteProtectedMemory(0x669FA4, defaultCharNames);
	}
}
