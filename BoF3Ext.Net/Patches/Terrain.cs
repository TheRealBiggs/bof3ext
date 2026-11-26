using System.Runtime.CompilerServices;

namespace BoF3Ext.Patches;


static class Terrain {
	readonly record struct TerrainCacheBytePatch(nuint Address, byte Value);
	readonly record struct TerrainCacheInstructionPatch(nuint Address, ushort Value);
	readonly record struct TerrainCacheReference(nuint Address, uint Value);


	const uint ORIGINAL_TERRAIN_POOL = 0x905E80;
	const nuint TERRAIN_SLOT_LIMIT_ADDRESS = 0x51087F;


	static readonly nuint[] TERRAIN_PRIM_REFERENCE_ADDRESSES = [
		0x405F74, 0x405F8E, 0x405FAC, 0x405FC6, 0x405FE4, 0x405FFE,
		0x40601C, 0x406036, 0x486EBD, 0x5098F4, 0x5098FB, 0x509909,
		0x509910, 0x509930, 0x5109FC, 0x51242A, 0x512607, 0x5139C2,
		0x513B9B, 0x513BAC, 0x513C41, 0x56EE6A, 0x56EE70, 0x56EEE9,
		0x56F04F, 0x56F283, 0x56F2A8, 0x56F3DC, 0x56F401, 0x56FA5B,
		0x56FA94, 0x570AED, 0x570B09, 0x570B1B, 0x570B32, 0x57126A,
		0x57127C, 0x57128D, 0x5712A2, 0x5932C0, 0x5932F5
	];

	static readonly TerrainCacheBytePatch[] TERRAIN_CACHE_BYTE_PATCHES = [
		new(0x4CE335, 0x28),
		new(0x4CEA2B, 0x28),
		new(0x56ED12, 0x28),
		new(0x56ED17, 0x28),
		new(0x56ED19, 0x14),
		new(0x56ED47, 0x27),
		new(0x56F549, 0x14),
		new(0x56FDAE, 0x28),
		new(0x56FDB3, 0x28),
		new(0x56FDB5, 0x14),
		new(0x56FDDD, 0x27),
		new(0x56ED33, 0x85),
		new(0x56FDC7, 0xB5),
		new(0x56ED3C, 0x03),
		new(0x56FDD2, 0x03)
	];

	static readonly TerrainCacheInstructionPatch[] TERRAIN_CACHE_INSTRUCTION_PATCHES = [
		new(0x56ED38, 0xE803),
		new(0x56FDCD, 0xD603),
	];


	static unsafe void PatchTerrainCache() {
		var manager = TerrainManager.Instance;
		var terrainBase = (uint)Unsafe.AsPointer(ref manager.TerrainCells[0]);

		var objectBase = (uint)Unsafe.AsPointer(ref manager.TerrainObjectCells[0]);
		var objectEnd = objectBase + (uint)(manager.TerrainObjectCells.Length * sizeof(ushort));

		TerrainCacheReference[] references = [
			new(0x4CE32E, objectBase),
			new(0x4CE409, objectEnd),
			new(0x4CEA24, objectBase),
			new(0x4CEB01, objectEnd),
			new(0x56ED5D, terrainBase),
			new(0x56ED64, terrainBase),
			new(0x56FDFA, objectBase),
		];

		foreach (var patch in TERRAIN_CACHE_BYTE_PATCHES)
			ProtectedMemory.Write(patch.Address, patch.Value);

		foreach (var patch in TERRAIN_CACHE_INSTRUCTION_PATCHES)
			ProtectedMemory.Write(patch.Address, patch.Value);

		foreach (var patch in references)
			ProtectedMemory.Write(patch.Address, patch.Value);
	}


	public static unsafe void Apply() {
		PatchTerrainCache();

		var primBase = (uint)Unsafe.AsPointer(ref TerrainManager.Instance.TerrainPrims[0]);

		foreach (var address in TERRAIN_PRIM_REFERENCE_ADDRESSES) {
			var offset = *(uint*)address - ORIGINAL_TERRAIN_POOL;

			ProtectedMemory.Write(address, primBase + offset);
		}

		ProtectedMemory.Write(TERRAIN_SLOT_LIMIT_ADDRESS, (ushort)TerrainManager.TERRAIN_SLOT_COUNT);
	}
}