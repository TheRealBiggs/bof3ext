using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text.Json;

using Silk.NET.OpenGL;

namespace BoF3Ext;


record class AtlasTexture(int Width, int Height, uint Id);
record struct ReplacementTexture(AtlasTexture? Atlas, ushort X, ushort Y, ushort Width, ushort Height);
record struct ReplacementTextureIndexEntry(string Texture, ushort X, ushort Y, ushort Width, ushort Height);

record struct ReplacementTextureKey {
	public ulong StartHash;
	public ulong EndHash;
	public ulong MiddleHash;
	public ushort Width;
	public ushort Height;
	public byte Bpp;


	public ReplacementTextureKey(ulong startHash, ulong endHash, ulong middleHash, ushort width, ushort height, byte bpp) {
		StartHash = startHash;
		EndHash = endHash;
		MiddleHash = middleHash;
		Width = width;
		Height = height;
		Bpp = bpp;
	}

	public override int GetHashCode()
		=> (int)StartHash;
}


class TextureReplacer : Singleton<TextureReplacer>, IDisposable {
	GL? gl;
	byte[] tpages = [];
	readonly Dictionary<ReplacementTextureKey, ReplacementTextureIndexEntry> indexEntries = [];
	readonly HashSet<ReplacementTextureKey> unreplacedTextureKeys = [];
	readonly Dictionary<string, AtlasTexture?> atlasTextures = [];
	readonly Dictionary<ReplacementTextureKey, ReplacementTexture> replacementTextures = [];

#if DEBUG
	readonly HashSet<ReplacementTextureKey> dumpedTextureKeys = [];
#endif


	public void Initialise(GL gl, byte[] tpages) {
		this.gl = gl;
		this.tpages = tpages;

		if (!File.Exists("NewData/Textures/index.json"))
			return;

		ReadOnlySpan<byte> json = File.ReadAllBytes("NewData/Textures/index.json");

		if (json.StartsWith("\uFEFF"u8))
			json = json[3..];

		Utf8JsonReader reader = new(json, new JsonReaderOptions { CommentHandling = JsonCommentHandling.Skip });
		Span<ushort> rect = stackalloc ushort[4];

		reader.Read();

		while (reader.Read() && reader.TokenType == JsonTokenType.StartObject) {
			string? id = null, texture = null;
			var hasRect = false;

			while (reader.Read() && reader.TokenType == JsonTokenType.PropertyName) {
				if (reader.ValueTextEquals("id"u8)) {
					reader.Read();
					id = reader.GetString();
				} else if (reader.ValueTextEquals("texture"u8)) {
					reader.Read();
					texture = reader.GetString();
				} else if (reader.ValueTextEquals("rect"u8)) {
					reader.Read();

					for (int i = 0; i < rect.Length; i++) {
						reader.Read();
						rect[i] = reader.GetUInt16();
					}

					while (reader.Read() && reader.TokenType != JsonTokenType.EndArray)
						reader.Skip();

					hasRect = true;
				} else {
					reader.Skip();
				}
			}

			if (id == null || texture == null || !hasRect)
				throw new JsonException();

			indexEntries[ParseTextureId(id)] = new(texture, rect[0], rect[1], rect[2], rect[3]);
		}

		if (reader.Read())
			throw new JsonException();
	}

