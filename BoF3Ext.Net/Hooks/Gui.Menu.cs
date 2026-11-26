using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text.Unicode;

namespace BoF3Ext.Hooks;


static partial class Gui {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawStatusWindowHook(short x, short y, byte characterId) {
		Game.Gui.DrawWindowBackground(
			(short)(x + 5), (short)(y + 3), 164, 45, 0, Game.World.PersistentStateData.WindowColour
		);

		ref var character = ref Game.World.PersistentStateData.Characters[characterId];
		var textMgr = TextManager.Instance;

		var buffer = stackalloc byte[4];
		Span<byte> formatted = new(buffer, 3);
		GameString number = new(buffer);

		textMgr.TryGetStatName(Game.World.StatId.Power, out var powerText);
		Game.Render.DrawStringNumFont((short)(x + 11), (short)(y + 10), 0, powerText.GameString);

		Utf8.TryWrite(formatted, $"{character.BattleStats.Power,3}", out var length);
		buffer[length] = 0;
		Game.Render.DrawStringNumFont((short)(x + 56), (short)(y + 10), 0, number);

		textMgr.TryGetStatName(Game.World.StatId.Defence, out var defenceText);
		Game.Render.DrawStringNumFont((short)(x + 11), (short)(y + 23), 0, defenceText.GameString);

		Utf8.TryWrite(formatted, $"{character.BattleStats.Defence,3}", out length);
		buffer[length] = 0;
		Game.Render.DrawStringNumFont((short)(x + 56), (short)(y + 23), 0, number);

		textMgr.TryGetStatName(Game.World.StatId.Intellect, out var intellectText);
		Game.Render.DrawStringNumFont((short)(x + 89), (short)(y + 10), 0, intellectText.GameString);

		Utf8.TryWrite(formatted, $"{character.BattleStats.Intellect,3}", out length);
		buffer[length] = 0;
		Game.Render.DrawStringNumFont((short)(x + 146), (short)(y + 10), 0, number);

		textMgr.TryGetStatName(Game.World.StatId.Agility, out var agilityText);
		Game.Render.DrawStringNumFont((short)(x + 89), (short)(y + 23), 0, agilityText.GameString);

		Utf8.TryWrite(formatted, $"{character.BattleStats.Agility,3}", out length);
		buffer[length] = 0;
		Game.Render.DrawStringNumFont((short)(x + 146), (short)(y + 23), 0, number);

		if (character.MasterId != 255) {
			var master = Game.Text.GetText((ushort)(character.MasterId + 273));
			Game.Text.LoadTextParameter(0, 8, master);

			var text = Game.Text.GetText(52);
			Game.Render.DrawStringNormal((short)(x + 11), (short)(y + 34), 0, 255, text);
		}

		var parts = Game.Gui.StatusWindowLeftUiParts;
		Game.Gui.DrawUiParts(x, y, (nint)Unsafe.AsPointer(ref parts[0]), 0);

		parts = Game.Gui.StatusWindowMiddleUiParts;

		for (var i = 0; i < 18; ++i)
			Game.Gui.DrawUiParts((short)(x + 8 * i), y, (nint)Unsafe.AsPointer(ref parts[0]), 0);

