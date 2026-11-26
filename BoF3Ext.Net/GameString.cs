
namespace BoF3Ext;


readonly struct GameString {
	public readonly unsafe byte* Pointer;


	public override unsafe string ToString() {
		// TODO: Decode multi-byte characters
		return new((sbyte*)Pointer);
	}


	internal unsafe GameString(byte* pointer) {
		Pointer = pointer;
	}
}