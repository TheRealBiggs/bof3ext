using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Hooks;


static unsafe partial class Terrain {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static ushort AllocateTerrainSlotHook() {
		var index = Game.Terrain.FreeSlotIndex;

		if (index == 0)
			return 0;

		var slot = TerrainManager.Instance.TerrainFreeSlots[index];

		Game.Terrain.FreeSlotIndex = (ushort)((index + 1) & (TerrainManager.TERRAIN_SLOT_COUNT - 1));

		return slot;
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static int FreeTerrainSlotHook(short slot)
		=> TerrainManager.Instance.FreeTerrainSlot(slot);

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static int ScrollTerrainColumnsBackHook() {
		var cacheColumn = Game.Terrain.CacheColumn;

		var row = (int)Game.Terrain.CacheRow;
		var result = 0;

		for (var i = 0; i < TerrainManager.TERRAIN_CACHE_ROWS; ++i) {
			row = WrapTerrainCacheIndex(row + 1, TerrainManager.TERRAIN_CACHE_ROWS);

			ref var cell = ref GetTerrainCell(row, cacheColumn);

			TerrainManager.Instance.ReleaseTerrainCell(ref cell);

			result = PopulateTerrainCell(i, -1, ref cell);
		}

		Game.Terrain.CacheColumn = (short)WrapTerrainCacheIndex(cacheColumn - 1, TerrainManager.TERRAIN_CACHE_COLUMNS);

		Game.Terrain.CacheOriginX--;
		Game.Terrain.CacheOriginY++;

		return result;
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static int ScrollTerrainColumnsForwardHook() {
		var cacheColumn = Game.Terrain.CacheColumn;

		var column = WrapTerrainCacheIndex(cacheColumn + 1, TerrainManager.TERRAIN_CACHE_COLUMNS);
		var row = (int)Game.Terrain.CacheRow;
		var result = 0;

		for (var i = 0; i < TerrainManager.TERRAIN_CACHE_ROWS; ++i) {
			row = WrapTerrainCacheIndex(row + 1, TerrainManager.TERRAIN_CACHE_ROWS);

			ref var cell = ref GetTerrainCell(row, column);

			TerrainManager.Instance.ReleaseTerrainCell(ref cell);
			result = PopulateTerrainCell(i, (int)TerrainManager.TERRAIN_CACHE_COLUMNS, ref cell);
		}

		Game.Terrain.CacheOriginX++;
		Game.Terrain.CacheOriginY--;

		Game.Terrain.CacheColumn = (short)column;

		return result;
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static int RefreshTerrainCacheHook() {
		var cacheColumn = Game.Terrain.CacheColumn;

		var row = (int)Game.Terrain.CacheRow;

		for (var i = 0; i < TerrainManager.TERRAIN_CACHE_ROWS; ++i) {
			row = WrapTerrainCacheIndex(row + 1, TerrainManager.TERRAIN_CACHE_ROWS);

			var column = (int)cacheColumn;

			for (var j = 0; j < TerrainManager.TERRAIN_CACHE_COLUMNS; ++j) {
				column = WrapTerrainCacheIndex(column + 1, TerrainManager.TERRAIN_CACHE_COLUMNS);

				PopulateTerrainCell(i, j, ref GetTerrainCell(row, column));
			}
		}

		return PopulateTerrainObjects();
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static int InitialiseTerrainHook() {
		var cameraX = Game.Terrain.CameraX;
		var cameraY = Game.Terrain.CameraY;
		ref var header = ref Game.Terrain.Header;
		ref var rotation = ref Game.Terrain.CameraRotation;

		Game.Render.SetGeomScreen(1000);
		Game.Render.SetGeomOffset(160, 120);
		Game.Terrain.SetTerrainBackgroundColour(120, 120, 120);

		var manager = TerrainManager.Instance;
		var prims = manager.TerrainPrims;

		manager.InitialiseTerrainPrims();

		rotation.Vx = -682;
		rotation.Vy = 0;
		rotation.Vz = 512;
		Game.Terrain.PreviousCameraRotation = rotation;

		Game.Terrain.PositionX = 0x7FFF - (cameraX >> 8);
		Game.Terrain.PositionY = 0x8000 - (cameraY >> 8);

		for (var frame = 0; frame < 2; ++frame) {
			ref var prim = ref prims[0][frame];

			prim.Vert1.X = prim.Vert1.Y = 0f;
			prim.Vert2.X = prim.Vert2.Y = 0f;
			prim.Vert3.X = prim.Vert3.Y = 0f;
			prim.Vert4.X = prim.Vert4.Y = 0f;
		}

		Game.Terrain.CameraDepthOffset = 0;
		Game.Terrain.CameraTranslationOffsetY = 0;
		Game.Terrain.CameraTranslationOffsetX = 0;

		SetTerrainCacheOrigin();

		short height = 0;

		if ((Game.Terrain.AreaFlags & 8) == 0)
			height = Game.Terrain.GetTerrainHeight(cameraX, cameraY);

		Game.Terrain.PositionZ = height;

		Game.Terrain.BackgroundDrawFlags = (byte)(Game.Terrain.AreaFlags << 7);
		Game.Terrain.TerrainSortEntryCount = 0;

		var rowOffset = header.TerrainRowOffset;

		Game.Terrain.RowOffset = rowOffset != 0 ? rowOffset : (short)(-2 * height);
		Game.Terrain.ColumnOffset = 0;

		Game.Terrain.CameraPanStep = 0;
		Game.Terrain.CameraHeightDelta = 0;
		Game.Terrain.ApplyHeightAdjustments = false;
		Game.Terrain.WalkMap = (byte*)Unsafe.AsPointer(ref header) + 4 * header.WalkMapOffset;
		*(byte*)0x905E20 = 0;
		Game.Render.TerrainRefreshCounter = 3;

		ResetTerrainCache();

		Game.Terrain.UpdateTerrainTextureMap();
		Game.Terrain.InitialiseTerrainObjects();

		return PopulateTerrainObjects();
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static int ResetTerrainHook() {
		ref var header = ref Game.Terrain.Header;

		TerrainManager.Instance.ResetTerrainSlotMetadata();

		Game.Terrain.PositionX = 0x7FFF - (Game.Terrain.CameraX >> 8);
		Game.Terrain.PositionY = 0x8000 - (Game.Terrain.CameraY >> 8);
		Game.Terrain.TerrainSortEntryCount = 0;
		SetTerrainCacheOrigin();

		var initialRowOffset = header.TerrainRowOffset;

		Game.Terrain.RowOffset = initialRowOffset != 0 ? initialRowOffset : (short)(-2 * Game.Terrain.PositionZ);
		Game.Terrain.ColumnOffset = (short)0;

		while (Game.Terrain.RowOffset >= 512) {
			ScrollTerrainRowsForward();
			Game.Terrain.RowOffset -= 512;
		}

		while (Game.Terrain.RowOffset <= -512) {
			ScrollTerrainRowsBack();
			Game.Terrain.RowOffset += 512;
		}

		Game.Terrain.CameraPanStep = 0;
		Game.Terrain.CameraHeightDelta = 0;
		Game.Terrain.ApplyHeightAdjustments = false;
		Game.Render.TerrainRefreshCounter = 3;

		ResetTerrainCache();
		Game.Terrain.UpdateTerrainTextureMap();

		return Game.Terrain.InitialiseTerrainObjects();
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static int ScrollTerrainRowsBackHook()
		=> ScrollTerrainRowsBack();

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static short ScrollTerrainRowsForwardHook()
		=> ScrollTerrainRowsForward();

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static int PopulateTerrainObjectsHook()
		=> PopulateTerrainObjects();


	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static short ReleaseTerrainCellHook(nint cell)
		=> TerrainManager.Instance.ReleaseTerrainCell(ref Unsafe.AsRef<Game.Terrain.TerrainCell>((void*)cell));

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static int GetTerrainSlotHook(int x, int y)
		=> TerrainManager.Instance.GetTerrainSlot(x, y);

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static nint GetTerrainPrimHook(int x, int y) {
		var manager = TerrainManager.Instance;
		var slot = manager.GetTerrainSlot(x, y);

		return slot == 0 ? 0 : (nint)Unsafe.AsPointer(ref manager.TerrainPrims[slot][Game.Render.CurrentEnvIndex]);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void ApplyTerrainOverlayAtCellHook(nint command, int x, int y) {
		var slot = TerrainManager.Instance.GetTerrainSlot(x, y);

		if (slot != 0)
			Game.Terrain.ApplyTerrainOverlay((int)(*(uint*)command >> 24) - 16, slot);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static byte GetTerrainObjectConditionHook(short x, short y) {
		ref var header = ref Game.Terrain.Header;
		var commands = TerrainManager.Instance.GetTerrainObjectCommands(x, y);

		for (var i = 0; i < commands.Length; i += (byte)(commands[i] >> 16)) {
			if ((commands[i] & 0xFF000000) is 0x23000000 or 0x24000000 or 0x25000000) {
				var condition = (uint*)Unsafe.AsPointer(ref header) + header.ConditionalChangesOffset + (ushort)commands[i];

				if ((*condition & 0xF000) == 0x8000)
					return (byte)(*condition & 1);
			}
		}

		return 0;
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static byte SetTerrainObjectConditionHook(short x, short y)
		=> SetTerrainObjectCondition(x, y, true);

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static byte ClearTerrainObjectConditionHook(short x, short y)
		=> SetTerrainObjectCondition(x, y, false);

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static short ClearTerrainCellHook(short x, short y) {
		ref var header = ref Game.Terrain.Header;
		var manager = TerrainManager.Instance;

		Game.Terrain.WalkMap[x + y * header.Width] = 0;

		var relativeX = (short)(x + 1 - Game.Terrain.CacheOriginX);
		var relativeY = (short)(y - Game.Terrain.CacheOriginY);

		if (!manager.TryGetCacheIndex(relativeX, relativeY, out var index))
			return (short)((uint)(relativeX + relativeY) >= TerrainManager.TERRAIN_CACHE_ROWS ? 1 : relativeX - relativeY);

		var offset = manager.TerrainObjectCells[index];

		if (offset == 0)
			return 0;

		var result = header.TerrainObjectsOffset + offset;
		var commands = manager.GetTerrainObjectCommands(index);

		for (var i = 0; i < commands.Length; i += result) {
			if ((commands[i] & 0xFF000000) is 0x03000000 or 0x09000000 or 0x27000000)
				commands[i] = (commands[i] & 0x00FFFFFF) | 0x30000000;

			result = (byte)(commands[i] >> 16);
		}

		return (short)result;
	}


	static byte SetTerrainObjectCondition(short x, short y, bool enabled) {
		ref var header = ref Game.Terrain.Header;
		var commands = TerrainManager.Instance.GetTerrainObjectCommands(x, y);
		byte result = 0;

		for (var i = 0; i < commands.Length; i += (byte)(commands[i] >> 16)) {
			if ((commands[i] & 0xFF000000) != 0x23000000)
				continue;

			commands[i] = (commands[i] & 0x00FFFFFF) | 0x24000000;

			var condition = (uint*)Unsafe.AsPointer(ref header) + header.ConditionalChangesOffset + (ushort)commands[i];

			if ((*condition & 0xF001) == (enabled ? 0x8000u : 0x8001u)) {
				*condition = enabled ? *condition | 1u : *condition & ~1u;
				result = 1;
			}
		}

		return result;
	}

	static int WrapTerrainCacheIndex(int index, int size) {
		index %= size;

		return index < 0 ? index + size : index;
	}

	static ref Game.Terrain.TerrainCell GetTerrainCell(int row, int column)
		=> ref TerrainManager.Instance.TerrainCells[row * TerrainManager.TERRAIN_CACHE_COLUMNS + column];

	static void SetTerrainCacheOrigin() {
		var offset = (TerrainManager.TERRAIN_CACHE_COLUMNS - 28) / 2;

		Game.Terrain.CacheOriginX = (short)((Game.Terrain.CameraX >> 16) - 24 - offset);
		Game.Terrain.CacheOriginY = (short)((Game.Terrain.CameraY >> 16) + 3 + offset);
	}

	static void ResetTerrainCache() {
		for (var row = 0; row < TerrainManager.TERRAIN_CACHE_ROWS; ++row) {
			for (var column = 0; column < TerrainManager.TERRAIN_CACHE_COLUMNS; ++column) {
				ref var cell = ref GetTerrainCell(row, column);

				cell.SlotAndFlags = 0;
				PopulateTerrainCell(row, column, ref cell);
			}
		}

		var freeSlots = TerrainManager.Instance.TerrainFreeSlots;

		for (var i = 0; i < TerrainManager.TERRAIN_SLOT_COUNT; ++i)
			freeSlots[i] = (ushort)i;

		Game.Terrain.FreeSlotIndex = 1;
		Game.Terrain.CacheColumn = TerrainManager.TERRAIN_CACHE_COLUMNS - 1;
		Game.Terrain.CacheRow = TerrainManager.TERRAIN_CACHE_ROWS - 1;
	}

	static int PopulateTerrainCell(int row, int column, ref Game.Terrain.TerrainCell cell) {
		ref var header = ref Game.Terrain.Header;

		var x = (row + 1) / 2 + column + Game.Terrain.CacheOriginX;
		var y = row / 2 - column + Game.Terrain.CacheOriginY;
		var width = header.Width;
		var height = header.Height;
		var tileMapOffset = header.TileMapOffset;

		if (x > 0 && x < width - 1 && y > 0 && y < height - 1
			&& ((ushort*)Unsafe.AsPointer(ref header))[tileMapOffset * 2 + y * width + x] != 0) {
			cell.X = (byte)x;
			cell.Y = (byte)y;

			return y;
		}

		cell.X = 0;
		cell.Y = 0;

		return (int)Unsafe.AsPointer(ref cell);
	}

	static int ScrollTerrainRowsBack() {
		var cacheColumn = Game.Terrain.CacheColumn;

		for (var i = 0; i < 2; ++i) {
			var column = (int)cacheColumn;

			for (var j = 0; j < TerrainManager.TERRAIN_CACHE_COLUMNS; ++j) {
				column = WrapTerrainCacheIndex(column + 1, TerrainManager.TERRAIN_CACHE_COLUMNS);

				ref var cell = ref GetTerrainCell(Game.Terrain.CacheRow, column);

				TerrainManager.Instance.ReleaseTerrainCell(ref cell);
				PopulateTerrainCell((i == 0 ? 1 : 0) - 3, j, ref cell);
			}

			Game.Terrain.CacheRow = (short)WrapTerrainCacheIndex(
				Game.Terrain.CacheRow - 1,
				TerrainManager.TERRAIN_CACHE_ROWS
			);
		}

		Game.Terrain.CacheOriginX--;
		Game.Terrain.CacheOriginY--;

		return 2;
	}

	static short ScrollTerrainRowsForward() {
		var cacheColumn = Game.Terrain.CacheColumn;

		for (var i = 0; i < 2; ++i) {
			var row = WrapTerrainCacheIndex(Game.Terrain.CacheRow + 1, TerrainManager.TERRAIN_CACHE_ROWS);
			var column = (int)cacheColumn;

			for (var j = 0; j < TerrainManager.TERRAIN_CACHE_COLUMNS; ++j) {
				column = WrapTerrainCacheIndex(column + 1, TerrainManager.TERRAIN_CACHE_COLUMNS);

				ref var cell = ref GetTerrainCell(row, column);

				TerrainManager.Instance.ReleaseTerrainCell(ref cell);
				PopulateTerrainCell(i + (int)TerrainManager.TERRAIN_CACHE_ROWS, j, ref cell);
			}

			Game.Terrain.CacheRow = (short)row;
		}

		Game.Terrain.CacheOriginX++;
		Game.Terrain.CacheOriginY++;

		return Game.Terrain.CacheRow;
	}

	static int PopulateTerrainObjects() {
		ref var header = ref Game.Terrain.Header;
		var originX = Game.Terrain.CacheOriginX;
		var originY = Game.Terrain.CacheOriginY;
		var cacheRow = Game.Terrain.CacheRow;
		var cacheColumn = Game.Terrain.CacheColumn;

		var terrainObjectCells = TerrainManager.Instance.TerrainObjectCells;

		terrainObjectCells.Clear();

		var records = (uint*)Unsafe.AsPointer(ref header) + header.TerrainObjectsOffset;
		var record = records;

		while (*record != 0) {
			var x = (int)((*record >> 8) & 0xFF) - originX;
			var y = (int)(*record & 0xFF) - originY;
			var row = x + y;
			var column = x - y;

			if ((uint)row < TerrainManager.TERRAIN_CACHE_ROWS && (uint)column < TerrainManager.TERRAIN_CACHE_COLUMNS * 2) {
				row = WrapTerrainCacheIndex(row + cacheRow + 1, TerrainManager.TERRAIN_CACHE_ROWS);
				column = WrapTerrainCacheIndex(column / 2 + cacheColumn + 1, TerrainManager.TERRAIN_CACHE_COLUMNS);

				terrainObjectCells[row * TerrainManager.TERRAIN_CACHE_COLUMNS + column] = (ushort)(record - records + 1);
			}

			record += *record >> 16;
		}

		return 0;
	}


	public static void Enable() {
		Game.Terrain.HookReleaseTerrainCell(&ReleaseTerrainCellHook);
		Game.Terrain.HookGetTerrainSlot(&GetTerrainSlotHook);
		Game.Terrain.HookGetTerrainPrim(&GetTerrainPrimHook);
		Game.Terrain.HookApplyTerrainOverlayAtCell(&ApplyTerrainOverlayAtCellHook);
		Game.Terrain.HookGetTerrainObjectCondition(&GetTerrainObjectConditionHook);
		Game.Terrain.HookSetTerrainObjectCondition(&SetTerrainObjectConditionHook);
		Game.Terrain.HookClearTerrainObjectCondition(&ClearTerrainObjectConditionHook);
		Game.Terrain.HookClearTerrainCell(&ClearTerrainCellHook);
		Game.Terrain.HookAllocateTerrainSlot(&AllocateTerrainSlotHook);
		Game.Terrain.HookFreeTerrainSlot(&FreeTerrainSlotHook);
		Game.Terrain.HookScrollTerrainColumnsBack(&ScrollTerrainColumnsBackHook);
		Game.Terrain.HookScrollTerrainColumnsForward(&ScrollTerrainColumnsForwardHook);
		Game.Terrain.HookScrollTerrainRowsBack(&ScrollTerrainRowsBackHook);
		Game.Terrain.HookScrollTerrainRowsForward(&ScrollTerrainRowsForwardHook);
		Game.Terrain.HookPopulateTerrainObjects(&PopulateTerrainObjectsHook);
		Game.Terrain.HookRefreshTerrainCache(&RefreshTerrainCacheHook);
		Game.Terrain.HookInitialiseTerrain(&InitialiseTerrainHook);
		Game.Terrain.HookResetTerrain(&ResetTerrainHook);
	}
}