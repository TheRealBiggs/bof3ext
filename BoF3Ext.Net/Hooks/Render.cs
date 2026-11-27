using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Hooks;


static class Render {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawCurrentActorSpriteHook() {
		ref var actor = ref Game.World.CurrentActor;

		if ((nint)Unsafe.AsPointer(ref actor) != Hooks.Overworld.LocationPanelActor
			|| actor.Byte_7 != 1
			|| actor.State <= 1) {
			Game.Render.OriginalDrawCurrentActorSprite();

			return;
		}

		var areaId = Game.World.CurrentAreaId;
		var locationId = actor.Dword_18;
		var key = ((uint)areaId << 8) | locationId;

		if (!TextManager.Instance.TryGetLocationName(key, out var name)) {
			Console.WriteLine($"{areaId}, {locationId}");
			Game.Render.OriginalDrawCurrentActorSprite();

			return;
		}

		var scale = actor.ScaleMode != 0 ? actor.ScaleX / 65536f : 1f;
		var advance = GlyphManager.Instance.ScaledGlyphAdvance * 10f / 12f;

		var textureId = Renderer.Instance.LocationNameBgTextureId;
		var width = Math.Max(40f, name.Length * advance + 8f * 2f);
		var stretch = (width - 40f) * 0.5f;

		ReadOnlySpan<float> edges = [
			0f,
			64f / 320f,
			144f / 320f,
			176f / 320f,
			256f / 320f,
			1f
		];

		ReadOnlySpan<float> widths = [
			16f, stretch, 8f, stretch, 16f
		];

		var offset = -width * 0.5f;

		for (var i = 0; i < 5; i++) {
			// Round shared boundaries so adjacent sections meet exactly.
			var left = (int)Math.Round(actor.fX + offset * scale);

			offset += widths[i];

			var right = (int)Math.Round(actor.fX + offset * scale);

			if (right <= left)
				continue;

			RendererHelpers.DrawTextureFrame(
				textureId,
				new(edges[i], 0f, edges[i + 1], 1f),
				left,
				(int)Math.Round(actor.fY),
				right - left,
				16
			);
		}

		var x = actor.fX - name.Length * advance * scale * 0.5f - 2f;
		var y = actor.fY + 2;
		var text = name.GameString.Pointer;

