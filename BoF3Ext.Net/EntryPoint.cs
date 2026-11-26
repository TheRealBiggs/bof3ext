using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext;


public static class EntryPoint {
	[UnmanagedCallersOnly(EntryPoint = "Initialise", CallConvs = [typeof(CallConvCdecl)])]
	public static void Initialise() {
		ConfigManager.Instance.Initialise();

		if (ConfigManager.Instance.EnableConsole)
			Console.AllocConsole();

		TextManager.Instance.Initialise();
		GlyphManager.Instance.Initialise();

		Hooks.Battle.Enable();
		Hooks.Dat.Enable();
		Hooks.Gui.Enable();
		Hooks.Overworld.Enable();
		Hooks.Render.Enable();
		Hooks.Terrain.Enable();
		Hooks.Text.Enable();
		Hooks.Widescreen.Enable();
		Hooks.Window.Enable();

		Patches.Gui.Apply();
		Patches.Input.Apply();
		Patches.Render.Apply();
		Patches.Terrain.Apply();
		Patches.Text.Apply();
		Patches.Widescreen.Apply();

		Wrappers.MinHook.Enable(0);	// MH_ALL_HOOKS
	}
}