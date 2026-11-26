using System.Runtime.InteropServices;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Window {
	[GameFunction(0x4FCB00, hookable: true, callingConvention: CallingConvention.StdCall)]
	public static partial int WinMain(nint hInstance, nint hPrevInstance, nint lpCmdLine, int nShowCmd);
}