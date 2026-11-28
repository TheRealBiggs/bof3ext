using System;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

using Silk.NET.Core.Loader;
using Silk.NET.Input;
using Silk.NET.Input.Glfw;
using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using Silk.NET.Windowing.Glfw;

using SilkWindow = Silk.NET.Windowing.Window;

namespace BoF3Ext.Hooks;


static class Window {
	enum MessageColour {
		White,
		PeriwinkleBlue,
		LightRed,
		SkyBlue,
		PineappleYellow,
		Pink,
		Green,
		StoneGrey,
		Gainsboro,
		SlateBlue = 10,
		LightGrey,
		ChartreuseYellow,
		LightBlue,
		Lead,
		Black
	}


	static IWindow window = null!;
	static IInputContext? input;
	static GL? gl;
	static int messageTimer = 0;
	static unsafe byte* messageBuffer = null;
	static MessageColour messageColour;


	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvStdcall)])]
	static int WinMainHook(nint hInstance, nint hPrevInstance, nint lpCmdLine, int nShowCmd) {
		var windowSize = ConfigManager.Instance.WindowSize;

		var options = WindowOptions.Default with {
			Size = new(windowSize.Width, windowSize.Height),
			Title = "Breath of Fire III - OpenGL Renderer",
			FramesPerSecond = 30,
			UpdatesPerSecond = 1,
			Samples = 0,
			API = GraphicsAPI.Default with {
				Version = new(4, 3)
			},
			ShouldSwapAutomatically = false,
			VSync = false
		};

		SilkWindow.ShouldLoadFirstPartyPlatforms(false);
		InputWindowExtensions.ShouldLoadFirstPartyPlatforms(false);

		var resolver = (DefaultPathResolver)PathResolver.Default;

		resolver.Resolvers.Clear();
		resolver.Resolvers.Add(DefaultPathResolver.PassthroughResolver);
		resolver.Resolvers.Add(DefaultPathResolver.BaseDirectoryResolver);

		GlfwWindowing.RegisterPlatform();
		GlfwInput.RegisterPlatform();

		window = SilkWindow.Create(options);

		window.Load += OnLoad;
		window.Render += OnRender;
		window.FramebufferResize += OnFramebufferResize;
		window.FocusChanged += InputManager.Instance.SetFocused;

		window.Run();

		input?.Dispose();
		gl?.Dispose();
		window.Dispose();

		return 0;
	}


	static unsafe void OnLoad() {
		gl = GL.GetApi(window);
		input = window.CreateInput();
		input.Keyboards[0].KeyDown += OnKeyboardKeyDown;

		var inputMgr = InputManager.Instance;

		inputMgr.Initialise(input);
		inputMgr.SetFocused(true);

		var renderer = Renderer.Instance;

		if (!renderer.Initialise(gl)) {
			window.Close();

			return;
		}

		renderer.SetViewport(0, 0, window.FramebufferSize.X, window.FramebufferSize.Y);

		if (!GlyphManager.Instance.GenerateCache(gl)) {
			window.Close();

			return;
		}

		Game.Render.InitGeom();
		Game.Render.SetGeomOffset(160, 120);
		Game.Render.SetGeomScreen(1000);

		Game.Audio.InitialiseAudio(window.Native!.Win32!.Value.Hwnd);

		var envs = Game.Render.Envs;

		SetDefDrawEnv(ref envs[0].Draw, 0, 0, 320, 240);
		SetDefDrawEnv(ref envs[1].Draw, 0, 240, 320, 240);

		Game.Render.ClearOTagR((nint)Unsafe.AsPointer(ref envs[0].OrderingTable), 8);
		envs[0].Draw.IsBg = true;
		envs[0].Draw.R = envs[0].Draw.G = envs[0].Draw.B = 0;

		Game.Render.ClearOTagR((nint)Unsafe.AsPointer(ref envs[1].OrderingTable), 8);
		envs[1].Draw.IsBg = true;
		envs[1].Draw.R = envs[1].Draw.G = envs[1].Draw.B = 0;

		Game.Render.CurrentEnvIndex = 0;
		Game.Render.CurrentEnvPointer = (nint)Unsafe.AsPointer(ref envs[0]);

		Game.Render.ResetGpuPrimBuffers();

		Game.World.InitialiseModeStack();
		Game.World.SetGameMode(0, Game.World.GAMEMODE_FIRST);
	}

	static unsafe void OnRender(double delta) {
		UpdateInput();

		var renderer = Renderer.Instance;

		renderer.Begin();
		{
			ref var env = ref Game.Render.CurrentEnv;

			PutDrawEnv(ref env.Draw);

			Game.Render.LoadPalettes();
			Game.Render.LoadTextures();

			DrawOTag(ref env.OrderingTable[7]);
		}
		renderer.End();

		window.GLContext!.SwapBuffers();

		Game.Audio.UpdateAudio();

		Game.Render.CurrentEnvIndex ^= 1;

		ref var nextEnv = ref Game.Render.Envs[Game.Render.CurrentEnvIndex];

		Game.Render.CurrentEnvPointer = (nint)Unsafe.AsPointer(ref nextEnv);

		Game.Render.ClearOTagR((nint)Unsafe.AsPointer(ref nextEnv.OrderingTable), 8);
		Game.Render.ResetGpuPrimBuffers();
		Game.Render.TextureBlockInfoCount = 0;

		Game.World.UpdateScheduler();

		if (messageTimer > 0) {
			Game.Render.DrawStringSmall(
				(short)(4 - ConfigManager.Instance.HalfRenderWidthDiff),
				4,
				(byte)messageColour,
				255,
				new GameString(messageBuffer)
			);

			messageTimer--;
		} else if (messageBuffer != null) {
			NativeMemory.Free(messageBuffer);
			messageBuffer = null;
		}

		Game.Render.LinkGpuPrimBuffers();

		Game.World.FrameCount++;
	}

	static void OnFramebufferResize(Vector2D<int> size) {
		if (size == Vector2D<int>.Zero)
			return;

		var cfgMgr = ConfigManager.Instance;

		var scale = size.Y / 240.0f;

		cfgMgr.WindowSize = new(size.X, size.Y);
		cfgMgr.RenderScale = scale;

		Patches.Widescreen.Update();

		Game.Render.TerrainRefreshCounter = 3;

		Renderer.Instance.SetViewport(0, 0, size.X, size.Y);
	}

	static void OnKeyboardKeyDown(IKeyboard keyboard, Key key, int keyCode) {
		if (key == Key.Enter && (keyboard.IsKeyPressed(Key.AltLeft) || keyboard.IsKeyPressed(Key.AltRight))) {
			window.WindowState = window.WindowState == WindowState.Fullscreen
				? WindowState.Normal
				: WindowState.Fullscreen;
		}
		else if (key == Key.F10) {
			var size = window.FramebufferSize;
			var pixels = new byte[size.X * size.Y * 4];

			gl!.ReadBuffer(ReadBufferMode.Front);
			gl!.ReadPixels(0, 0, (uint)size.X, (uint)size.Y, PixelFormat.Rgba, PixelType.UnsignedByte, pixels.AsSpan());

			var stride = size.X * 4;
			byte[] row = new byte[stride];

			for (int y = 0; y < size.Y / 2; y++) {
				var top = pixels.AsSpan(y * stride, stride);
				var bottom = pixels.AsSpan((size.Y - y - 1) * stride, stride);

				top.CopyTo(row);
				bottom.CopyTo(top);
				row.AsSpan().CopyTo(bottom);
			}

			// Remove alpha
			for (int i = 3; i < pixels.Length; i += 4)
				pixels[i] = 255;

			if (!Directory.Exists("Screenshots"))
				Directory.CreateDirectory("Screenshots");

			var filename = $"Screenshots/{DateTime.Now:yyyy-MM-dd_HHmmss}.png";

			TextureImage.WritePng(filename, size.X, size.Y, pixels);

			DisplayTimedMessage($"Screenshot saved as {filename}", 30 * 5, MessageColour.ChartreuseYellow);
		}
	}


	static void SetDefDrawEnv(ref Game.Render.DRAWENV draw, short x, short y, short width, short height) {
		draw.Clip.X = x;
		draw.Clip.Y = y;
		draw.Clip.W = width;
		draw.Clip.H = height;

		draw.Dtd = true;
		draw.Dfe = true;

		draw.IsBg = false;
		draw.R = draw.G = draw.B = 0;

		draw.TextureWindow = new(0, 0, 256, 256);
	}

	static void UpdateInput() {
		var mappings = Game.Input.CurrentButtonMappings;
		mappings[(int)Game.Input.ButtonMappingId.Confirm] = mappings[(int)Game.Input.ButtonMappingId.Speak];
		mappings[(int)Game.Input.ButtonMappingId.Cancel] = mappings[(int)Game.Input.ButtonMappingId.Action];

		var inputMgr = InputManager.Instance;
		inputMgr.Update();

		Game.Input.HeldButtons = inputMgr.HeldButtons;
		Game.Input.PressedButtons = inputMgr.PressedButtons;
	}

	static void PutDrawEnv(ref Game.Render.DRAWENV draw) {
		ref var currentDraw = ref Game.Render.DrawEnv;

		if (currentDraw.R != draw.R || currentDraw.G != draw.G || currentDraw.B != draw.B)
			gl!.ClearColor(draw.R / 255.0f, draw.G / 255.0f, draw.B / 255.0f, 1);

		currentDraw = draw;
	}

	static unsafe void DrawOTag(ref nint orderingTable) {
		ref var env = ref Game.Render.DrawEnv;

		if (!env.Dfe)
			return;

		var renderer = Renderer.Instance;
		var address = orderingTable;

		while (address != -1) {
			var terrain = TerrainManager.Instance.IsTerrainPrim((nuint)address);

			if (!terrain && ((nuint)address & 0xFF000000) != 0) {
				address = (nint)((nuint)address & 0xFFFFFF);
			} else if ((nuint)address < 0x9037C0 || (nuint)address >= 0x903800) {
				ref var prim = ref Unsafe.AsRef<Game.Render.GpuPrim>((void*)address);
				var command = (Game.Render.GpuPrimType)(prim.Value >> 2);

				switch (command) {
					case Game.Render.GpuPrimType.Tri:
						renderer.ProcessPrim_Tri(ref prim.As<Game.Render.GpuPrim_Tri>());
						break;

					case Game.Render.GpuPrimType.Quad:
						renderer.ProcessPrim_Quad(ref prim.As<Game.Render.GpuPrim_Quad>());
						break;

					case Game.Render.GpuPrimType.TriGouraud:
						renderer.ProcessPrim_TriGouraud(ref prim.As<Game.Render.GpuPrim_TriGouraud>());
						break;

					case Game.Render.GpuPrimType.QuadTexture:
						renderer.ProcessPrim_QuadTexture(ref prim.As<Game.Render.GpuPrim_QuadTexture>());
						break;

					case Game.Render.GpuPrimType.QuadGouraud:
						renderer.ProcessPrim_QuadGouraud(ref prim.As<Game.Render.GpuPrim_QuadGouraud>());
						break;

					case Game.Render.GpuPrimType.QuadTextureGouraud:
						renderer.ProcessPrim_QuadTextureGouraud(ref prim.As<Game.Render.GpuPrim_QuadTextureGouraud>());
						break;

					case Game.Render.GpuPrimType.Line:
						renderer.ProcessPrim_Lines(ref prim.As<Game.Render.GpuPrim_Line>(), 1);
						break;

					case Game.Render.GpuPrimType.TwoLines:
						renderer.ProcessPrim_Lines(ref prim.As<Game.Render.GpuPrim_Line>(), 2);
						break;

					case Game.Render.GpuPrimType.ThreeLines:
						renderer.ProcessPrim_Lines(ref prim.As<Game.Render.GpuPrim_Line>(), 3);
						break;

					case Game.Render.GpuPrimType.RectWH:
						renderer.ProcessPrim_RectWH(ref prim.As<Game.Render.GpuPrim_RectWH>());
						break;

					case Game.Render.GpuPrimType.RectWHTexture:
						renderer.ProcessPrim_RectWHTexture(ref prim.As<Game.Render.GpuPrim_RectWHTexture>());
						break;

					case Game.Render.GpuPrimType.TextGlyph:
						renderer.ProcessPrim_TextGlyph(ref prim.As<Game.Render.GpuPrim_TextGlyph>());
						break;

					case Game.Render.GpuPrimType.Rect8Texture:
						renderer.ProcessPrim_Rect8Texture(ref prim.As<Game.Render.GpuPrim_Rect8Texture>());
						break;

					case Game.Render.GpuPrimType.Sprite:
						renderer.ProcessPrim_Sprite(ref prim.As<Game.Render.GpuPrim_Sprite>());
						break;

					case Game.Render.GpuPrimType.SetDrawEnv: {
						ref var p = ref prim.As<Game.Render.GpuPrim_SetDrawEnv>();

						env.Dfe = (p.Flags & Game.Render.SetDrawEnvFlags.AllowDraw) != 0;
						env.Dtd = (p.Flags & Game.Render.SetDrawEnvFlags.EnableDither) != 0;
						env.TPage = p.TPage;

						if (p.rect != null)
							env.TextureWindow = *p.rect;

						break;
					}

					case Game.Render.GpuPrimType.SetTextureWindow: {
						ref var p = ref prim.As<Game.Render.GpuPrim_SetDrawEnv>();

						env.TextureWindow = *p.rect;

						break;
					}

					case Game.Render.GpuPrimType.TPageCopy:
						renderer.ProcessPrim_TPageCopy(ref prim.As<Game.Render.GpuPrim_TPageCopy>());
						break;

					case Game.Render.GpuPrimType.GlTextureFrame:
						renderer.ProcessPrim_GlTextureFrame(ref prim.As<Game.Render.GpuPrim_GlTextureFrame>());
						break;

					default:
#if DEBUG
						if (prim.Value != 0 && prim.Value != 0x80) {
							var tag = (nuint)prim.Tag;
							var cmd = (byte)command;

							Console.WriteLine(
								$"prim={address:X8} tag={tag:X8} value={prim.Value:X2} command={cmd} terrain={terrain}"
							);
						}
#endif
						break;
				}
			}

			address = Unsafe.Read<nint>((void*)address);
		}
	}

	static unsafe void DisplayTimedMessage(string message, int frameCount, MessageColour colour = MessageColour.White) {
		if (messageBuffer != null) {
			NativeMemory.Free(messageBuffer);
			messageBuffer = null;
		}

		messageBuffer = (byte*)NativeMemory.Alloc((nuint)(message.Length + 1));
		Encoding.ASCII.GetBytes(message.AsSpan(), new Span<byte>(messageBuffer, message.Length));
		messageBuffer[message.Length] = 0;

		messageTimer = frameCount;
		messageColour = colour;
	}


	public static unsafe void Enable() {
		Game.Window.HookWinMain(&WinMainHook);
	}
}