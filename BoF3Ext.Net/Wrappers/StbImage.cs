using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Wrappers;


static unsafe partial class StbImage {
	[LibraryImport("stb_image", EntryPoint = "stbi_load_from_memory")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial byte* LoadFromMemory(
		byte* data, int length, out int width, out int height, int* sourceChannels, int channels
	);

	[LibraryImport("stb_image", EntryPoint = "stbi_image_free")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial void Free(void* data);
}
