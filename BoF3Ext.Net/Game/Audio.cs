using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Audio {
	// TODO: Remove this once we replace audio stuff

	[GameFunction(0x587C70)]
	public static partial void UpdateAudio();

	[GameFunction(0x5A6830)]
	public static partial int InitialiseAudio(nint hWnd);
}
