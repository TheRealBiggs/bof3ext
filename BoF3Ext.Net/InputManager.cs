using System;
using System.Collections.Generic;

using Silk.NET.Input;

namespace BoF3Ext;


class InputManager : Singleton<InputManager> {
	public enum Axis {
		None = -1,
		LeftStickX,
		LeftStickY,
		RightStickX,
		RightStickY,
		LeftTrigger,
		RightTrigger
	}

	public record struct Binding(
		Game.Input.Buttons Button,
		Key Key = Key.Unknown,
		ButtonName GamepadButton = ButtonName.Unknown,
		Axis GamepadAxis = Axis.None,
		float AxisDirection = 1f,
		float AxisThreshold = 0.5f
	);


	public Game.Input.Buttons HeldButtons => heldButtons;
	public Game.Input.Buttons PressedButtons => pressedButtons;
	public Game.Input.Buttons ReleasedButtons => releasedButtons;
	public ReadOnlySpan<Binding> Bindings => bindings;


	IInputContext? input;
	IGamepad? gamepad;
	bool focused = true;

	Game.Input.Buttons heldButtons;
	Game.Input.Buttons pressedButtons;
	Game.Input.Buttons releasedButtons;

	Binding[] bindings = [
		new(Game.Input.Buttons.DPadUp, Key.Up, ButtonName.DPadUp, Axis.LeftStickY, -1f),
		new(Game.Input.Buttons.DPadRight, Key.Right, ButtonName.DPadRight, Axis.LeftStickX, 1f),
		new(Game.Input.Buttons.DPadDown, Key.Down, ButtonName.DPadDown, Axis.LeftStickY, 1f),
		new(Game.Input.Buttons.DPadLeft, Key.Left, ButtonName.DPadLeft, Axis.LeftStickX, -1f),

		new(Game.Input.Buttons.Cross, Key.X, ButtonName.A),
		new(Game.Input.Buttons.Circle, Key.C, ButtonName.B),
		new(Game.Input.Buttons.Triangle, Key.S, ButtonName.Y),
		new(Game.Input.Buttons.Square, Key.Z, ButtonName.X),

		new(Game.Input.Buttons.L1, Key.A, ButtonName.LeftBumper),
		new(Game.Input.Buttons.R1, Key.D, ButtonName.RightBumper),
		new(Game.Input.Buttons.L2, Key.Q, ButtonName.Unknown, Axis.LeftTrigger, 1f, 0f),
		new(Game.Input.Buttons.R2, Key.E, ButtonName.Unknown, Axis.RightTrigger, 1f, 0f),

		new(Game.Input.Buttons.Select, Key.Space, ButtonName.Back),
		new(Game.Input.Buttons.Start, Key.Enter, ButtonName.Start),

		new(Game.Input.Buttons.L3, Key.Unknown, ButtonName.LeftStick),
		new(Game.Input.Buttons.R3, Key.Unknown, ButtonName.RightStick),

		// Additional keyboard bindings
		new(Game.Input.Buttons.Triangle, Key.Escape),
		new(Game.Input.Buttons.Circle, Key.ShiftRight)
	];


	public void SetBinding(int index, Binding binding)
		=> bindings[index] = binding;

	public void SetBindings(ReadOnlySpan<Binding> bindings)
		=> this.bindings = bindings.ToArray();

	public void Initialise(IInputContext input) {
		this.input = input;
		gamepad = null;

		Reset();
	}

	public void SetFocused(bool focused) {
		this.focused = focused;
	}

	public void Reset() {
		heldButtons = 0;
		pressedButtons = 0;
		releasedButtons = 0;
	}

	public void Update() {
		var previousButtons = heldButtons;

		heldButtons = ReadButtons();
		pressedButtons = heldButtons & ~previousButtons;
		releasedButtons = previousButtons & ~heldButtons;
	}


	IGamepad? ReadGamepad() {
		if (gamepad != null && gamepad.IsConnected)
			return gamepad;

		gamepad = null;

		for (var i = 0; i < input!.Gamepads.Count; ++i) {
			if (!input.Gamepads[i].IsConnected)
				continue;

			gamepad = input.Gamepads[i];
			gamepad.Deadzone = new(0f, DeadzoneMethod.Traditional);

			return gamepad;
		}

		return null;
	}

	Game.Input.Buttons ReadButtons() {
		if (input == null || !focused)
			return 0;

		var gamepad = ReadGamepad();
		var gamepadButtons = gamepad?.Buttons;
		var thumbsticks = gamepad?.Thumbsticks;
		var triggers = gamepad?.Triggers;
		IKeyboard? keyboard = null;

		for (var i = 0; i < input.Keyboards.Count; ++i) {
			if (!input.Keyboards[i].IsConnected)
				continue;

			keyboard = input.Keyboards[i];
			break;
		}

		Game.Input.Buttons buttons = 0;

		foreach (var binding in bindings) {
			var held = binding.Key != Key.Unknown
				&& keyboard != null && keyboard.IsKeyPressed(binding.Key);

			if (gamepad != null) {
				if (binding.GamepadButton != ButtonName.Unknown) {
					for (var i = 0; i < gamepadButtons!.Count; ++i) {
						if (gamepadButtons[i].Name != binding.GamepadButton)
							continue;

						held |= gamepadButtons[i].Pressed;
						break;
					}
				}

				if (binding.GamepadAxis != Axis.None) {
					var value = ReadAxis(thumbsticks!, triggers!, binding.GamepadAxis) * binding.AxisDirection;

					held |= value > binding.AxisThreshold;
				}
			}

			if (held)
				buttons |= binding.Button;
		}

		return buttons;
	}


	static float ReadAxis(IReadOnlyList<Thumbstick> thumbsticks, IReadOnlyList<Trigger> triggers, Axis axis) {
		if (axis <= Axis.RightStickY) {
			var index = (int)axis / 2;

			if ((uint)index >= (uint)thumbsticks.Count)
				return 0f;

			return ((int)axis & 1) == 0 ? thumbsticks[index].X : thumbsticks[index].Y;
		}

		var trigger = axis - Axis.LeftTrigger;

		return (uint)trigger < (uint)triggers.Count ? triggers[trigger].Position : -1f;
	}
}