using System;
using System.IO;
using System.Text.Json;

namespace BoF3Ext;


class ConfigManager : Singleton<ConfigManager> {
	internal class Config {
		public record struct DebugConfig(bool EnableConsole, bool DumpTextures);


		public Size Window { get; set; } = new(640, 480);
		public float Scale { get; set; } = 2f;
		public DebugConfig Debug { get; set; } = new(false, false);
	}


	public Size WindowSize {
		get => config.Window;
		set => config.Window = value;
	}

	public float RenderScale {
		get => config.Scale;
		set => config.Scale = value;
	}

	public bool EnableConsole
		=> config.Debug.EnableConsole;

#if DEBUG
	public bool DumpTextures
		=> config.Debug.DumpTextures;
#endif

	public int ScaledRenderWidth
		=> (int)Math.Ceiling(config.Window.Width / config.Scale);

	public int RenderWidthDiff
		=> ScaledRenderWidth - 320;

	public int HalfRenderWidthDiff
		=> (int)Math.Round((ScaledRenderWidth - 320) / 2.0f);


	Config config = new();


	public void Initialise() {
		try {
			ReadOnlySpan<byte> json = File.ReadAllBytes("bof3ext.json");

			if (json.StartsWith("\uFEFF"u8))
				json = json[3..];

			Utf8JsonReader reader = new(json, new JsonReaderOptions { CommentHandling = JsonCommentHandling.Skip });
			Config parsed = new();

			reader.Read();

			if (reader.TokenType != JsonTokenType.Null) {
				while (reader.Read() && reader.TokenType == JsonTokenType.PropertyName) {
					if (reader.ValueTextEquals("window"u8)) {
						reader.Read();
						parsed.Window = ReadWindow(ref reader);
					} else if (reader.ValueTextEquals("render"u8)) {
						reader.Read();
						parsed.Scale = ReadRenderScale(ref reader, parsed.Scale);
					} else if (reader.ValueTextEquals("debug"u8)) {
						reader.Read();
						parsed.Debug = ReadDebug(ref reader);
					} else {
						reader.Skip();
					}
				}
			}

			if (reader.Read())
				throw new JsonException();

			config = parsed;
		} catch (Exception e) {
			Console.WriteErrorLine($"Error reading config file: {e.Message}. Falling back to default config values.");

			config = new();
		}

		if (RenderScale == 0)
			RenderScale = config.Window.Height / 240f;
	}


	static Size ReadWindow(ref Utf8JsonReader reader) {
		int width = 0, height = 0;

		while (reader.Read() && reader.TokenType == JsonTokenType.PropertyName) {
			if (reader.ValueTextEquals("width"u8)) {
				reader.Read();
				width = reader.GetInt32();
			} else if (reader.ValueTextEquals("height"u8)) {
				reader.Read();
				height = reader.GetInt32();
			} else {
				reader.Skip();
			}
		}

		return new(width, height);
	}

	static float ReadRenderScale(ref Utf8JsonReader reader, float scale) {
		while (reader.Read() && reader.TokenType == JsonTokenType.PropertyName) {
			if (reader.ValueTextEquals("scale"u8)) {
				reader.Read();
				scale = reader.GetSingle();
			} else {
				reader.Skip();
			}
		}

		return scale;
	}

	static Config.DebugConfig ReadDebug(ref Utf8JsonReader reader) {
		bool enableConsole = false, dumpTextures = false;

		while (reader.Read() && reader.TokenType == JsonTokenType.PropertyName) {
			if (reader.ValueTextEquals("enableConsole"u8)) {
				reader.Read();
				enableConsole = reader.GetBoolean();
			} else if (reader.ValueTextEquals("dumpTextures"u8)) {
				reader.Read();
				dumpTextures = reader.GetBoolean();
			} else {
				reader.Skip();
			}
		}

		return new(enableConsole, dumpTextures);
	}
}
