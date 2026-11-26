using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using Silk.NET.OpenGL;

using Mat4f = System.Numerics.Matrix4x4;

namespace BoF3Ext;


enum DrawCommandType {
	Psx,
	ReplacedTexture,
	Lines,
	Text
}

struct Vert(Vec3f position, Vec4f colour, Vec2f uv, int tPage = 0, int palette = 0) {
	public Vec3f Position = position;
	public Vec4f Colour = colour;
	public Vec2f Uv = uv;
	public int TPage = tPage;
	public int Palette = palette;
}

class DrawCommand {
	public DrawCommandType Type;
	public uint TextureId;
	public Game.Render.PSX_RECT TextureWindow;
	public bool BlendEnabled;
	public int BlendMode;
	public List<Vert> Verts = [];
	public List<uint> Indices = [];
}


unsafe partial class Renderer : Singleton<Renderer> {
	public static readonly Vec4b White = new(255, 255, 255, 255);


	public uint LocationNameBgTextureId => locationNameBgTextureId;
	public uint DialogueBorderTextureId => dialogueBorderTextureId;


	readonly Stack<DrawCommand> commandPool = new();
	readonly List<DrawCommand> commandQueue = [];
	readonly byte[] cpuTPages = new byte[64 * 256 * sizeof(ushort) * 32];
	readonly float[] scale = new float[2];

	Mat4f projectionMatrix;
	ShaderProgram basicShader = null!, psxShader = null!, replacedTextureShader = null!, textShader = null!;
	uint vao, vbo, ebo;
	uint uboProj, uboScale, ssboTPages;
	uint locationNameBgTextureId, dialogueBorderTextureId;
	GL gl = null!;


	public bool Initialise(GL gl) {
		this.gl = gl;
		basicShader = new(gl);
		psxShader = new(gl);
		replacedTextureShader = new(gl);
		textShader = new(gl);

		vao = gl.GenVertexArray();
		vbo = gl.GenBuffer();
		ebo = gl.GenBuffer();
		uboProj = gl.GenBuffer();
		uboScale = gl.GenBuffer();
		ssboTPages = gl.GenBuffer();

		// Load location popup background texture
		if (!LoadTexture("location_name_bg.png", out locationNameBgTextureId))
			return false;

		// Load dialogue border texture
		if (!LoadTexture("dialogue_border.png", out dialogueBorderTextureId))
			return false;

		gl.BindBuffer(BufferTargetARB.UniformBuffer, uboProj);
		{
			gl.BufferData(
				BufferTargetARB.UniformBuffer,
				(nuint)Unsafe.SizeOf<Mat4f>(),
				new Span<Mat4f>(ref projectionMatrix),
				BufferUsageARB.StaticDraw
			);
		}
		gl.BindBuffer(BufferTargetARB.UniformBuffer, 0);

		gl.BindBuffer(BufferTargetARB.UniformBuffer, uboScale);
		{
			gl.BufferData(
				BufferTargetARB.UniformBuffer,
				2 * sizeof(float),
				scale.AsSpan(),
				BufferUsageARB.StaticDraw
			);
		}
		gl.BindBuffer(BufferTargetARB.UniformBuffer, 0);

		gl.BindBuffer(BufferTargetARB.ShaderStorageBuffer, ssboTPages);
		{
			gl.BufferData(
				BufferTargetARB.ShaderStorageBuffer,
				64 * 256 * sizeof(ushort) * 32,
				null,
				BufferUsageARB.StaticDraw
			);
		}
		gl.BindBuffer(BufferTargetARB.ShaderStorageBuffer, 0);

		if (!InitialiseShaders())
			return false;

		gl.BindVertexArray(vao);
		{
			gl.BindBuffer(BufferTargetARB.ArrayBuffer, vbo);
			gl.BindBuffer(BufferTargetARB.ElementArrayBuffer, ebo);

			gl.BindVertexBuffer(0, vbo, 0, (uint)Unsafe.SizeOf<Vert>());

			gl.EnableVertexAttribArray(0);
			gl.VertexAttribFormat(0, 3, VertexAttribType.Float, false, (uint)Marshal.OffsetOf<Vert>(nameof(Vert.Position)));
			gl.VertexAttribBinding(0, 0);

			gl.EnableVertexAttribArray(1);
			gl.VertexAttribFormat(1, 4, VertexAttribType.Float, false, (uint)Marshal.OffsetOf<Vert>(nameof(Vert.Colour)));
			gl.VertexAttribBinding(1, 0);

			gl.EnableVertexAttribArray(2);
			gl.VertexAttribFormat(2, 2, VertexAttribType.Float, false, (uint)Marshal.OffsetOf<Vert>(nameof(Vert.Uv)));
			gl.VertexAttribBinding(2, 0);

			gl.EnableVertexAttribArray(3);
			gl.VertexAttribIFormat(3, 1, VertexAttribIType.Int, (uint)Marshal.OffsetOf<Vert>(nameof(Vert.TPage)));
			gl.VertexAttribBinding(3, 0);

			gl.EnableVertexAttribArray(4);
			gl.VertexAttribIFormat(4, 1, VertexAttribIType.Int, (uint)Marshal.OffsetOf<Vert>(nameof(Vert.Palette)));
			gl.VertexAttribBinding(4, 0);
		}
		gl.BindVertexArray(0);

		gl.BindBuffer(BufferTargetARB.ArrayBuffer, 0);

		gl.Enable(EnableCap.Blend);
		gl.Disable(EnableCap.Multisample);
		gl.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
		gl.Enable(EnableCap.LineSmooth);
		gl.LineWidth(4);

		gl.DebugMessageCallback(DebugMessageCallback, null);

		TextureReplacer.Instance.Initialise(gl, cpuTPages);

		return true;
	}

