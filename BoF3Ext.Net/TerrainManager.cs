using System;
using System.Runtime.CompilerServices;

namespace BoF3Ext;


unsafe class TerrainManager : Singleton<TerrainManager> {
	public const int TERRAIN_SLOT_COUNT = 4096;
	public const int TERRAIN_SLOT_SIZE = 144;
	public const int TERRAIN_POOL_SIZE = TERRAIN_SLOT_COUNT * TERRAIN_SLOT_SIZE;
	public const int TERRAIN_CACHE_COLUMNS = 40;
	public const int TERRAIN_CACHE_ROWS = 56;


	public Span<Game.Terrain.TerrainPrim> TerrainPrims => terrainPrims;
	public Span<Game.Terrain.TerrainCell> TerrainCells => terrainCells;
	public Span<ushort> TerrainObjectCells => terrainObjectCells;
	public Span<ushort> TerrainFreeSlots => terrainFreeSlots;


	readonly Game.Terrain.TerrainPrim[] terrainPrims =
		GC.AllocateArray<Game.Terrain.TerrainPrim>(TERRAIN_SLOT_COUNT, pinned: true);
	readonly Game.Terrain.TerrainCell[] terrainCells =
		GC.AllocateArray<Game.Terrain.TerrainCell>(TERRAIN_CACHE_COLUMNS * TERRAIN_CACHE_ROWS, pinned: true);
	readonly ushort[] terrainObjectCells =
		GC.AllocateArray<ushort>(TERRAIN_CACHE_COLUMNS * TERRAIN_CACHE_ROWS, pinned: true);
	readonly ushort[] terrainFreeSlots = GC.AllocateArray<ushort>(TERRAIN_SLOT_COUNT, pinned: true);


	public bool IsTerrainPrim(nuint address) {
		var start = (nuint)Unsafe.AsPointer(ref terrainPrims[0]);

		return address >= start && address - start < TERRAIN_POOL_SIZE;
	}

	public bool TryGetCacheIndex(int x, int y, out int index) {
		var row = x + y;
		var col = x - y;

		index = 0;

		if ((uint)row >= TERRAIN_CACHE_ROWS || (uint)col >= TERRAIN_CACHE_COLUMNS * 2)
			return false;

		row = (row + Game.Terrain.CacheRow + 1) % TERRAIN_CACHE_ROWS;
		col = (col / 2 + Game.Terrain.CacheColumn + 1) % TERRAIN_CACHE_COLUMNS;
		index = row * TERRAIN_CACHE_COLUMNS + col;

		return true;
	}

	public int GetTerrainSlot(int x, int y) {
		return TryGetCacheIndex(x - Game.Terrain.CacheOriginX, y - Game.Terrain.CacheOriginY, out var index)
			? terrainCells[index].SlotAndFlags & 0xFFF
			: 0;
	}

	public Span<uint> GetTerrainObjectCommands(int index) {
		var offset = terrainObjectCells[index];

		if (offset == 0)
			return [];

		ref var header = ref Game.Terrain.Header;
		var commands = (uint*)Unsafe.AsPointer(ref header) + header.TerrainObjectsOffset + offset;

		return new(commands, (int)(commands[-1] >> 16) - 1);
	}

	public Span<uint> GetTerrainObjectCommands(short x, short y) {
		var relativeX = (short)(x - Game.Terrain.CacheOriginX);
		var relativeY = (short)(y - Game.Terrain.CacheOriginY);

		return TryGetCacheIndex(relativeX, relativeY, out var index)
			? GetTerrainObjectCommands(index)
			: [];
	}

	public int FreeTerrainSlot(short slot) {
		var index = (ushort)((Game.Terrain.FreeSlotIndex - 1) & (TERRAIN_SLOT_COUNT - 1));
		Game.Terrain.FreeSlotIndex = index;
		terrainFreeSlots[index] = (ushort)slot;

		return index;
	}

	public short ReleaseTerrainCell(ref Game.Terrain.TerrainCell cell) {
		var slot = cell.SlotAndFlags & 0xFFF;

		if (slot == 0)
			return 0;

		FreeTerrainSlot((short)slot);
		cell.SlotAndFlags = 0;

		ref var prim = ref terrainPrims[slot][1];
		ref var data = ref Unsafe.As<Game.Render.GpuPrim_QuadTexture, byte>(ref prim);
		ref var first = ref Unsafe.As<byte, ushort>(ref Unsafe.Add(ref data, 54));
		ref var second = ref Unsafe.As<byte, ushort>(ref Unsafe.Add(ref data, 70));

		if (first != 0) {
			FreeTerrainSlot((short)first);
			first = 0;
		}

		var result = (short)second;

		if (second != 0) {
			result = (short)FreeTerrainSlot((short)second);
			second = 0;
		}

		return result;
	}

	public void InitialiseTerrainPrims() {
		for (var slot = 0; slot < TERRAIN_SLOT_COUNT; slot++) {
			for (var frame = 0; frame < 2; frame++) {
				ref var prim = ref terrainPrims[slot][frame];

				prim.Value = ((byte)Game.Render.GpuPrimType.QuadTexture << 2);
				prim.Vert1.Z = prim.Vert2.Z = prim.Vert3.Z = prim.Vert4.Z = 0.01f;

				ResetTerrainPrimMetadata(ref prim);
			}
		}
	}

	public void ResetTerrainSlotMetadata() {
		for (var slot = 0; slot < TERRAIN_SLOT_COUNT; slot++) {
			for (var frame = 0; frame < 2; frame++)
				ResetTerrainPrimMetadata(ref terrainPrims[slot][frame]);
		}
	}


	static void ResetTerrainPrimMetadata(ref Game.Render.GpuPrim_QuadTexture prim) {
		ref var data = ref Unsafe.As<Game.Render.GpuPrim_QuadTexture, byte>(ref prim);

		Unsafe.WriteUnaligned(ref Unsafe.Add(ref data, 54), (ushort)0);
		Unsafe.WriteUnaligned(ref Unsafe.Add(ref data, 70), (ushort)0);
	}
}
