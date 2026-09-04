module;

#include "../res/resource.h"

#include <GLAD/glad.h>

#include <cstdint>
#include <windows.h>

export module bof3ext.shaderProgram;

import bof3ext.helpers;

import std;


namespace {
	std::string GetResource(const wchar_t* id) {
		static auto mod = GetModuleHandleA("bof3ext.dll");

		auto info = FindResource(mod, id, RT_RCDATA);
		auto len = SizeofResource(mod, info);
		auto res = LoadResource(mod, info);
		auto data = static_cast<const char*>(LockResource(res));

		return std::string(data, len);
	}
}


export enum class BufferType {
	Uniform = GL_UNIFORM_BUFFER,
	ShaderStorage = GL_SHADER_STORAGE_BUFFER
};


export class ShaderProgram {
public:
	void Compile(const char* vsSource, const char* fsSource) {
		vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vsSource, nullptr);
		glCompileShader(vs);
		VerifyShader(vs);

		fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fsSource, nullptr);
		glCompileShader(fs);
		VerifyShader(fs);

		program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		VerifyProgram();
	}

	void Compile(int vsResId, int fsResId) {
		auto vsSource = GetResource(MAKEINTRESOURCE(vsResId));
		auto fsSource = GetResource(MAKEINTRESOURCE(fsResId));

		Compile(vsSource.c_str(), fsSource.c_str());
	}

	void SetUniformBlockBuffer(const char* name, BufferType type, int binding, int buffer) {
		auto index = glGetUniformBlockIndex(program, name);
		glUniformBlockBinding(program, index, binding);
		auto err = glGetError();
		glBindBufferBase((GLenum)type, binding, buffer);
		err = glGetError();
		auto asd = true;
	}

	void SetUniformInt32(const char* name, int32_t value) {
		glUniform1i(glGetUniformLocation(program, name), value);
	}

	void SetUniformUInt32(const char* name, uint32_t value) {
		glUniform1ui(glGetUniformLocation(program, name), value);
	}

	void SetUniformFloat(const char* name, float value) {
		glUniform1f(glGetUniformLocation(program, name), value);
	}

	void SetUniformVec2(const char* name, float x, float y) {
		glUniform2f(glGetUniformLocation(program, name), x, y);
	}

	void Use() {
		glUseProgram(program);
	}


private:
	void __forceinline VerifyShader(GLuint id) {
		GLint compiled;
		GLchar log[256];

		glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);

		if (!compiled) {
			glGetShaderInfoLog(id, sizeof(log), nullptr, log);
			LogError(log);
		}
	}

	void __forceinline VerifyProgram() {
		GLint linked;
		GLchar log[256];

		glGetProgramiv(program, GL_LINK_STATUS, &linked);

		if (!linked) {
			glGetProgramInfoLog(program, sizeof(log), nullptr, log);
			LogError(log);
		}
	}


	GLuint vs, fs, program;
};