	public void SetViewport(int x, int y, int w, int h) {
		gl.Viewport(x, y, (uint)w, (uint)h);

		projectionMatrix = new(
			 2f / w, 0, 0, 0,
			 0, -2f / h, 0, 0,
			 0, 0, 1, 0,
			-1, 1, 0, 1
		);

		gl.BindBuffer(BufferTargetARB.UniformBuffer, uboProj);
		{
			gl.BufferSubData(BufferTargetARB.UniformBuffer, 0, (nuint)Unsafe.SizeOf<Mat4f>(), ref projectionMatrix);
		}
		gl.BindBuffer(BufferTargetARB.UniformBuffer, 0);

		var _scale = ConfigManager.Instance.RenderScale;

		scale[0] = _scale;
		scale[1] = _scale;

		gl.BindBuffer(BufferTargetARB.UniformBuffer, uboScale);
		{
			gl.BufferSubData(BufferTargetARB.UniformBuffer, 0, scale);
		}
		gl.BindBuffer(BufferTargetARB.UniformBuffer, 0);
	}

	public void LoadTexturePageData(ref Game.Render.PSX_RECT rect, ref Span<byte> data) {
		if (rect.X + rect.W > 1024
			|| rect.Y + rect.H > 512)
			return;

		gl.BindBuffer(BufferTargetARB.ShaderStorageBuffer, ssboTPages);

		var offset = rect.Y * 2048 + rect.X * sizeof(ushort);
		var w = rect.W * sizeof(ushort);
		var dataOffset = 0;

		for (int i = 0; i < rect.H; ++i) {
			var row = data.Slice(dataOffset, w);

			gl.BufferSubData(BufferTargetARB.ShaderStorageBuffer, offset, row);
			row.CopyTo(cpuTPages.AsSpan(offset, w));

			offset += 2048;
			dataOffset += w;
		}

		gl.BindBuffer(BufferTargetARB.ShaderStorageBuffer, 0);
	}

	public void Begin() {
		commandQueue.Clear();

		gl.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
	}

