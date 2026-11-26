using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using unsafe DrawCenteredConfigDescriptionBackgroundSig =
	delegate* unmanaged[Cdecl]<short, short, ushort, ushort, byte, void>;
using unsafe DrawDemoTextSig = delegate* unmanaged[Cdecl]<short, short, uint, byte, BoF3Ext.GameString, nint>;

namespace BoF3Ext.Patches;


static class Widescreen {
	struct DrawRanges {
		public float TerrainMin1, TerrainMax1;
		public float TerrainMin2, TerrainMax2;
		public float ObjectMin1, ObjectMax1;
		public float ObjectMin2, ObjectMax2;
		public float SpriteMin1, SpriteMax1;
		public short SpriteMin2, SpriteMax2;
		public float UnknownMin1, UnknownMax1;
		public float UnknownMax2;
	}


	static unsafe readonly DrawRanges* drawRanges =
		(DrawRanges*)NativeMemory.Alloc((nuint)Unsafe.SizeOf<DrawRanges>());


	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawCenteredConfigDescriptionBackground(short x, short y, ushort width, ushort height, byte paletteIndex) {
		x += (short)ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.DrawBorderedTextPanel_1(x, y, width, height, paletteIndex);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe nint DrawDemoText(short x, short y, uint paletteIndex, byte len, GameString text) {
		var address = (nuint)text.Pointer;

		if (address > 0x803580 && address <= 0x803580 + 0xFFFF) {
			var offsets = (ushort*)0x803580;
			var count = offsets[0] / 2;

			for (var i = 0; i < count; i++) {
				if (0x803580U + offsets[i] != address)
					continue;

				var areaId = Game.World.CurrentAreaId;

				if (TextManager.Instance.TryGetDialogue(areaId, (uint)i, out var replacement))
					text = replacement.GameString;

				break;
			}
		}

		var columns = 0;
		var maxColumns = 0;

		for (var p = text.Pointer; *p != 0;) {
			var c = *p++;

			if (c == 1 || c == '\n') {
				if (columns > maxColumns)
					maxColumns = columns;

				columns = 0;

				continue;
			}

			if ((c & 0x80) != 0 && *p != 0)
				p++;

			columns++;
		}

		if (columns > maxColumns)
			maxColumns = columns;

		var advance = (int)Math.Round(GlyphManager.Instance.ScaledGlyphAdvance);
		var width = ConfigManager.Instance.ScaledRenderWidth;

		x = (short)Math.Round((width - maxColumns * advance) * 0.5f);

		return Game.Render.DrawStringNormal(x, y, paletteIndex, len, text);
	}


	public static unsafe void Apply() {
		static void PatchConfigDescriptionBackground(nuint address) {
			ProtectedMemory.WriteCall(
				address,
				(DrawCenteredConfigDescriptionBackgroundSig)(&DrawCenteredConfigDescriptionBackground)
			);
		}


		// Config menu
		PatchConfigDescriptionBackground(0x460D0D);
		PatchConfigDescriptionBackground(0x460D9B);
		PatchConfigDescriptionBackground(0x460E37);
		PatchConfigDescriptionBackground(0x46108E);
		PatchConfigDescriptionBackground(0x4613CD);
		PatchConfigDescriptionBackground(0x46164A);
		PatchConfigDescriptionBackground(0x46169A);

		// Demo text
		ReadOnlySpan<nuint> DEMO_TEXT_DRAWSTRINGS = [
			0x56B63A, 0x56B68E, 0x56B6EF, 0x56B781, 0x56B7C8, 0x56B819, 0x56B9AB, 0x56B9E4,
			0x56BA3C, 0x56BB0C, 0x56BB75, 0x56BBC8, 0x56BD95, 0x56BF9B, 0x56BFE5, 0x56C03B
		];

		foreach (var address in DEMO_TEXT_DRAWSTRINGS)
			ProtectedMemory.WriteCall(address, (DrawDemoTextSig)(&DrawDemoText));

		// Disable terrain column trimming
		ProtectedMemory.Nop(0x56ECD3, 2);

		// Set width of all start menu items to 254
		// Needed for fitting English text in new textures
		ProtectedMemory.Write(0x5888E8, (byte)254);
		ProtectedMemory.Write(0x5888ED, (byte)254);
		ProtectedMemory.Write(0x5888F2, (byte)254);

		Update();
	}

	public static unsafe void Update() {
		var cfgMgr = ConfigManager.Instance;

		var renderWidth = (float)cfgMgr.ScaledRenderWidth;

		PatchDrawRanges(cfgMgr.RenderWidthDiff);

		// Widescreen fix for sprite culling
		ProtectedMemory.Write(0x57B9EB, (uint)renderWidth);

		var offset = cfgMgr.HalfRenderWidthDiff;

		// Widescreen fix for start menu items
		ProtectedMemory.Write(0x58893B, (uint)(renderWidth / 2));
		ProtectedMemory.Write(0x588A19, (uint)(renderWidth / 2));

		// Widescreen fix for save menu
		ProtectedMemory.Write(0x5881AC, (byte)(20 + offset));   // Top panel X
		ProtectedMemory.Write(0x5881C7, (byte)(28 + offset));   // Top panel text
		ProtectedMemory.Write(0x58820D, (byte)(20 + offset));
		ProtectedMemory.Write(0x588228, (byte)(28 + offset));
		ProtectedMemory.Write(0x58835C, (byte)(20 + offset));
		ProtectedMemory.Write(0x588377, (byte)(28 + offset));
		ProtectedMemory.Write(0x5883DD, (byte)(20 + offset));
		ProtectedMemory.Write(0x5884BE, (byte)(20 + offset));
		ProtectedMemory.Write(0x588515, (byte)(28 + offset));
		ProtectedMemory.Write(0x58857C, (byte)(20 + offset));
		ProtectedMemory.Write(0x5885A2, (byte)(28 + offset));
		ProtectedMemory.Write(0x5887C2, (byte)(20 + offset));
		ProtectedMemory.Write(0x5887E8, (byte)(28 + offset));

		// Widescreen fix for black fade
		ProtectedMemory.Write(0x4957CF, renderWidth);

		// Widescreen fix for menu background
		// Repeat background pattern to fit extended screen width.
		// Each column is 32px wide and alternates between 2 patterns, meaning 64px per repeat.
		ProtectedMemory.Write(0x575732, (uint)Math.Ceiling(renderWidth / 32 / 2));

		// Widescreen fix for overworld popups ("!", location name, etc.)
		ProtectedMemory.Write(0x4112A9, (ushort)(160 + offset));

		// Widescreen fix for sky
		ProtectedMemory.Write(0x571C85, renderWidth);

		// Widescreen fix for pause menu
		ProtectedMemory.Write(0x589F9C, (short)(116 + offset)); // Top text panel X
		ProtectedMemory.Write(0x589EB7, (short)(98 + offset));  // Character panels X
	}


	static unsafe void PatchDrawRanges(float diff) {
		var r = drawRanges;

		*r = new() {
			TerrainMin1 = -50f - diff,
			TerrainMax1 = 370f + diff,
			TerrainMin2 = -200f - diff,
			TerrainMax2 = 520f + diff,
			ObjectMin1 = -100f - diff,
			ObjectMax1 = 420f + diff,
			ObjectMin2 = -80f - diff,
			ObjectMax2 = 400f + diff,
			SpriteMin1 = -60f - diff,
			SpriteMax1 = 380f + diff,
			SpriteMin2 = (short)(384 + diff),
			SpriteMax2 = (short)(-64 - diff),
			UnknownMin1 = -20f - diff,
			UnknownMax1 = 340f + diff,
			UnknownMax2 = 320f + diff
		};

		ProtectedMemory.Write(0x5109BB, (nuint)(&r->TerrainMin1));
		ProtectedMemory.Write(0x5109D2, (nuint)(&r->TerrainMax1));
		ProtectedMemory.Write(0x51097E, (nuint)(&r->TerrainMin2));
		ProtectedMemory.Write(0x510991, (nuint)(&r->TerrainMax2));
		ProtectedMemory.Write(0x56EDFA, (nuint)(&r->TerrainMin1));
		ProtectedMemory.Write(0x56EE14, (nuint)(&r->TerrainMax1));

		ProtectedMemory.Write(0x4CEC09, (nuint)(&r->ObjectMin1));
		ProtectedMemory.Write(0x4CEC1F, (nuint)(&r->ObjectMax1));
		ProtectedMemory.Write(0x5700D1, (nuint)(&r->ObjectMin1));
		ProtectedMemory.Write(0x5700E7, (nuint)(&r->ObjectMax1));
		ProtectedMemory.Write(0x570319, (nuint)(&r->ObjectMin2));
		ProtectedMemory.Write(0x570333, (nuint)(&r->ObjectMax2));

		ProtectedMemory.Write(0x4CF319, (nuint)(&r->SpriteMin1));
		ProtectedMemory.Write(0x4CF335, (nuint)(&r->SpriteMax1));
		ProtectedMemory.Write(0x4FF6A3, (nuint)(&r->SpriteMin1));
		ProtectedMemory.Write(0x4FF6BF, (nuint)(&r->SpriteMax1));
		ProtectedMemory.Write(0x571366, (nuint)(&r->SpriteMin1));
		ProtectedMemory.Write(0x571386, (nuint)(&r->SpriteMax1));
		ProtectedMemory.Write(0x59360B, r->SpriteMin2);
		ProtectedMemory.Write(0x593615, r->SpriteMax2);

		ProtectedMemory.Write(0x5054E3, (nuint)(&r->UnknownMin1));
		ProtectedMemory.Write(0x5054FA, (nuint)(&r->UnknownMax1));
		ProtectedMemory.Write(0x59293A, (nuint)(&r->UnknownMax2));
	}
}