	public ReplacementTexture GetReplacementTexture(int x, int y, int w, int h, ushort tpage, ushort palette) {
		var key = CreateTextureKey(x, y, w, h, tpage, palette);

		if (replacementTextures.TryGetValue(key, out var texture))
			return texture;

		if (unreplacedTextureKeys.Contains(key))
			return default;

		if (!indexEntries.TryGetValue(key, out var entry)) {
			unreplacedTextureKeys.Add(key);

			return default;
		}

		if (!atlasTextures.TryGetValue(entry.Texture, out var atlas)) {
			atlas = LoadTexture(entry.Texture);
			atlasTextures[entry.Texture] = atlas;
		}

		texture = new(atlas, entry.X, entry.Y, entry.Width, entry.Height);
		replacementTextures[key] = texture;

		return texture;
	}

#if DEBUG
	public bool DumpFrame(int tx, int ty, int w, int h, ushort tpage, ushort palette) {
		if ((w == 15 && h == 15) || (w == 12 && h == 12) || (w == 10 && h == 10))
			return false;

		if (w <= 0 || h <= 0)
			return false;

		var depth = (tpage >> 7) & 3;

		// The shader only samples indexed textures.
		if (depth != 0 && depth != 1)
			return false;

		var key = CreateTextureKey(tx, ty, w, h, tpage, palette);

		if (dumpedTextureKeys.Contains(key))
			return false;

		var filename = $"{key.StartHash:x16}_{key.MiddleHash:x16}_{key.EndHash:x16}_{key.Width}x{key.Height}_{key.Bpp}";
		var filepath = $"NewData/Textures/dumped/{filename}.png";

		if (File.Exists(filepath)) {
			dumpedTextureKeys.Add(key);

			return false;
		}

		var pageX = (tpage & 0xF) * 64;
		var pageY = ((tpage >> 4) & 1) * 256;

		var clutX = (palette & 0x3F) * 16;
		var clutY = palette >> 6;

		var rgba = new byte[w * h * 4];

		for (var y = 0; y < h; ++y) {
			for (var x = 0; x < w; ++x) {
				var u = tx + x;
				var v = ty + y;

				var sourceY = pageY + v;
				var index = depth == 0
					? (GetTPagesByte(pageX * 2 + u / 2, sourceY) >> ((u & 1) * 4)) & 0xF
					: GetTPagesByte(pageX * 2 + u, sourceY);

				var c = GetTPagesByte(clutX * 2 + index * 2, clutY)
					| (GetTPagesByte(clutX * 2 + index * 2 + 1, clutY) << 8);

				var r5 = c & 0x1F;
				var g5 = (c >> 5) & 0x1F;
				var b5 = (c >> 10) & 0x1F;

				// Match shader normalization: channel / 31.0.
				var dst = (y * w + x) * 4;
				rgba[dst] = (byte)((r5 * 255 + 15) / 31);
				rgba[dst + 1] = (byte)((g5 * 255 + 15) / 31);
				rgba[dst + 2] = (byte)((b5 * 255 + 15) / 31);
				rgba[dst + 3] = (byte)(r5 == 0 && g5 == 0 && b5 == 0 ? 0 : 255);
			}
		}

		try {
			Directory.CreateDirectory("NewData/Textures/dumped");
			TextureImage.WritePng(filepath, w, h, rgba);
			dumpedTextureKeys.Add(key);

			return true;
		} catch (IOException) {
			return false;
		} catch (UnauthorizedAccessException) {
			return false;
		}
	}
#endif

	public void Dispose() {
		foreach (var atlas in atlasTextures.Values) {
			if (atlas != null)
				gl!.DeleteTexture(atlas.Id);
		}

		atlasTextures.Clear();
		indexEntries.Clear();
		replacementTextures.Clear();
		unreplacedTextureKeys.Clear();

#if DEBUG
		dumpedTextureKeys.Clear();
#endif
	}


	AtlasTexture? LoadTexture(string filename) {
		TextureImage image;

		try {
			image = TextureImage.Load($"NewData/Textures/{filename}");
		} catch (Exception e) {
			Console.WriteErrorLine($"Failed to load replacement texture '{filename}': {e.Message}");

			return null;
		}

		var id = gl!.GenTexture();
		gl.BindTexture(TextureTarget.Texture2D, id);

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
		gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)GLEnum.LinearMipmapLinear);
		gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)GLEnum.Linear);
		gl.GenerateMipmap(TextureTarget.Texture2D);
		gl.BindTexture(TextureTarget.Texture2D, 0);

		return new(image.Width, image.Height, id);
	}

#if DEBUG
	byte GetTPagesByte(int x, int y)
		=> tpages[(y & 511) * 2048 + (x & 2047)];
