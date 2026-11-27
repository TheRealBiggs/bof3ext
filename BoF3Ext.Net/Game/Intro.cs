
using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Intro {
	[GameFunction(0x462600, hookable: true)]
	public static partial void DrawTapestry();


	[GameVariable(0x66C7F9)]
	public static partial byte TapestryState { get; set; }

	[GameVariable(0x66C7FE)]
	public static partial short TapestryScroll { get; set; }

	[GameVariable(0x66C800)]
	public static partial byte TapestryFade { get; set; }
}