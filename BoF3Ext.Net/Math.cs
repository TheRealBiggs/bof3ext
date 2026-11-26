
using BoF3Ext.Generators;

namespace BoF3Ext;


[AdditionOperator] [ZeroProperty] public partial record struct Size(int Width, int Height);
[AdditionOperator] [ZeroProperty] public partial record struct Vec3b(byte X, byte Y, byte Z);
[AdditionOperator] [ZeroProperty] public partial record struct Vec2b(byte X, byte Y);
[AdditionOperator] [ZeroProperty] public partial record struct Vec2s(short X, short Y);
[AdditionOperator] [ZeroProperty] public partial record struct Vec2us(ushort X, ushort Y);
[AdditionOperator] [ZeroProperty] public partial record struct Vec2i(int X, int Y);
[AdditionOperator] [ZeroProperty] public partial record struct Vec2f(float X, float Y);
[AdditionOperator] [ZeroProperty] public partial record struct Vec3f(float X, float Y, float Z);
[AdditionOperator] [ZeroProperty] public partial record struct Vec4f(float X, float Y, float Z, float W);
[AdditionOperator] [ZeroProperty] public partial record struct Vec4s(short X, short Y, short Z, short W);
[AdditionOperator] [ZeroProperty] public partial record struct Vec4b(byte X, byte Y, byte Z, byte W);