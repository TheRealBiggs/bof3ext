using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using unsafe ResetButtonMappingsSig = delegate* unmanaged[Cdecl]<void>;

namespace BoF3Ext.Patches;


static class Input {
	static readonly Game.Input.Buttons[] NewDefaultButtonMappings = [
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
		Game.Input.CurrentButtonMappings = NewDefaultButtonMappings;
	}


	public static unsafe void Apply() {
		// Overwrite default button mappings
		Game.Input.DefaultButtonMappings = NewDefaultButtonMappings;

		// Use Square to unequip during battle
		// Needed because the gamepad replacement code causes a conflict, due to the
		//   PC port hard-coding triangle for unequip.
		ProtectedMemory.Write(0x448EDC, (byte)Game.Input.Buttons.Square);

		// Use Square to return a learned skill to Skill Notes
		ProtectedMemory.Write(0x584920, (byte)Game.Input.Buttons.Square);

		// Use Square to unequip selected item on equipment screen
		ProtectedMemory.Write(0x58C9DF, (byte)Game.Input.Buttons.Square);

		ProtectedMemory.WriteCall(0x460FDE, (ResetButtonMappingsSig)(&ResetButtonMappings), trailingNopCount: 49);
		ProtectedMemory.WriteCall(0x588629, (ResetButtonMappingsSig)(&ResetButtonMappings), trailingNopCount: 75);
	}
}