using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Wrappers;


static unsafe partial class StbImageWrite {
	[LibraryImport("stb_image_write", EntryPoint = "stb_image_write_png")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial byte* EncodePng(byte* data, int width, int height, out int length);

	[LibraryImport("stb_image_write", EntryPoint = "stb_image_write_free")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial void Free(void* data);
}