	public void End() {
		foreach (var cmd in commandQueue) {
			switch (cmd.Type) {
				case DrawCommandType.Psx:
				case DrawCommandType.Lines:
					psxShader.Use();
					psxShader.SetUniform(
						ShaderProgram.Uniforms.TextureWindow,
						cmd.TextureWindow.X / 8,
						cmd.TextureWindow.Y / 8,
						31 - ((cmd.TextureWindow.W / 8) - 1),
						31 - ((cmd.TextureWindow.H / 8) - 1)
					);

					break;

				case DrawCommandType.ReplacedTexture:
					replacedTextureShader.Use();
					gl.BindTexture(TextureTarget.Texture2D, cmd.TextureId);

					break;

				case DrawCommandType.Text:
					textShader.Use();
					gl.BindTexture(TextureTarget.Texture2D, cmd.TextureId);

					break;
			}

			gl.BindBuffer(BufferTargetARB.ArrayBuffer, vbo);
			{
				var data = CollectionsMarshal.AsSpan(cmd.Verts);

				gl.BufferData(
					BufferTargetARB.ArrayBuffer,
					(nuint)(cmd.Verts.Count * Unsafe.SizeOf<Vert>()),
					data,
					BufferUsageARB.StaticDraw
				);
			}
			gl.BindBuffer(BufferTargetARB.ArrayBuffer, 0);

			gl.BindBuffer(BufferTargetARB.CopyWriteBuffer, ebo);
			{
				var data = CollectionsMarshal.AsSpan(cmd.Indices);

				gl.BufferData(
					BufferTargetARB.CopyWriteBuffer,
					(nuint)(cmd.Indices.Count * sizeof(uint)),
					data,
					BufferUsageARB.StaticDraw
				);
			}
			gl.BindBuffer(BufferTargetARB.CopyWriteBuffer, 0);

			gl.BlendEquation(BlendEquationModeEXT.FuncAdd);
			gl.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);

			if (cmd.Type == DrawCommandType.Psx) {
				psxShader.SetUniform(ShaderProgram.Uniforms.BlendMode, cmd.BlendMode);

				if (cmd.BlendEnabled) {
					// Pass 1 (opaque)
					gl.Disable(EnableCap.Blend);
					psxShader.SetUniform(ShaderProgram.Uniforms.Pass, 1);

					DrawElements(cmd);

					// Pass 2 (blended)
					gl.Enable(EnableCap.Blend);
					psxShader.SetUniform(ShaderProgram.Uniforms.Pass, 2);

					switch (cmd.BlendMode) {
						case 0:
							gl.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
							break;

						case 2:
							gl.BlendEquation(BlendEquationModeEXT.FuncReverseSubtract);
							gl.BlendFunc(BlendingFactor.One, BlendingFactor.One);
							break;

						case 1:
							gl.BlendFunc(BlendingFactor.One, BlendingFactor.One);
							break;

						case 3:
							gl.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.One);
							break;
					}

					DrawElements(cmd);
				} else {
					gl.Enable(EnableCap.Blend);
					gl.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
					psxShader.SetUniform(ShaderProgram.Uniforms.Pass, 0);

					DrawElements(cmd);
				}
			} else {
				if (cmd.Type == DrawCommandType.Lines)
					psxShader.SetUniform(ShaderProgram.Uniforms.Pass, 0);

				if (cmd.BlendEnabled && cmd.Type != DrawCommandType.ReplacedTexture) {
					gl.Enable(EnableCap.Blend);
					psxShader.SetUniform(ShaderProgram.Uniforms.BlendMode, cmd.BlendMode);

					switch (cmd.BlendMode) {
						case 0:
							gl.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
							break;

						case 2:
							gl.BlendEquation(BlendEquationModeEXT.FuncReverseSubtract);
							gl.BlendFunc(BlendingFactor.One, BlendingFactor.One);
							break;

						case 1:
							gl.BlendFunc(BlendingFactor.One, BlendingFactor.One);
							break;

						case 3:
							gl.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.One);
							break;
					}
				} else {
					if (cmd.Type == DrawCommandType.ReplacedTexture) {
						gl.Enable(EnableCap.Blend);

						switch (cmd.BlendMode) {
							case -1:
							case 0:
								gl.BlendFunc(BlendingFactor.One, BlendingFactor.OneMinusSrcAlpha);
								break;

							case 2:
								gl.BlendEquation(BlendEquationModeEXT.FuncReverseSubtract);
								gl.BlendFunc(BlendingFactor.One, BlendingFactor.One);
								break;

							case 1:
							case 3:
								gl.BlendFunc(BlendingFactor.One, BlendingFactor.One);
								break;
						}
					} else {
						gl.Disable(EnableCap.Blend);

						psxShader.SetUniform(ShaderProgram.Uniforms.BlendMode, -1);
					}
				}

