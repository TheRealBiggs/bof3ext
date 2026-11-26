
namespace BoF3Ext;


class Singleton<T> where T : new() {
	static T? _instance;
	public static T Instance => _instance ??= new T();
}
