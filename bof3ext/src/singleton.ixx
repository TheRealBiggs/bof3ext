module;

export module bof3ext.singleton;


export template<typename T>
class __declspec(novtable) Singleton {
public:
	static T& Get() {
		static T instance;

		return instance;
	}
};