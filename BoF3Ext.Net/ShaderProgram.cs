using System;
using System.Collections.Generic;
using System.IO;

using Silk.NET.OpenGL;

namespace BoF3Ext;


class ShaderProgram(GL gl) {
	public enum Uniforms {
		TextureWindow,
		BlendMode,
		Pass,
		ProjectionMatrix,
		ScaleAndOffset,
		TexturePages,
	}


	static readonly string[] UNIFORM_NAMES = [
		"TextureWindow",
		"BlendMode",
		"Pass",
		"ProjectionMatrix",
		"ScaleAndOffset",
		"TexturePages"
	];


	readonly Dictionary<Uniforms, long> uniformLocations = [];
	uint program;


	public void Compile(string vsName, string fsName) {
		var vsCode = GetShaderCode(vsName);
		var fsCode = GetShaderCode(fsName);

		var vs = CreateAndCompileShader(ShaderType.VertexShader, vsCode);
		var fs = CreateAndCompileShader(ShaderType.FragmentShader, fsCode);

		program = gl.CreateProgram();
		gl.AttachShader(program, vs);
		gl.AttachShader(program, fs);
		gl.LinkProgram(program);

		gl.GetProgram(program, ProgramPropertyARB.LinkStatus, out int status);

		if (status != (int)GLEnum.True)
			throw new Exception($"Failed to link shader program: {gl.GetProgramInfoLog(program)}");

		gl.DetachShader(program, vs);
		gl.DetachShader(program, fs);

		gl.DeleteShader(vs);
		gl.DeleteShader(fs);

		CacheUniformLocation(Uniforms.TextureWindow);
		CacheUniformLocation(Uniforms.BlendMode);
		CacheUniformLocation(Uniforms.Pass);

		CacheUniformBlockIndex(Uniforms.ProjectionMatrix);
		CacheUniformBlockIndex(Uniforms.ScaleAndOffset);

		CacheProgramResourceIndex(Uniforms.TexturePages);
	}

	public void Use() {
		gl.UseProgram(program);
	}

	public void SetUniformBlockBuffer(Uniforms name, uint binding, uint buffer) {
		gl.UniformBlockBinding(program, (uint)uniformLocations[name], binding);
		gl.BindBufferBase(BufferTargetARB.UniformBuffer, binding, buffer);
	}

	public void SetShaderStorageBlockBuffer(Uniforms name, uint binding, uint buffer) {
		gl.ShaderStorageBlockBinding(program, (uint)uniformLocations[name], binding);
		gl.BindBufferBase(BufferTargetARB.ShaderStorageBuffer, binding, buffer);
	}

	public void SetUniform(Uniforms name, int value) {
		gl.Uniform1((int)uniformLocations[name], value);
	}

	public void SetUniform(Uniforms name, uint value) {
		gl.Uniform1((int)uniformLocations[name], value);
	}

	public void SetUniform(Uniforms name, float value) {
		gl.Uniform1((int)uniformLocations[name], value);
	}

	public void SetUniform(Uniforms name, float x, float y) {
		gl.Uniform2((int)uniformLocations[name], x, y);
	}

	public void SetUniform(Uniforms name, int x, int y, int z, int w) {
		gl.Uniform4((int)uniformLocations[name], x, y, z, w);
	}


	uint CreateAndCompileShader(ShaderType type, string source) {
		uint id = gl.CreateShader(type);
		gl.ShaderSource(id, source);
		gl.CompileShader(id);

		gl.GetShader(id, ShaderParameterName.CompileStatus, out int status);

		if (status != (int)GLEnum.True)
			throw new Exception($"Failed to compile shader: {gl.GetShaderInfoLog(id)}");

		return id;
	}

	void CacheUniformLocation(Uniforms value) {
		uniformLocations[value] = gl.GetUniformLocation(program, UNIFORM_NAMES[(int)value]);
	}

	void CacheUniformBlockIndex(Uniforms value) {
		uniformLocations[value] = gl.GetUniformBlockIndex(program, UNIFORM_NAMES[(int)value]);
	}

	void CacheProgramResourceIndex(Uniforms value) {
		uniformLocations[value] = gl.GetProgramResourceIndex(
			program, ProgramInterface.ShaderStorageBlock, UNIFORM_NAMES[(int)value]
		);
	}


	static string GetShaderCode(string name) {
		var path = $"BoF3Ext.res.shaders.{name}.glsl";

		using var stream = typeof(ShaderProgram).Assembly.GetManifestResourceStream(path)
			?? throw new Exception($"Resource not found '{path}'");
		using var reader = new StreamReader(stream);

		return reader.ReadToEnd();
	}
}
