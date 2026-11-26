using System.Runtime.CompilerServices;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static unsafe partial class Terrain {
	[InlineArray(2)]
	public struct TerrainPrim {
		public Render.GpuPrim_QuadTexture Value;
	}

	public struct TerrainCell {
		public byte X;
		public byte Y;
		public ushort SlotAndFlags;
	}

	public struct AreaHeader {
		public byte Width;
		public byte Height;
		public ushort TileMapOffset;
		public ushort TileTableOffset0;
		public ushort QuadTableOffset0;
		public ushort TileTableOffset1;
		public ushort QuadTableOffset1;
		public ushort TileTableOffset2;
		public ushort QuadTableOffset2;
		public ushort TileTableOffset3;
		public ushort QuadTableOffset3;
		public ushort WalkMapOffset;
		public ushort U0;
		public ushort U1;
		public ushort OverlayColour0;
		public ushort OverlayColour1;
		public short TerrainRowOffset;
		public ushort SetupCommandsOffset;
		public ushort RenderCommandsOffset;
		public ushort TerrainObjectsOffset;
		public ushort U8;
		public ushort ConditionalChangesOffset;
		public ushort HeightAdjustmentsOffset;
		public ushort U11;
		public ushort U12;
	}


	[GameFunction(0x405BF0, hookable: true)]
	public static partial byte GetTerrainObjectCondition(short x, short y);

	[GameFunction(0x4B5350, hookable: true)]
	public static partial int ResetTerrain();

	[GameFunction(0x56E9A0, hookable: true)]
	public static partial int ScrollTerrainColumnsBack();

	[GameFunction(0x56EA30, hookable: true)]
	public static partial int ScrollTerrainColumnsForward();

	[GameFunction(0x56EAB0, hookable: true)]
	public static partial int ScrollTerrainRowsBack();

	[GameFunction(0x56EB50, hookable: true)]
	public static partial short ScrollTerrainRowsForward();

	[GameFunction(0x56F670, hookable: true)]
	public static partial int InitialiseTerrain();

	[GameFunction(0x56FAD0)]
	public static partial uint UpdateTerrainTextureMap();

	[GameFunction(0x56FBD0, hookable: true)]
	public static partial ushort AllocateTerrainSlot();

	[GameFunction(0x56FC00, hookable: true)]
	public static partial short ReleaseTerrainCell(nint cell);

	[GameFunction(0x56FC70, hookable: true)]
	public static partial int FreeTerrainSlot(short slot);

	[GameFunction(0x56FCA0, hookable: true)]
	public static partial int RefreshTerrainCache();

	[GameFunction(0x570A00, hookable: true)]
	public static partial void ApplyTerrainOverlayAtCell(nint command, int x, int y);

	[GameFunction(0x570AB0)]
	public static partial void ApplyTerrainOverlay(int flags, int slot);

	[GameFunction(0x571720)]
	public static partial int InitialiseTerrainObjects();

	[GameFunction(0x571FF0, hookable: true)]
	public static partial int PopulateTerrainObjects();

	[GameFunction(0x5720C0)]
	public static partial short GetTerrainHeight(int x, int y);

	[GameFunction(0x572650, hookable: true)]
	public static partial byte SetTerrainObjectCondition(short x, short y);

	[GameFunction(0x572790, hookable: true)]
	public static partial byte ClearTerrainObjectCondition(short x, short y);

	[GameFunction(0x5728D0, hookable: true)]
	public static partial short ClearTerrainCell(short x, short y);

	[GameFunction(0x572ED0, hookable: true)]
	public static partial int GetTerrainSlot(int x, int y);

	[GameFunction(0x572F70, hookable: true)]
	public static partial nint GetTerrainPrim(int x, int y);

	[GameFunction(0x5A7B60)]
	public static partial int SetTerrainBackgroundColour(int r, short g, short b);


	[GameVariable(0x7E0688)]
	public static partial short CacheOriginX { get; set; }

	[GameVariable(0x7E068A)]
	public static partial short CacheOriginY { get; set; }

	[GameVariable(0x8CB580)]
	public static partial ref AreaHeader Header { get; }

	[GameVariable(0x903800)]
	public static partial short CameraTranslationOffsetX { get; set; }

	[GameVariable(0x903802)]
	public static partial short CameraTranslationOffsetY { get; set; }

	[GameVariable(0x903840)]
	public static partial short CameraDepthOffset { get; set; }

	[GameVariable(0x9039D4)]
	public static partial ushort FreeSlotIndex { get; set; }

	[GameVariable(0x904EFE)]
	public static partial short CameraHeightDelta { get; set; }

	[GameVariable(0x905BA0)]
	public static partial byte TerrainSortEntryCount { get; set; }

	[GameVariable(0x905BA2)]
	public static partial byte AreaFlags { get; set; }

	[GameVariable(0x905D88)]
	public static partial ref Render.SVECTOR PreviousCameraRotation { get; }

	[GameVariable(0x905D94)]
	public static partial byte* WalkMap { get; set; }

	[GameVariable(0x905E60)]
	public static partial int CameraY { get; set; }

	[GameVariable(0x905E64)]
	public static partial int CameraX { get; set; }

	[GameVariable(0x929EC8)]
	public static partial ref Render.SVECTOR CameraRotation { get; }

	[GameVariable(0x929ED8)]
	public static partial byte BackgroundDrawFlags { get; set; }

	[GameVariable(0x929F14)]
	public static partial int PositionX { get; set; }

	[GameVariable(0x929F18)]
	public static partial int PositionY { get; set; }

	[GameVariable(0x929F1C)]
	public static partial int PositionZ { get; set; }

	[GameVariable(0x929F20)]
	public static partial short CacheColumn { get; set; }

	[GameVariable(0x929F22)]
	public static partial bool ApplyHeightAdjustments { get; set; }

	[GameVariable(0x929F24)]
	public static partial short CacheRow { get; set; }

	[GameVariable(0x92BEE0)]
	public static partial short ColumnOffset { get; set; }

	[GameVariable(0x92BEE2)]
	public static partial short RowOffset { get; set; }

	[GameVariable(0x937F8C)]
	public static partial short CameraPanStep { get; set; }
}
