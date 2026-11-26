using System;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Dat {
	public struct EnemyLootInfo {
		public byte ItemId;
		public byte ItemType;
		public byte Chance;
		byte byte_3;
	};

	public struct DatPaletteRow {
		public Array256<ushort> Colours;
	};

	public struct DatPalettes {
		public Array32<DatPaletteRow> Rows;
	};


	[GameVariable(0x64F368, arrayLength: 799)]
	public static partial Span<GameString> FileNames { get; set; }

	[GameVariable(0x80B580)]
	public static partial ref DatPalettes Palettes { get; }

	[GameVariable(0x80B780, arrayLength: 512)]
	public static partial Span<byte> Chunk_0_8200 { get; set; }


	[GameFunction(0x454590, hookable: true)]
	public static partial void LoadDatFile(int id);

	[GameFunction(0x454770, hookable: true)]
	public static partial void LoadDatFile2(int id);
}
