
namespace BoF3Ext.Patches;


static class Render {
	public static void Apply() {
		// Fix incorrect identity matrix initialisation for shaded tris.
		// Most notably used for beams of light when Ryu's chrysm is blown up at the start of the game in Dauna Mine.
		ProtectedMemory.Write(0x494185, 0x1000U);
	}
}