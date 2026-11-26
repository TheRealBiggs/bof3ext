using System;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Input {
	[Flags]
	public enum Buttons : ushort {
		L2 = 1 << 0,
		R2 = 1 << 1,
		L1 = 1 << 2,
		R1 = 1 << 3,
		Triangle = 1 << 4,
		Circle = 1 << 5,
		Cross = 1 << 6,
		Square = 1 << 7,
		Select = 1 << 8,
		L3 = 1 << 9,
		R3 = 1 << 10,
		Start = 1 << 11,
		DPadUp = 1 << 12,
		DPadRight = 1 << 13,
		DPadDown = 1 << 14,
		DPadLeft = 1 << 15
	}


	public enum ButtonMappingId {
		Speak,
		Move,
		Menu,
		View,
		Change,
		Select,
		Action,
		Confirm,
		Cancel
	};


	[GameVariable(0x7E1BE8)]
	public static partial ref Buttons HeldButtons { get; }

	[GameVariable(0x7E1BEC)]
	public static partial ref Buttons PressedButtons { get; }

	[GameVariable(0x903580, arrayLength: 9)]
	public static partial Span<Buttons> CurrentButtonMappings { get; set; }
}