#endif

	static ReplacementTextureKey ParseTextureId(string id) {
		var parts = id.Split('_');
		var dimensions = parts[3].Split('x');

		return new(
			UInt64.Parse(parts[0], NumberStyles.HexNumber, CultureInfo.InvariantCulture),
			UInt64.Parse(parts[2], NumberStyles.HexNumber, CultureInfo.InvariantCulture),
			UInt64.Parse(parts[1], NumberStyles.HexNumber, CultureInfo.InvariantCulture),
			UInt16.Parse(dimensions[0], CultureInfo.InvariantCulture),
			UInt16.Parse(dimensions[1], CultureInfo.InvariantCulture),
			Byte.Parse(parts[4], CultureInfo.InvariantCulture)
		);
	}

	unsafe ReplacementTextureKey CreateTextureKey(int tx, int ty, int w, int h, ushort tpage, ushort palette) {
		if (w <= 0 || w > UInt16.MaxValue || h <= 0 || h > UInt16.MaxValue)
			return default;

		var depth = (tpage >> 7) & 3;

		int bpp = depth switch {
			0 => 4,
			1 => 8,
			2 => 15,
			_ => 0
		};

		var pageXBytes = ((tpage & 0xF) * 64) * 2;
		var pageY = ((tpage >> 4) & 1) * 256;
		var clutX = (palette & 0x3F) * 16;
		var clutY = (palette >> 6) & 0x1FF;
		var bytesPerPixel = bpp == 15 ? 2 : 1;
		var bytesPerRow = bpp == 4 ? (w + 1) / 2 : w * bytesPerPixel;
		var sampleBytes = Math.Min(8, bytesPerRow);
		var byteX = pageXBytes + (bpp == 4 ? tx / 2 : tx * bytesPerPixel);

		ReplacementTextureKey ti = new();

		Unsafe.InitBlockUnaligned(&ti, 0, (uint)Unsafe.SizeOf<ReplacementTextureKey>());

		ti.Width = (ushort)w;
		ti.Height = (ushort)h;
		ti.Bpp = (byte)bpp;

		var offset = Unsafe.ByteOffset(
			ref Unsafe.As<ReplacementTextureKey, byte>(ref ti),
			ref Unsafe.As<ushort, byte>(ref ti.Width)
		);

		Fnv startHash = new();
		startHash.Append(new ReadOnlySpan<byte>(&ti.Width, Unsafe.SizeOf<ReplacementTextureKey>() - (int)offset));

		var middleHash = startHash;
		var endHash = startHash;

		// Hash up to 8 bytes from the start of the first row.
		startHash.Append(
			tpages.AsSpan(
				((pageY + ty) & (512 - 1)) * 2048 + (byteX & (2048 - 1)),
				sampleBytes
			)
		);

		// Hash up to 8 bytes from the center of the middle row.
		middleHash.Append(
			tpages.AsSpan(
				((pageY + ty + h / 2) & (512 - 1)) * 2048 + ((byteX + (bytesPerRow - sampleBytes) / 2) & (2048 - 1)),
				sampleBytes
			)
		);

		// Hash up to 8 bytes from the end of the last row.
		endHash.Append(
			tpages.AsSpan(
				((pageY + ty + h - 1) & (512 - 1)) * 2048 + ((byteX + bytesPerRow - sampleBytes) & (2048 - 1)),
				sampleBytes
			)
		);

		// Hash 8 bytes from each end of the palette.
		if (bpp != 15) {
			var paletteEntries = bpp == 4 ? 16 : 256;

			startHash.Append(
				tpages.AsSpan(
					clutY * 2048 + ((clutX * 2) & (2048 - 1)),
					8
				)
			);
			endHash.Append(
				tpages.AsSpan(
					clutY * 2048 + (((clutX + paletteEntries - 4) * 2) & (2048 - 1)),
					8
				)
			);
		}

		ti.StartHash = startHash.GetHash();
		ti.MiddleHash = middleHash.GetHash();
		ti.EndHash = endHash.GetHash();

		return ti;
	}
}
