using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

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
		public uint SpriteLeftMargin1, SpriteLeftMargin2;
		public float UnknownMin1, UnknownMax1;
		public float UnknownMax2;
	}


	static unsafe readonly DrawRanges* drawRanges =
		(DrawRanges*)NativeMemory.Alloc((nuint)Unsafe.SizeOf<DrawRanges>());


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
		var width = 320;

		x = (short)Math.Round((width - maxColumns * advance) * 0.5f);

		return Game.Render.DrawStringNormal(x, y, paletteIndex, len, text);
	}


	public static unsafe void Apply() {
		// Demo text
		ReadOnlySpan<nuint> DEMO_TEXT_DRAWSTRINGS = [
			0x56B63A, 0x56B68E, 0x56B6EF, 0x56B781, 0x56B7C8, 0x56B819, 0x56B9AB, 0x56B9E4,
			0x56BA3C, 0x56BB0C, 0x56BB75, 0x56BBC8, 0x56BD95, 0x56BF9B, 0x56BFE5, 0x56C03B
		];

		foreach (var address in DEMO_TEXT_DRAWSTRINGS)
			ProtectedMemory.WriteCall(address, (DrawDemoTextSig)(&DrawDemoText));

		// Disable terrain column trimming
		ProtectedMemory.Nop(0x56ECD3, 2);

		// One-time draw range patches
		// `drawRanges` is allocated once and then never moved, so we can safely just
		//   write pointers to the values in it
		ProtectedMemory.Write(0x5109BB, (nuint)(&drawRanges->TerrainMin1));
		ProtectedMemory.Write(0x5109D2, (nuint)(&drawRanges->TerrainMax1));
		ProtectedMemory.Write(0x51097E, (nuint)(&drawRanges->TerrainMin2));
		ProtectedMemory.Write(0x510991, (nuint)(&drawRanges->TerrainMax2));
		ProtectedMemory.Write(0x56EDFA, (nuint)(&drawRanges->TerrainMin1));
		ProtectedMemory.Write(0x56EE14, (nuint)(&drawRanges->TerrainMax1));

		ProtectedMemory.Write(0x4CEC09, (nuint)(&drawRanges->ObjectMin1));
		ProtectedMemory.Write(0x4CEC1F, (nuint)(&drawRanges->ObjectMax1));
		ProtectedMemory.Write(0x5700D1, (nuint)(&drawRanges->ObjectMin1));
		ProtectedMemory.Write(0x5700E7, (nuint)(&drawRanges->ObjectMax1));
		ProtectedMemory.Write(0x570319, (nuint)(&drawRanges->ObjectMin2));
		ProtectedMemory.Write(0x570333, (nuint)(&drawRanges->ObjectMax2));

		ProtectedMemory.Write(0x4CF319, (nuint)(&drawRanges->SpriteMin1));
		ProtectedMemory.Write(0x4CF335, (nuint)(&drawRanges->SpriteMax1));
		ProtectedMemory.Write(0x4FF6A3, (nuint)(&drawRanges->SpriteMin1));
		ProtectedMemory.Write(0x4FF6BF, (nuint)(&drawRanges->SpriteMax1));
		ProtectedMemory.Write(0x571366, (nuint)(&drawRanges->SpriteMin1));
		ProtectedMemory.Write(0x571386, (nuint)(&drawRanges->SpriteMax1));

		ProtectedMemory.Write(0x5054E3, (nuint)(&drawRanges->UnknownMin1));
		ProtectedMemory.Write(0x5054FA, (nuint)(&drawRanges->UnknownMax1));
		ProtectedMemory.Write(0x59293A, (nuint)(&drawRanges->UnknownMax2));

		// Read the two sprite left margins as uints so they can include the widescreen offset.
		ProtectedMemory.Write(0x57B9F5, new byte[] { 0x8B, 0x90 }); // mov edx, [eax + margins]
		ProtectedMemory.Write(0x57B9F7, (nuint)(&drawRanges->SpriteLeftMargin1));

		Update();
	}

	public static void Update() {
		var cfgMgr = ConfigManager.Instance;

		var renderWidth = (float)cfgMgr.ScaledRenderWidth;

		var offset = cfgMgr.HalfRenderWidthDiff;

		PatchDrawRanges(cfgMgr.RenderWidthDiff, offset);

		// Widescreen fix for sprite culling
		ProtectedMemory.Write(0x57B9EB, (uint)(cfgMgr.ScaledRenderWidth - offset));

		// Widescreen fix for black fade
		ProtectedMemory.Write(0x4957C1, (float)-offset);
		ProtectedMemory.Write(0x4957CF, renderWidth);

		// Widescreen fix for menu background
		ProtectedMemory.Write(0x575721, -offset); // Cancel shader offset for the first column

		// Repeat background pattern to fit extended screen width.
		// Each column is 32px wide and alternates between 2 patterns, meaning 64px per repeat.
		ProtectedMemory.Write(0x575732, (uint)Math.Ceiling(renderWidth / 32 / 2));
	}


	static unsafe void PatchDrawRanges(float diff, int offset) {
		var r = drawRanges;

		*r = new() {
			TerrainMin1 = -50f - diff - offset,
			TerrainMax1 = 370f + diff - offset,
			TerrainMin2 = -200f - diff - offset,
			TerrainMax2 = 520f + diff - offset,
			ObjectMin1 = -100f - diff - offset,
			ObjectMax1 = 420f + diff - offset,
			ObjectMin2 = -80f - diff - offset,
			ObjectMax2 = 400f + diff - offset,
			SpriteMin1 = -60f - diff - offset,
			SpriteMax1 = 380f + diff - offset,
			SpriteMin2 = (short)(384 + diff - offset),
			SpriteMax2 = (short)(-64 - diff - offset),
			SpriteLeftMargin1 = (uint)(80 + offset),
			SpriteLeftMargin2 = (uint)(100 + offset),
			UnknownMin1 = -20f - diff - offset,
			UnknownMax1 = 340f + diff - offset,
			UnknownMax2 = 320f + diff - offset
		};

		ProtectedMemory.Write(0x59360B, r->SpriteMin2);
		ProtectedMemory.Write(0x593615, r->SpriteMax2);
	}
}