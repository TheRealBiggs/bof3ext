using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using unsafe ResetButtonMappingsSig = delegate* unmanaged[Cdecl]<void>;

namespace BoF3Ext.Patches;


static class Input {
	static Game.Input.Buttons[] DefaultButtonMappings = [
		Game.Input.Buttons.Cross | Game.Input.Buttons.L2 | Game.Input.Buttons.R2,	// Speak
		Game.Input.Buttons.Circle,													// Move
		Game.Input.Buttons.Square,													// Menu
		Game.Input.Buttons.R1,														// View
		Game.Input.Buttons.L1,														// Change
		Game.Input.Buttons.Select,													// Select
		Game.Input.Buttons.Triangle,												// Action
		Game.Input.Buttons.Cross | Game.Input.Buttons.L2 | Game.Input.Buttons.R2,	// Confirm
		Game.Input.Buttons.Triangle	// Cancel
	];


	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void ResetButtonMappings() {
		Game.Input.CurrentButtonMappings = DefaultButtonMappings;
	}


	public static unsafe void Apply() {
		// Overwrite default button mappings
		ProtectedMemory.Write(0x656AEC, DefaultButtonMappings);

		ProtectedMemory.WriteCall(0x460FDE, (ResetButtonMappingsSig)(&ResetButtonMappings), trailingNopCount: 49);
		ProtectedMemory.WriteCall(0x588629, (ResetButtonMappingsSig)(&ResetButtonMappings), trailingNopCount: 75);
	}
}