using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text.Unicode;

namespace BoF3Ext.Hooks;


static partial class Gui {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawBattleMessagePanelHook(short x, short y)
		=> Game.Gui.DrawBorderedTextPanel_1(x, y, 280, 20, Game.World.PersistentStateData.WindowColour);

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawBattleCommandTextPanelHook(byte index) {
		ref var rect = ref Game.Gui.BattleCommandTextPanelRects[index];
		var x = rect.X;
		var y = rect.Y;

		if (index == 0 || index == 3 || index == 5)
			x -= 14;
		else if (index == 1 || index == 2)
			x -= 7;

		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.DrawBorderedTextPanel_1(x, y, 54, 18, Game.World.PersistentStateData.WindowColour);

		TextManager.Instance.TryGetBattleCommandText(index, out var text);
		var len = text.Length;

		var advance = GlyphManager.Instance.ScaledGlyphAdvance / 2;

		Game.Render.DrawStringNormal(
			(short)Math.Floor(x + 27 - advance * len),
			(short)(y + 2),
			0,
			(byte)len,
			text.GameString
		);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawBattleEnemyTargetPanelHook(short x, short y, byte enemyIndex) {
		var paletteIdx = Game.World.PersistentStateData.WindowColour;
		var colourValue = Game.Dat.Palettes.Rows[1].Colours[paletteIdx * 32 + 20];

		Vec3b colour = new(
			(byte)(8 * (colourValue & 0x1F)),
			(byte)(8 * ((colourValue >> 5) & 0x1F)),
			(byte)(8 * ((colourValue >> 10) & 0x1F))
		);

		// Shadow
		RendererHelpers.SetDrawEnv(0, 0, 960, 0);

		RendererHelpers.DrawRectWH(x + 4, y + 8, 74, 18, colour, true);

		RendererHelpers.DrawQuad(
			new(x + 6, y + 6), new(x + 76, y + 6),
			new(x + 4, y + 8), new(x + 78, y + 8),
			colour, true
		);

		RendererHelpers.DrawQuad(
			new(x + 4, y + 26), new(x + 78, y + 26),
			new(x + 6, y + 28), new(x + 76, y + 28),
			colour, true
		);

		RendererHelpers.DrawBorderedPanel(x, y, 74, 22, colour, false);

		enemyIndex -= 3;

		ref var win = ref Game.Gui.CurrentUiWindow;
		ref var enemy = ref Game.Battle.EnemyBattleDatas[enemyIndex];

		RendererHelpers.DrawRectWH(x + 6, y + 12, 56, 6, Vec3b.Zero, false);

		if (enemy.IsHpKnown)
			Game.Gui.DrawHpApBar((short)(x + 6), (short)(y + 11), win.StateParamsU8[3], win.StateParamsU8[5], Game.Gui.HpApBarType.Hp);
		else
			Game.Render.DrawNumTiny((short)(x + 36), (short)(y + 13), 0, 0xFFFF);

		TextManager.Instance.TryGetEnemyName(Game.World.CurrentAreaId, enemy.Index, out var text);

		Game.Render.DrawStringSmall((short)(x + 4), (short)(y + 4), 0, (byte)text.Length, text.GameString);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawBattleEnemyPanelHook(short x, short y, byte enemyIndex) {
		Game.Gui.DrawBorderedTextPanel_1(x, y, 74, 22, Game.World.PersistentStateData.WindowColour);
		enemyIndex -= 3;

		ref var win = ref Game.Gui.CurrentUiWindow;
		ref var enemyData = ref Game.Battle.EnemyBattleDatas[enemyIndex];

		RendererHelpers.DrawRectWH(x + 6, y + 12, 56, 6, Vec3b.Zero, false);

		if (enemyData.IsHpKnown)
			Game.Gui.DrawHpApBar((short)(x + 6), (short)(y + 11), win.StateParamsU8[3], win.StateParamsU8[5], Game.Gui.HpApBarType.Hp);
		else
			Game.Render.DrawNumTiny((short)(x + 36), (short)(y + 13), 0, 0xFFFF /* '?' */);

		if (!Game.Battle.ShouldDrawEnemyName(enemyIndex))
			return;

		TextManager.Instance.TryGetEnemyName(Game.World.CurrentAreaId, enemyData.Index, out var text);

		Game.Render.DrawStringSmall((short)(x + 4), (short)(y + 4), 0, (byte)text.Length, text.GameString);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawBattleActionTextPanelHook() {
		ref var win = ref Game.Gui.CurrentUiWindow;
		ref var data = ref Game.Gui.BattleTextPanelDatas[win.StateParamsU8[2]];

		var textLen = MemoryMarshal.CreateReadOnlySpanFromNullTerminated(data.Text.Pointer).Length;
		var x = (short)(win.X + ConfigManager.Instance.HalfRenderWidthDiff);
		var y = win.Y;

		var advance = Math.Round(GlyphManager.Instance.ScaledGlyphAdvance);

		var panelX = data.IsCharacterName ? x - 16 : x;
		var panelW = data.IsCharacterName ? 102 : 70;

		Game.Gui.DrawBorderedTextPanel_1(
			(short)panelX, y, (ushort)panelW, 18,
			Game.World.PersistentStateData.WindowColour
		);

		var textX = (short)(x + 35 - textLen * advance / 2);

		Game.Render.DrawStringNormal(textX, (short)(y + 2), data.PaletteIndex, (byte)textLen, data.Text);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawBattleXpResultPanelHook() {
		Game.Gui.DrawBorderedTextPanel_1(
			20, 40, 280, Game.Gui.CurrentUiWindow.StateParamsU8[1],
			Game.World.PersistentStateData.WindowColour
		);

		var buffer = stackalloc byte[7];
		Span<byte> formatted = new(buffer, 6);
		GameString number = new(buffer);

		short y = 44;

		for (byte i = 0; i < Game.Battle.PartyMemberCount; ++i) {
			ref var character = ref Game.Battle.BattleCharacters[i];

			GameString name = new((byte*)Unsafe.AsPointer(ref character.Name[0]));
			Game.Render.DrawStringNormal(25, y, 0, 5, name);

			ushort textId;

			if (character.Level == 99)
				textId = 65;    // " has reached MAX LVL!"
			else {
				Utf8.TryWrite(formatted, $"{character.Level + 1,2}", out var length);
				buffer[length] = 0;

				Game.Render.DrawStringLarge(85, y, 0, number);

				var advance = GlyphManager.Instance.ScaledGlyphAdvance;
				var x = (short)(20 + 280 - 7 - Math.Ceiling(6 * advance * (16f / 12f)));

				Utf8.TryWrite(formatted, $"{Game.Battle.GetXpToNextLevel(i),6}", out length);
				buffer[length] = 0;

				Game.Render.DrawStringLarge(x, y, 0, number);

				textId = 21;    // "EXP to next level:"
			}

			var text = Game.Text.GetText(textId);
			Game.Render.DrawStringNormal(133, y, 0, 255, text);

			y += 16;
		}
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawBattleInventoryTabsHook(short x, short y, sbyte selectedTabIndex) {
		var textMgr = TextManager.Instance;
		var paletteIdx = Game.World.PersistentStateData.WindowColour;
		var advance = GlyphManager.Instance.ScaledGlyphAdvance / 2;

		Game.Gui.DrawWindowBackground(x, y, 45, 20, 0, paletteIdx);

		var textColour = selectedTabIndex == -1 || selectedTabIndex == 0 ? 0u : 7u;

		textMgr.TryGetMenuTabText(0, out var useText);

		Game.Render.DrawStringNormal(
			(short)(x + 22 - advance * useText.Length),
			(short)(y + 3), textColour, 16, useText.GameString
		);

		var parts = selectedTabIndex == 0
			? Game.Gui.SelectedBattleInventoryTabUiParts
			: Game.Gui.BattleInventoryTabUiParts;

		Game.Gui.DrawUiParts(x, y, (nint)Unsafe.AsPointer(ref parts[0]), 1);

		Game.Gui.DrawWindowBackground((short)(x + 48), y, 45, 20, 0, paletteIdx);

		textColour = selectedTabIndex == -1 || selectedTabIndex == 1 ? 0u : 7u;

		textMgr.TryGetMenuTabText(3, out var equipText);

		Game.Render.DrawStringNormal(
			(short)(x + 70 - advance * equipText.Length),
			(short)(y + 3), textColour, 16, equipText.GameString
		);

		parts = selectedTabIndex == 1
			? Game.Gui.SelectedBattleInventoryTabUiParts
			: Game.Gui.BattleInventoryTabUiParts;

		Game.Gui.DrawUiParts((short)(x + 48), y, (nint)Unsafe.AsPointer(ref parts[0]), 1);
	}


	static unsafe void EnableBattleGuiHooks() {
		Game.Gui.HookDrawBattleMessagePanel(&DrawBattleMessagePanelHook);
		Game.Gui.HookDrawBattleCommandTextPanel(&DrawBattleCommandTextPanelHook);
		Game.Gui.HookDrawBattleEnemyTargetPanel(&DrawBattleEnemyTargetPanelHook);
		Game.Gui.HookDrawBattleEnemyPanel(&DrawBattleEnemyPanelHook);
		Game.Gui.HookDrawBattleActionTextPanel(&DrawBattleActionTextPanelHook);
		Game.Gui.HookDrawBattleXpResultPanel(&DrawBattleXpResultPanelHook);
		Game.Gui.HookDrawBattleInventoryTabs(&DrawBattleInventoryTabsHook);
	}
}