		parts = Game.Gui.StatusWindowRightUiParts;
		Game.Gui.DrawUiParts((short)(x + 16), y, (nint)Unsafe.AsPointer(ref parts[0]), 0);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawZennyPanelHook(short x, short y, int a3, int amount) {
		var diff = ConfigManager.Instance.RenderWidthDiff;
		var advance = (int)Math.Ceiling(GlyphManager.Instance.ScaledGlyphAdvance);
		var largeAdvance = (int)Math.Ceiling(GlyphManager.Instance.ScaledGlyphAdvance * (16.0 / 12.0));

		Game.Gui.DrawWindowBackground(
			(short)(x + 3 + diff),
			(short)(y + 3),
			101,
			16,
			0,
			Game.World.PersistentStateData.WindowColour
		);

		var buffer = stackalloc byte[8];
		Span<byte> formatted = new(buffer, 7);

		Utf8.TryWrite(formatted, $"{amount,7}", out var length);
		buffer[length] = 0;

		GameString number = new(buffer);

		Game.Render.DrawStringLarge((short)(x + 101 - 5 - advance * 2 - 7 * largeAdvance + diff),(short)(y + 4), 0, number);

		var currency = TextManager.EncodeCharacter('ƶ');
		GameString currencyText = new((byte*)&currency);

		Game.Render.DrawStringNormal((short)(x + 101 - 5 - advance + diff), (short)(y + 4), 0, 1, currencyText);

		var parts = Game.Gui.ZennyPanelLeftUiParts;

		Game.Gui.DrawUiParts((short)(x + diff), y, (nint)Unsafe.AsPointer(ref parts[0]), 0);

		parts = Game.Gui.ZennyPanelMiddleUiParts;

		for (var i = 0; i < 11; ++i)
			Game.Gui.DrawUiParts((short)(x + 8 * i + diff), y, (nint)Unsafe.AsPointer(ref parts[0]), 0);

		parts = Game.Gui.ZennyPanelRightUiParts;

		Game.Gui.DrawUiParts((short)(x + diff), y, (nint)Unsafe.AsPointer(ref parts[0]), 0);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawMenuTabsHook(short x, short y, byte index, byte selectedTabIndex) {
		ref var group = ref Game.Gui.MenuTabGroups[index];

		var advance = GlyphManager.Instance.ScaledGlyphAdvance;
		var paletteIdx = Game.World.PersistentStateData.WindowColour;

		for (int i = 0; i < group.Count; i++) {
			Game.Gui.DrawWindowBackground((short)(x + 48 * i), y, 45, 20, 0, paletteIdx);

			var id = group.Tabs[i];

			TextManager.Instance.TryGetMenuTabText(id, out var text);
			var len = text.Length;

			var offset = (int)Math.Round(advance / 2.0f * len);
			var textX = x + 22 - offset + i * 48;

			var textColour = 0U;

			if ((selectedTabIndex < group.Count && i != selectedTabIndex)
				|| (id == 18 && (Game.Terrain.AreaFlags & 1) == 0)
				|| (id == 8
					&& !Game.Item.HaveKeyItem(Game.Item.KeyItemId.FountainPen)
					&& Game.Item.GetItemCount(Game.Item.ItemId.SkillInk, Game.Item.InventoryType.Inventory) == 0))
				textColour = 7;

			Game.Render.DrawStringNormal((short)textX, (short)(y + 3), textColour, (byte)len, text.GameString);

			var parts = (i == selectedTabIndex) ? Game.Gui.SelectedMenuTabUiParts : Game.Gui.MenuTabUiParts;

			Game.Gui.DrawUiParts((short)(x + 48 * i), y, (nint)Unsafe.AsPointer(ref parts[0]), 0);
		}
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawEquipmentWindowHook(
		byte characterId, short x, short y, nint equipment, bool noCompare, nint win
	) {
		Game.Gui.DrawWindowBackground(
			(short)(x + 4), (short)(y + 4), 120, 163, 0, Game.World.PersistentStateData.WindowColour
		);

		ref var character = ref Game.World.PersistentStateData.Characters[characterId];
		var textMgr = TextManager.Instance;

		GameString name = new((byte*)Unsafe.AsPointer(ref character.Name[0]));
		Game.Render.DrawStringNormal((short)(x + 32), (short)(y + 7), 0, 5, name);

		textMgr.TryGetStatName(Game.World.StatId.Power, out var powerText);
		Game.Render.DrawStringNormal((short)(x + 5), (short)(y + 26), 0, (byte)powerText.Length, powerText.GameString);

		textMgr.TryGetStatName(Game.World.StatId.Defence, out var defenceText);
		Game.Render.DrawStringNormal((short)(x + 5), (short)(y + 39), 0, (byte)defenceText.Length, defenceText.GameString);

		textMgr.TryGetStatName(Game.World.StatId.Intellect, out var intellectText);
		Game.Render.DrawStringNormal(
			(short)(x + 5),
			(short)(y + 52),
			0,
			(byte)intellectText.Length,
			intellectText.GameString
		);

		textMgr.TryGetStatName(Game.World.StatId.Agility, out var agilityText);
		Game.Render.DrawStringNormal((short)(x + 5), (short)(y + 65), 0, (byte)agilityText.Length, agilityText.GameString);

		var buffer = stackalloc byte[6];
		Span<byte> formatted = new(buffer, 5);
		GameString number = new(buffer);

		var advance = Math.Ceiling(GlyphManager.Instance.ScaledGlyphAdvance);
		var statXPos = x + 120 - advance * 3 - 5;

		if (!noCompare)
			statXPos -= advance * 4;

		var statX = (short)statXPos;

		Utf8.TryWrite(formatted, $"{character.BattleStats.Power}", out var length);
		buffer[length] = 0;
		Game.Render.DrawStringNormal(statX, (short)(y + 26), 0, 3, number);

		Utf8.TryWrite(formatted, $"{character.BattleStats.Defence}", out length);
		buffer[length] = 0;
		Game.Render.DrawStringNormal(statX, (short)(y + 39), 0, 3, number);

		Utf8.TryWrite(formatted, $"{character.BattleStats.Intellect}", out length);
		buffer[length] = 0;
		Game.Render.DrawStringNormal(statX, (short)(y + 52), 0, 3, number);

		Utf8.TryWrite(formatted, $"{character.BattleStats.Agility}", out length);
		buffer[length] = 0;
		Game.Render.DrawStringNormal(statX, (short)(y + 65), 0, 3, number);

		if (!noCompare) {
			Span<byte> colours = stackalloc byte[4];
			Span<ushort> newStats = stackalloc ushort[4];

			var arrow = TextManager.EncodeCharacter('»');
			GameString arrowText = new((byte*)&arrow);

			Game.Item.GetNewStats(characterId, equipment, ref colours[0], ref newStats[0]);

			for (var i = 0; i < 4; ++i) {
				var textY = (short)(y + 26 + 13 * i);

				Game.Render.DrawStringNormal((short)(statXPos + advance * 3), textY, 0, 1, arrowText);

				var colour = colours[i];
				var newStat = newStats[i];

				Utf8.TryWrite(formatted, $"{newStat}", out length);
				buffer[length] = 0;

				Game.Render.DrawStringNormal((short)(statXPos + advance * 4), textY, colour, 3, number);
			}
		}

		ReadOnlySpan<GameString> equipNames = [
			Game.Text.GetItemName(1, character.WeaponId),
			Game.Text.GetItemName(2, character.ArmorId1),
			Game.Text.GetItemName(2, character.ArmorId2),
			Game.Text.GetItemName(2, character.ArmorId3),
			Game.Text.GetItemName(3, character.OptionId1),
			Game.Text.GetItemName(3, character.OptionId2)
		];

		for (var i = 0; i < 6; ++i) {
			var textY = y + i + 12 * i;

			Game.Gui.DrawItemIcon((short)(x + 6), (short)(textY + 87), Game.Gui.EquipWindowItemIconIds[i], false);

			var len = Game.Text.GetStringLength(equipNames[i]);
			Game.Render.DrawStringNormal((short)(x + 17), (short)(textY + 85), 0, len, equipNames[i]);
		}

		var parts = Game.Gui.EquipmentWindowUiParts;
		Game.Gui.DrawUiParts(x, y, (nint)Unsafe.AsPointer(ref parts[0]), 1);

		for (var i = 0; i < 7; ++i) {
			var frameY = (short)(y + 8 * i + 24);

			Game.Gui.DrawUiPart(x, frameY, 4, 1);
			Game.Gui.DrawUiPart((short)(x + 120), frameY, 8, 1);
		}

		for (var i = 0; i < 9; ++i) {
			var frameY = (short)(y + 8 * i + 88);

			Game.Gui.DrawUiPart(x, frameY, 4, 1);
			Game.Gui.DrawUiPart((short)(x + 120), frameY, 8, 1);
		}

		for (var i = 0; i < 14; ++i) {
			var frameX = (short)(x + 8 * i + 8);

			Game.Gui.DrawUiPart(frameX, (short)(y + 80), 14, 1);
			Game.Gui.DrawUiPart(frameX, (short)(y + 160), 17, 1);
		}

		Game.Gui.DrawUiPart(x, (short)(y + 80), 13, 1);
		Game.Gui.DrawUiPart((short)(x + 120), (short)(y + 80), 15, 1);
		Game.Gui.DrawUiPart(x, (short)(y + 160), 16, 1);
		Game.Gui.DrawUiPart((short)(x + 120), (short)(y + 160), 18, 1);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawSkillInfoHook(
		short x, short y, int paletteIndex, byte iconId, GameString text, byte cost, bool greyed
	) {
		var skills = Game.Skill.SkillDatas;
		var start = (nuint)Unsafe.AsPointer(ref skills[0]);
		var end = (nuint)Unsafe.AsPointer(ref skills[^1]);
		var address = (nuint)text.Pointer;

		if (address >= start && address <= end) {
			var id = (uint)((address - start) / (nuint)Unsafe.SizeOf<Game.Skill.SkillData>());

			if (TextManager.Instance.TryGetSkillName(id, out var name))
				text = name.GameString;
		}

		Game.Gui.OriginalDrawSkillInfo(x, y, paletteIndex, iconId, text, cost, greyed);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawSavesHook(short x, short y, uint colour) {
		Game.Gui.OriginalDrawSaves(x, y, colour);

		// Draw page indicators as it's not obvious that there are more than 3 saves without them
		var offset = ConfigManager.Instance.HalfRenderWidthDiff;
		var indicatorX = 156 + offset;

		var tpage = Game.Render.GetTPage(0, 1, 960, 0);
		var palette = Game.Render.CreatePaletteIndex(192, 480);

		if (Game.Gui.FirstVisibleSaveIndex > 0) {
			RendererHelpers.DrawRectWHTexture(
				indicatorX, 44, 8, 8, 208, 248, (ushort)(tpage | 0x2000), palette, new(128, 128, 128), true
			);
		}

		if (Game.Gui.FirstVisibleSaveIndex <= 12)
			RendererHelpers.DrawRectWHTexture(indicatorX, 221, 8, 8, 208, 248, tpage, palette, new(128, 128, 128), true);
	}


	static unsafe void EnableMenuGuiHooks() {
		EnableConfigMenuGuiHooks();

		Game.Gui.HookDrawStatusWindow(&DrawStatusWindowHook);
		Game.Gui.HookDrawZennyPanel(&DrawZennyPanelHook);
		Game.Gui.HookDrawMenuTabs(&DrawMenuTabsHook);
		Game.Gui.HookDrawEquipmentWindow(&DrawEquipmentWindowHook);
		Game.Gui.HookDrawSkillInfo(&DrawSkillInfoHook);
		Game.Gui.HookDrawSaves(&DrawSavesHook);
	}
}