				DrawElements(cmd);
			}

			ReturnDrawCommand(cmd);
		}
	}


	bool InitialiseShader(ShaderProgram shader, string vsName, string fsName) {
		try {
			shader.Compile(vsName, fsName);
		} catch (Exception e) {
			Console.WriteErrorLine(e.Message);

			return false;
		}

		shader.SetUniformBlockBuffer(ShaderProgram.Uniforms.ProjectionMatrix, 0, uboProj);
		shader.SetUniformBlockBuffer(ShaderProgram.Uniforms.Scale, 1, uboScale);

		return true;
	}

	bool InitialiseShaders() {
		if (!InitialiseShader(basicShader, "basic_shader.vs", "basic_shader.fs")
			|| !InitialiseShader(psxShader, "psx_shader.vs", "psx_shader.fs")
			|| !InitialiseShader(replacedTextureShader, "replaced_texture_shader.vs", "replaced_texture_shader.fs")
			|| !InitialiseShader(textShader, "text_shader.vs", "text_shader.fs"))
			return false;

		psxShader.SetShaderStorageBlockBuffer(ShaderProgram.Uniforms.TexturePages, 2, ssboTPages);

		return true;
	}

	bool LoadTexture(string filename, out uint textureId) {
		TextureImage image;

		try {
			image = TextureImage.Load($"NewData/Textures/{filename}");
		} catch (Exception) {
			Console.WriteErrorLine("Failed to load location popup image");
			textureId = UInt32.MaxValue;

			return false;
		}

		var width = image.Width;
		var height = image.Height;
		textureId = gl.GenTexture();

		gl.BindTexture(TextureTarget.Texture2D, textureId);
		{
			gl.TexImage2D(
				TextureTarget.Texture2D,
				0,
				InternalFormat.Rgba,
				(uint)width,
				(uint)height,
				0,
				PixelFormat.Rgba,
				PixelType.UnsignedByte,
				image.Data.AsSpan()
			);

			gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)GLEnum.ClampToEdge);
			gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)GLEnum.ClampToEdge);
			gl.TexParameter(
				TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)GLEnum.LinearMipmapLinear
			);
			gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)GLEnum.Linear);

			gl.GenerateMipmap(TextureTarget.Texture2D);
		}
		gl.BindTexture(TextureTarget.Texture2D, 0);

		return true;
	}

	DrawCommand BorrowDrawCommand() {
		if (commandPool.Count == 0)
			return new();

		var c = commandPool.Peek();
		commandPool.Pop();

		return c;
	}

	void ReturnDrawCommand(DrawCommand c) {
		c.TextureId = 0;
		c.Verts.Clear();
		c.Indices.Clear();

		commandPool.Push(c);
	}

	DrawCommand CreateOrGetDrawCommand(
		DrawCommandType type, int textureId, bool blendEnabled, int blendMode, Game.Render.PSX_RECT textureWindow
	) {
		DrawCommand cmd;

		if (commandQueue.Count > 0) {
			var lastCmd = commandQueue[^1];

			var typeSame = lastCmd.Type == type;
			var abeSame = lastCmd.BlendEnabled == blendEnabled;
			var blendSame = lastCmd.BlendMode == blendMode;
			var twSame = lastCmd.TextureWindow == textureWindow;
			var texSame = lastCmd.TextureId == (uint)textureId;
			var typePsx = type == DrawCommandType.Psx;
			var typeText = type == DrawCommandType.Text;
			var typeRTex = type == DrawCommandType.ReplacedTexture;

			var psxMatch =
				typeSame && (typePsx || typeText) && !(typePsx && blendEnabled) && abeSame && blendSame && twSame;

			if (psxMatch || (typeSame && typeRTex && texSame && abeSame && blendSame))
				cmd = lastCmd;
			else {
				cmd = BorrowDrawCommand();
				commandQueue.Add(cmd);
			}
		} else {
			cmd = BorrowDrawCommand();
			commandQueue.Add(cmd);
		}

		cmd.Type = type;
		cmd.TextureId = (uint)textureId;
		cmd.TextureWindow = Game.Render.DrawEnv.TextureWindow;
		cmd.BlendEnabled = blendEnabled;
		cmd.BlendMode = blendMode;

		return cmd;
	}

	void DrawElements(DrawCommand cmd) {
		gl.BindVertexArray(vao);
		{
			if (cmd.Type == DrawCommandType.Lines) {
				gl.DrawElements(
					PrimitiveType.LineStrip,
					(uint)cmd.Indices.Count,
					DrawElementsType.UnsignedInt,
					null
				);
			} else {
				gl.DrawElements(
					PrimitiveType.Triangles,
					(uint)cmd.Indices.Count,
					DrawElementsType.UnsignedInt,
					null
				);
			}
		}
		gl.BindVertexArray(0);
	}


	static void DebugMessageCallback(
		GLEnum source, GLEnum type, int id, GLEnum severity, int length, nint message, nint userParam
	) {
		Console.WriteLine(Marshal.PtrToStringUTF8(message, length));
	}

	static void GetBlendAndAlpha<T>(ref T p, ushort tpage, out bool enabled, out int mode, out float alpha) {
		ref var prim = ref Unsafe.As<T, Game.Render.GpuPrim>(ref p);

		enabled = prim.Abe;
		mode = enabled ? (tpage >> 5) & 3 : -1;
		alpha = enabled
			? (mode == 0
				? 0.5f
				: mode == 3
				? 0.25f
				: 1f)
			: 1f;
	}
}