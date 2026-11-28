using System;
using System.IO;

using Silk.NET.OpenGL;

using BoF3Ext.Wrappers;

namespace BoF3Ext;


sealed unsafe record TextureImage(int Width, int Height, byte[] Data) {
	public static TextureImage Load(string filename, bool greyscale = false) {
		var buf = File.ReadAllBytes(filename);
		var channels = greyscale ? 1 : 4;
		byte* pixels;
		int width;
		int height;

		fixed (byte* pBuf = buf)
			pixels = StbImage.LoadFromMemory(pBuf, buf.Length, out width, out height, null, channels);

		if (pixels == null)
			throw new InvalidDataException($"Failed to decode PNG '{filename}'.");

		ReadOnlySpan<byte> data = new(pixels, checked(width * height * channels));
		TextureImage ti = new(width, height, data.ToArray());

		StbImage.Free(pixels);

		return ti;
	}

	public static uint LoadTexture(GL gl, string filename, bool mipmaps = true) {
		var image = Load(filename);
		var texture = gl.GenTexture();

		gl.BindTexture(TextureTarget.Texture2D, texture);

		gl.TexImage2D(
			TextureTarget.Texture2D,
			0,
			InternalFormat.Rgba,
			(uint)image.Width,
			(uint)image.Height,
			0,
			PixelFormat.Rgba,
			PixelType.UnsignedByte,
			image.Data
		);

		gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)GLEnum.ClampToEdge);
		gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)GLEnum.ClampToEdge);
		gl.TexParameter(
			TextureTarget.Texture2D,
			TextureParameterName.TextureMinFilter,
			(int)(mipmaps ? GLEnum.LinearMipmapLinear : GLEnum.Linear)
		);
		gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)GLEnum.Linear);

		if (mipmaps)
			gl.GenerateMipmap(TextureTarget.Texture2D);

		gl.BindTexture(TextureTarget.Texture2D, 0);

		return texture;
	}

	public static void WritePng(string filename, int width, int height, byte[] data) {
		byte* png;
		int length;

		fixed (byte* pData = data)
			png = StbImageWrite.EncodePng(pData, width, height, out length);

		File.WriteAllBytes(filename, new ReadOnlySpan<byte>(png, length));
		StbImageWrite.Free(png);
	}
}
