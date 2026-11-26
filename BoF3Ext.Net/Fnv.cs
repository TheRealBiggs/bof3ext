using System;

namespace BoF3Ext;


struct Fnv {
	public readonly ulong GetHash() => hash;


	ulong hash;


	public Fnv() {
		hash = 0xCBF29CE484222325;
	}


	public void Append(ReadOnlySpan<byte> data) {
		foreach (var value in data) {
			hash ^= value;
			hash = unchecked(hash * 0x100000001B3);
		}
	}
}
