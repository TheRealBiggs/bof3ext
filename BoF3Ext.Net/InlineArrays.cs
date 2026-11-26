using System.Runtime.CompilerServices;

namespace BoF3Ext;


[InlineArray(4)]
struct Array4<T> where T : unmanaged {
	public T Value;
}

[InlineArray(8)]
struct Array8<T> where T : unmanaged {
	public T Value;
}

[InlineArray(9)]
struct Array9<T> where T : unmanaged {
	public T Value;
}

[InlineArray(10)]
struct Array10<T> where T : unmanaged {
	public T Value;
}

[InlineArray(12)]
struct Array12<T> where T : unmanaged {
	public T Value;
}

[InlineArray(16)]
struct Array16<T> where T : unmanaged {
	public T Value;
}

[InlineArray(32)]
struct Array32<T> where T : unmanaged {
	public T Value;
}

[InlineArray(256)]
struct Array256<T> where T : unmanaged {
	public T Value;
}