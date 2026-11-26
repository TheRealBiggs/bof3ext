using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Hooks;


static class Text {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void LoadTextParameter_BattleActionHook(byte index, bool isCharacterName) {
		if (!TextManager.Instance.TryGetBattleActionText(index, out var text)) {
			Game.Text.OriginalLoadTextParameter_BattleAction(index, isCharacterName);

			return;
		}

		Span<byte> buffer = Game.Text.TextParameterBuffers[15];
		ReadOnlySpan<byte> source = new(text.GameString.Pointer, text.ByteCount + 1);

		source.CopyTo(buffer);

		Game.Gui.BattleTextPanelDatas[0].IsCharacterName = isCharacterName;
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void LoadTextParameter_EnemyNameHook(byte index) {
		var id = Game.Battle.EnemyBattleDatas[index - 3].EnemyId;

		TextManager.Instance.TryGetEnemyName(Game.World.CurrentAreaId, id, out var name);

		Span<byte> buffer = Game.Text.TextParameterBuffers[0];
		Span<byte> nameSpan = new(name.GameString.Pointer, name.ByteCount + 1);

		nameSpan.CopyTo(buffer);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe GameString GetTextHook(ushort index) {
		var textMgr = TextManager.Instance;

		if (textMgr.TryGetGeneralText((uint)Hooks.Dat.LastLoadedDatFileId, index, out var text))
			return text.GameString;

		Console.WriteLine($"Untranslated general text: file '{Hooks.Dat.LastLoadedDatFileId}', index {index & 0x3FFF}");

		return Game.Text.OriginalGetText(index);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void LoadDialogueHook(ushort index) {
		var textMgr = TextManager.Instance;
		 
		var areaId = Game.World.CurrentAreaId;

		if (textMgr.TryGetDialogue(areaId, index, out var text)) {
			Game.Text.DialoguePage = text.GameString;
			Game.Text.DialogueCursor = text.GameString;

			Game.Text.InitialiseDialogue();

			Game.Text.DialogueIndex = index;
		} else
			Game.Text.OriginalLoadDialogue(index);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void LoadItemNameHook(byte index, byte type) {
		var textMgr = TextManager.Instance;

		TextManager.TextEntry? name = null;

		switch (type) {
			case 0: {
				textMgr.TryGetItemName(index, out name);
				break;
			}

			case 1: {
				textMgr.TryGetWeaponName(index, out name);
				break;
			}

			case 2: {
				textMgr.TryGetArmourName(index, out name);
				break;
			}

			case 3: {
				textMgr.TryGetAccessoryName(index, out name);
				break;
			}
		}

		if (name != null) {
			Span<byte> buffer = Game.Text.TextParameterBuffers[0];
			Span<byte> nameSpan = new(name.GameString.Pointer, name.ByteCount + 1);

			nameSpan.CopyTo(buffer);
		}
		else
			Game.Text.OriginalLoadItemName(index, type);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe byte GetStringLengthHook(GameString text) {
		if (Unsafe.ReadUnaligned<uint>(text.Pointer) == 0x600DBEEFu)
			text = new((byte*)Unsafe.ReadUnaligned<nuint>(text.Pointer + 4));

		return Game.Text.OriginalGetStringLength(text);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe GameString GetItemNameHook(byte type, byte index) {
		var textMgr = TextManager.Instance;

		switch (type) {
			case 0: {
				if (textMgr.TryGetItemName(index, out var name))
					return name.GameString;

				break;
			}

			case 1: {
				if (textMgr.TryGetWeaponName(index, out var name))
					return name.GameString;

				break;
			}

			case 2: {
				if (textMgr.TryGetArmourName(index, out var name))
					return name.GameString;

				break;
			}

			case 3: {
				if (textMgr.TryGetAccessoryName(index, out var name))
					return name.GameString;

				break;
			}
		}

		// TODO: Add key item names

		return Game.Text.OriginalGetItemName(type, index);
	}


	public static unsafe void Enable() {
		Game.Text.HookLoadTextParameter_BattleAction(&LoadTextParameter_BattleActionHook);
		Game.Text.HookLoadTextParameter_EnemyName(&LoadTextParameter_EnemyNameHook);
		Game.Text.HookGetText(&GetTextHook);
		Game.Text.HookLoadDialogue(&LoadDialogueHook);
		Game.Text.HookLoadItemName(&LoadItemNameHook);
		Game.Text.HookGetStringLength(&GetStringLengthHook);
		Game.Text.HookGetItemName(&GetItemNameHook);
	}
}
