using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Wrappers;


static unsafe partial class FreeType {
	public struct Generic {
		public void* Data;
		public delegate* unmanaged[Cdecl]<void*, void> Finalizer;
	}

	public struct Vector {
		public int X;
		public int Y;
	}

	public struct BBox {
		public int XMin;
		public int YMin;
		public int XMax;
		public int YMax;
	}

	public struct BitmapSize {
		public short Height;
		public short Width;
		public int Size;
		public int XPpem;
		public int YPpem;
	}

	public struct CharMap {
		public Face* Face;
		public uint Encoding;
		public ushort PlatformId;
		public ushort EncodingId;
	}

	public struct ListNodeRec {
		public ListNodeRec* Prev;
		public ListNodeRec* Next;
		public void* Data;
	}

	public struct ListRec {
		public ListNodeRec* Head;
		public ListNodeRec* Tail;
	}

	public struct Face {
		public int NumFaces;
		public int FaceIndex;
		public int FaceFlags;
		public int StyleFlags;
		public int NumGlyphs;
		public sbyte* FamilyName;
		public sbyte* StyleName;
		public int NumFixedSizes;
		public BitmapSize* AvailableSizes;
		public int NumCharmaps;
		public CharMap** Charmaps;
		public Generic Generic;
		public BBox Bbox;
		public ushort UnitsPerEM;
		public short Ascender;
		public short Descender;
		public short Height;
		public short MaxAdvanceWidth;
		public short MaxAdvanceHeight;
		public short UnderlinePosition;
		public short UnderlineThickness;
		public GlyphSlot* Glyph;
		public void* Size;
		public CharMap* Charmap;
		public void* Driver;
		public void* Memory;
		public void* Stream;
		public ListRec SizesList;
		public Generic Autohint;
		public void* Extensions;
		public void* Internal;
	}

	public struct GlyphMetrics {
		public int Width;
		public int Height;
		public int HoriBearingX;
		public int HoriBearingY;
		public int HoriAdvance;
		public int VertBearingX;
		public int VertBearingY;
		public int VertAdvance;
	}

	public struct Bitmap {
		public uint Rows;
		public uint Width;
		public int Pitch;
		public byte* Buffer;
		public ushort NumGrays;
		public byte PixelMode;
		public byte PaletteMode;
		public void* Palette;
	}

	public struct Outline {
		public ushort NContours;
		public ushort NPoints;
		public Vector* Points;
		public byte* Tags;
		public ushort* Contours;
		public int Flags;
	}

	public struct GlyphSlot {
		public nint Library;
		public Face* Face;
		public GlyphSlot* Next;
		public uint GlyphIndex;
		public Generic Generic;
		public GlyphMetrics Metrics;
		public int LinearHoriAdvance;
		public int LinearVertAdvance;
		public Vector Advance;
		public uint Format;
		public Bitmap Bitmap;
		public int BitmapLeft;
		public int BitmapTop;
		public Outline Outline;
		public uint NumSubglyphs;
		public void* Subglyphs;
		public void* ControlData;
		public int ControlLen;
		public int LsbDelta;
		public int RsbDelta;
		public void* Other;
		public void* Internal;
	}


	public const int LOAD_NO_HINTING = 1 << 1;
	public const int LOAD_NO_BITMAP = 1 << 3;
	public const uint ENCODING_UNICODE = 0x756E6963;
	public const int RENDER_MODE_SDF = 5;


	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int FT_Init_FreeType(out nint library);

	[LibraryImport("freetype", StringMarshalling = StringMarshalling.Utf8)]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int FT_New_Face(nint library, string filename, int faceIndex, out Face* face);

	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int FT_Set_Pixel_Sizes(Face* face, uint width, uint height);

	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int FT_Select_Charmap(Face* face, uint encoding);

	[LibraryImport("freetype", StringMarshalling = StringMarshalling.Utf8)]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int FT_Property_Set(nint library, string module, string property, void* value);

	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial uint FT_Get_Char_Index(Face* face, uint charCode);

	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial uint FT_Get_First_Char(Face* face, out uint glyphIndex);

	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial uint FT_Get_Next_Char(Face* face, uint charCode, out uint glyphIndex);

	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int FT_Load_Glyph(Face* face, uint glyphIndex, int flags);

	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int FT_Render_Glyph(GlyphSlot* slot, int renderMode);

	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int FT_Done_Face(Face* face);

	[LibraryImport("freetype")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int FT_Done_FreeType(nint library);
}
