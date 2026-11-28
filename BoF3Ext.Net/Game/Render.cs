using System;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.CompilerServices;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static unsafe partial class Render {
	public enum GpuPrimType : byte {
		Tri = 8,
		TriTexture,
		Quad,
		QuadTexture,
		TriGouraud,
		TriTextureGouraud,
		QuadGouraud,
		QuadTextureGouraud,
		Line,
		_UNUSED_0,
		TwoLines,
		ThreeLines,
		LineGouraud,
		_UNUSED_1,
		TwoLinesGouraud,
		ThreeLinesGouraud,
		RectWH,
		RectWHTexture,
		Point,
		TextGlyph,
		_UNUSED_2,
		Rect8Texture,
		_UNUSED_3,
		Rect16Texture,
		_UNUSED_4,
		Sprite,

		SetDrawEnv = 58,
		TPageCopy,
		SetTextureWindow,

		GlTextureFrame = 63,
	}

	public enum SetDrawEnvFlags : byte {
		AllowDraw = 0x1,
		EnableDither = 0x2,
	}


	[UnmanagedSizeProperty]
	public partial record struct PSX_RECT(short X, short Y, short W, short H);
	public record struct SVECTOR(short Vx, short Vy, short Vz, short Pad = 0);

	public struct DR_ENV {
		public uint* Tag;
		public fixed uint Code[15];
	}

	public struct DRAWENV {
		public PSX_RECT Clip;
		public ushort OffsetX;
		public ushort OffsetY;
		public PSX_RECT TextureWindow;
		public ushort TPage;
		public bool Dtd;
		public bool Dfe;
		public bool IsBg;
		public byte R;
		public byte G;
		public byte B;
		public DR_ENV DrEnv;
	}

	public struct DISPENV {
		public PSX_RECT Disp;
		public PSX_RECT Screen;
		public bool IsInter;
		public bool IsRgb24;
		byte pad0;
		byte pad1;
	}

	public struct ENV {
		public DISPENV Disp;
		public DRAWENV Draw;
		public Array8<nint> OrderingTable;
	}

	public struct TextureBlockInfo {
		public short RelX;
		public sbyte RelY;
		public byte Size;
		public ushort TPage;
		public byte Tx;
		public byte Ty;
	}

	public struct GpuPrim {
		public void* Tag;
		public Vec3b Colour1;
		public byte Value;

		public readonly bool Abe
			=> (Value & 2) != 0;

		[UnscopedRef]
		public ref T As<T>() where T : unmanaged
			=> ref Unsafe.As<GpuPrim, T>(ref this);
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_Tri {
		GpuPrim @base;

		public Vec3f Vert1;
		public Vec3f Vert2;
		public Vec3f Vert3;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_TriTexture {
		GpuPrim @base;

		public Vec3f Vert1;
		public Vec2b Uv1;
		public ushort Palette;
		public Vec3f Vert2;
		public Vec2b Uv2;
		public ushort TPage;
		public Vec3f Vert3;
		public Vec2b Uv3;
		readonly ushort _unused_0;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_Quad {
		GpuPrim_Tri @base;

		public Vec3f Vert4;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_QuadTexture {
		GpuPrim_TriTexture @base;

		public Vec3f Vert4;
		public Vec2b Uv4;
		readonly ushort _unused_1;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_TriGouraud {
		GpuPrim @base;

		public Vec3f Vert1;
		public Vec3b Colour2;
		public Vec3f Vert2;
		public Vec3b Colour3;
		public Vec3f Vert3;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_TriTextureGouraud {
		GpuPrim @base;

		public Vec3f Vert1;
		public Vec2b Uv1;
		public ushort Palette;
		public Vec3b Colour2;
		public Vec3f Vert2;
		public Vec2b Uv2;
		public ushort TPage;
		public Vec3b Colour3;
		public Vec3f Vert3;
		public Vec2b Uv3;
		readonly ushort _unused_0;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_QuadGouraud {
		GpuPrim_TriGouraud @base;

		public Vec3b Colour4;
		public Vec3f Vert4;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_QuadTextureGouraud {
		GpuPrim_TriTextureGouraud @base;

		public Vec3b Colour4;
		public Vec3f Vert4;
		public Vec2b Uv4;
		readonly ushort _unused_1;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_Line {
		GpuPrim @base;

		public Vec3f Vert1;
		public Vec3f Vert2;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_TwoLines {
		GpuPrim_Line @base;

		public Vec3f Vert3;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_ThreeLines {
		GpuPrim_TwoLines @base;

		public Vec3f Vert4;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_LineGouraud {
		GpuPrim @base;

		public Vec3f Vert1;
		public Vec3b Colour2;
		public Vec3f Vert2;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_TwoLinesGouraud {
		GpuPrim_LineGouraud @base;

		public Vec3b Colour3;
		public Vec3f Vert3;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_ThreeLinesGouraud {
		GpuPrim_TwoLinesGouraud @base;

		public Vec3b Colour4;
		public Vec3f Vert4;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_Point {
		GpuPrim @base;

		public Vec3f Vert1;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_Rect8 {
		GpuPrim @base;

		public Vec3f Vert1;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_Rect16 {
		GpuPrim_Rect8 @base;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_RectWH {
		GpuPrim_Rect8 @base;

		public Vec2f Size;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_Rect8Texture {
		GpuPrim_Rect8 @base;

		public Vec2b Uv1;
		public ushort Palette;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_Rect16Texture {
		GpuPrim_Rect8Texture @base;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_RectWHTexture {
		GpuPrim_Rect8Texture @base;

		public Vec2us Size;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_TextGlyph {
		GpuPrim @base;

		public Vec2s Vert1;
		public Vec2b Uv1;
		public ushort PaletteIndex;
		public Vec2s Vert2;
		public Vec2b Uv2;
		public ushort CharCode;
		public Vec2s Vert3;
		public Vec2b Uv3;
		readonly ushort _unused_0;
		public Vec2s Vert4;
		public Vec2b Uv4;
		readonly ushort _unused_1;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_Sprite {
		GpuPrim @base;

		public Vec2f Position;
		public Vec2f Scale;
		public ushort BlockIndex;
		public ushort BlockCount;
		public ushort Palette;
		public ushort TPage;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_TPageCopy {
		GpuPrim @base;

		public PSX_RECT SrcRect;
		public uint DstX;
		public uint DstY;
	}

	[CompositeStruct(nameof(@base))]
	[UnmanagedSizeProperty]
	public partial struct GpuPrim_GlTextureFrame {
		GpuPrim @base;

		public Vec3f Vert1;
		public Vec2f Size;
		public uint TextureId;
		public Vec4f SrcRect;
	}

	[UnmanagedSizeProperty]
	public partial struct GpuPrim_SetDrawEnv {
		fixed byte gap0[4];
		public ushort TPage;
		public SetDrawEnvFlags Flags;
		public byte Command;
		public PSX_RECT* rect;
	}


	public struct SkyRenderCommand {
		public uint Command;
		public Vec4b Bounds;
		public uint Colours;
		public uint AlternateColours;
	}

	[GameFunction(0x404560)]
	public static partial void DrawOverworldUiTextureFrame(short x, short y, byte index);

	[GameFunction(0x444340, hookable: true)]
	public static partial void DrawNumTiny(short x, short y, byte paletteIndex, ushort value);

	[GameFunction(0x454960)]
	public static partial void LoadPalettes();

	[GameFunction(0x461E50)]
	public static partial void PushGpuPrim(byte index, byte size);

	[GameFunction(0x461F00)]
	public static partial void LoadTextures();

	[GameFunction(0x462560, hookable: true)]
	public static partial nint DrawTitleDemoTextureFrame(
		short x, short y, byte frameIndex, byte drawIndex, byte translucent
	);

	[GameFunction(0x4FD230)]
	public static partial void ResetGpuPrimBuffers();

	[GameFunction(0x4FD290)]
	public static partial void LinkGpuPrimBuffers();

	[GameFunction(0x516B30, hookable: true)]
	public static partial nint DrawStringNormal(short x, short y, uint paletteIndex, byte len, GameString text);

	[GameFunction(0x516D50)]
	public static partial void DrawGlyph(
		short x, short y, short width, short height, short tx, sbyte ty, ushort paletteIndex
	);

	[GameFunction(0x516E70, hookable: true)]
	public static partial nint DrawStringSmall(short x, short y, byte paletteIndex, byte len, GameString text);

	[GameFunction(0x516F60, hookable: true)]
	public static partial void DrawStringLarge(short x, short y, byte paletteIndex, GameString text);

	[GameFunction(0x517090, hookable: true)]
	public static partial void DrawStringNumFont(short x, short y, uint paletteIndex, GameString text);

	[GameFunction(0x571BE0, hookable: true)]
	public static partial void DrawSky(ref SkyRenderCommand command);

	[GameFunction(0x588C90, hookable: true)]
	public static partial void DrawStartMenuLogo(short x, short y, byte spriteIndex, byte drawIndex);

	[GameFunction(0x5935B0, hookable: true)]
	public static partial void DrawCurrentActorSprite();

	[GameFunction(0x59EA70, hookable: true)]
	public static partial void LoadImage2(nint rect, nint data);

	[GameFunction(0x59EB00, hookable: true)]
	public static partial void LoadImage(nint rect, nint data);

	[GameFunction(0x5A7710)]
	public static partial void InitGpuPrim_RectWHTexture(ref GpuPrim_RectWHTexture prim);

	[GameFunction(0x5A77C0)]
	public static partial void InitGpuPrim_SetDrawEnv(
		ref GpuPrim_SetDrawEnv prim, int allowDraw, int enableDither, ushort tpage, ref PSX_RECT rect
	);

	[GameFunction(0x5A7960)]
	public static partial void ClearOTagR(nint orderingTable, int count);

	[GameFunction(0x5A79A0)]
	public static partial ushort GetTPage(byte tp, byte abr, int tx, int ty);

	[GameFunction(0x5A79E0)]
	public static partial ushort CreatePaletteIndex(ushort x, ushort y);

	[GameFunction(0x5A7AA0)]
	public static partial void InitGeom();

	[GameFunction(0x5A7AE0, hookable: true)]
	public static partial void SetGeomOffset(int x, int y);

	[GameFunction(0x5A7B00)]
	public static partial void SetGeomScreen(int distance);


	[GameVariable(0x6BEA18, arrayLength: 2048)]
	public static partial Span<TextureBlockInfo> TextureBlockInfos { get; set; }

	[GameVariable(0x7CC374)]
	public static partial int TextureBlockInfoCount { get; set; }

	[GameVariable(0x7DE790)]
	public static partial int GeomOffsetX { get; set; }

	[GameVariable(0x7DECE0)]
	public static partial ref DISPENV DispEnv { get; }

	[GameVariable(0x7DED00)]
	public static partial ref DRAWENV DrawEnv { get; }

	[GameVariable(0x7E0670, isPointer: true)]
	public static partial ref GpuPrim GpuPrims { get; }

	[GameVariable(0x7E1BE2)]
	public static partial bool UseAlternateColours { get; set; }

	[GameVariable(0x903880, arrayLength: 2)]
	public static partial Span<ENV> Envs { get; set; }

	[GameVariable(0x905B89)]
	public static partial byte CurrentEnvIndex { get; set; }

	[GameVariable(0x905E69)]
	public static partial byte TerrainRefreshCounter { get; set; }

	[GameVariable(0x937F84, isPointer: true)]
	public static partial ref ENV CurrentEnv { get; }
}
