module;

#include <toml++/toml.hpp>

export module bof3ext.configManager;

import bof3ext.helpers;
import bof3ext.math;
import bof3ext.singleton;


export class ConfigManager : public Singleton<ConfigManager> {
public:
	void Initialise() {
		auto cfg = toml::parse_file("bof3ext.toml");

		windowWidth = cfg["window"]["width"].value_or(640);
		windowHeight = cfg["window"]["height"].value_or(480);

		renderScale = cfg["render"]["scale"].value_or(0.f);

		if (renderScale == 0.f)
			renderScale = windowHeight / 240.f;

#if ENABLE_LOGGING
		enableConsole = cfg["debug"]["enable_console"].value_or(true);
#endif

		dumpTextures = cfg["debug"]["dump_textures"].value_or(false);
	}


	const Vec2i GetWindowSize() const { return { windowWidth, windowHeight }; }
	float GetRenderScale() const { return renderScale; }
	bool GetEnableConsole() const { return enableConsole; }
	bool GetDumpTextures() const { return dumpTextures; }

	void SetWindowSize(int width, int height) { windowWidth = width; windowHeight = height; }
	void SetRenderScale(float scale) { renderScale = scale; }


private:
	int windowWidth, windowHeight;
	float renderScale;
	bool enableConsole;
	bool dumpTextures;
};