		for (var i = 0; i < name.Length; i++) {
			ushort charCode = *text++;

			if ((charCode & 0x80) != 0)
				charCode = (ushort)((charCode << 8) | *text++);

			if (charCode == ' ')
				continue;

			ref var prim = ref RendererHelpers.GetGpuPrim<Game.Render.GpuPrim_TextGlyph>();
			prim.CharCode = charCode;

			Game.Render.DrawGlyph(
				(short)Math.Round(x + i * advance * scale),
				(short)y,
				(short)(10 * scale),
				10,
				0,
				0,
				0x7800
			);
		}
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void LoadImageHook(nint rect, nint data) {
		ref var _rect = ref Unsafe.AsRef<Game.Render.PSX_RECT>((void*)rect);
		Span<byte> _data = new((void*)data, _rect.W * _rect.H * sizeof(ushort));

		Renderer.Instance.LoadTexturePageData(ref _rect, ref _data);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe nint DrawStringNormalHook(short x, short y, uint paletteIndex, byte len, GameString text) {
		var address = (nuint)text.Pointer;

		// Translate strings passed directly into offset 0 data.
		if (address > 0x803580 && address <= 0x803580 + 0xFFFF) {
			var offsets = (ushort*)0x803580;
			var count = offsets[0] / 2;

			for (var i = 0; i < count; i++) {
				if (0x803580U + offsets[i] != address)
					continue;

				if (TextManager.Instance.TryGetDialogue(Game.World.CurrentAreaId, (uint)i, out var replacement))
					text = replacement.GameString;

				break;
			}
		}

		y++;

		return DrawString(
			ref x, ref y, x,
			(byte)paletteIndex, len, text,
			12, 12
		);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static nint DrawStringSmallHook(short x, short y, byte paletteIndex, byte len, GameString text) {
		y -= 2;

		return DrawString(
			ref x, ref y, x,
			paletteIndex, len, text,
			10, 9
		);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawStringLargeHook(short x, short y, byte paletteIndex, GameString text) {
		y--;

		DrawString(
			ref x, ref y, x,
			paletteIndex, Int32.MaxValue, text,
			16, 12
		);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawStringNumFontHook(short x, short y, uint paletteIndex, GameString text) {
		Game.Render.DrawStringSmall(
			x, (short)(y + 2), (byte)paletteIndex, 0xFF, text
		);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void DrawNumTinyHook(short x, short y, byte paletteIndex, ushort value) {
		Span<char> digits = stackalloc char[5];
		var digitCount = 0;

		if (value == 0xFFFF) {
			digits[0] = '?';
			digitCount = 1;
		} else {
			value.TryFormat(digits, out digitCount);
		}

		var length = value == 0xFFFF ? 1 : Math.Max(4, digitCount);
		var buffer = stackalloc byte[length + 1];
		var padding = length - digitCount;

		new Span<byte>(buffer, padding).Fill((byte)' ');

		for (var i = 0; i < digitCount; i++)
			buffer[padding + i] = (byte)digits[i];

		buffer[length] = 0;

		GameString text = new(buffer);
		y -= 2;

		DrawString(
			ref x, ref y, x,
			paletteIndex, length, text,
			8, 9
		);
	}


	static unsafe nint DrawString(
		ref short x, ref short y, short startX,
		byte paletteIdx, int len, GameString text,
		short glyphSize, int lineHeight
	) {
		var p = text.Pointer;

		if (Unsafe.ReadUnaligned<uint>(p) == 0x600DBEEFu)
			p = (byte*)Unsafe.ReadUnaligned<nuint>(p + 4);

		var previousPaletteIdx = (byte)0;
		var advance = (int)Math.Round(
			GlyphManager.Instance.ScaledGlyphAdvance * glyphSize / 12.0
		);

		for (; *p != 0 && len != 0; --len) {
			ushort charCode = *p++;

			switch (charCode) {
				case 1:
				case '\n': {
					x = startX;
					y = (short)(y + lineHeight);

					continue;
				}

				case 5: {
					previousPaletteIdx = paletteIdx;
					paletteIdx = *p++;

					continue;
				}

				case 6: {
					paletteIdx = previousPaletteIdx;

					continue;
				}

				case 7: {
					ref var buffer = ref Game.Text.TextParameterBuffers[*p++];
					GameString parameter = new((byte*)Unsafe.AsPointer(ref buffer[0]));

					DrawString(
						ref x, ref y, startX,
						paletteIdx, 32, parameter,
						glyphSize, lineHeight
					);

					continue;
				}
			}

			if ((charCode & 0x80) != 0)
				charCode = (ushort)(((charCode & 0x7F) << 8) | *p++);

			if (charCode != ' ') {
				ref var prim = ref RendererHelpers.GetGpuPrim<Game.Render.GpuPrim_TextGlyph>();
				prim.CharCode = charCode;

				Game.Render.DrawGlyph(
					x, y, glyphSize, glyphSize, 0, 0,
					(ushort)((paletteIdx & 0xF) | 0x7800)
				);
			}

			x = (short)(x + advance);
		}

		// Match the original functions' return convention.
		return (nint)(p + 1);
	}


	public static unsafe void Enable() {
		Game.Render.HookDrawCurrentActorSprite(&DrawCurrentActorSpriteHook);
		Game.Render.HookLoadImage(&LoadImageHook);
		Game.Render.HookLoadImage2(&LoadImageHook);
		Game.Render.HookDrawStringNormal(&DrawStringNormalHook);
		Game.Render.HookDrawStringSmall(&DrawStringSmallHook);
		Game.Render.HookDrawStringLarge(&DrawStringLargeHook);
		Game.Render.HookDrawStringNumFont(&DrawStringNumFontHook);
		Game.Render.HookDrawNumTiny(&DrawNumTinyHook);
	}
}