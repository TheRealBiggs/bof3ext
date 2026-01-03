//module;
//
//#include <GLAD/glad.h>
//
//#include <cstdint>
//
//export module bof3ext.shaderProgram;
//
//import bof3ext.helpers;
//
//
//export enum class BufferType {
//	Uniform = GL_UNIFORM_BUFFER,
//	ShaderStorage = GL_SHADER_STORAGE_BUFFER
//};
//
//
//export class ShaderProgram {
//public:
//	void Compile(const char* vsSource, const char* fsSource) {
//		vs = glCreateShader(GL_VERTEX_SHADER);
//		glShaderSource(vs, 1, &vsSource, nullptr);
//		glCompileShader(vs);
//		VerifyShader(vs);
//
//		fs = glCreateShader(GL_FRAGMENT_SHADER);
//		glShaderSource(fs, 1, &fsSource, nullptr);
//		glCompileShader(fs);
//		VerifyShader(fs);
//
//		program = glCreateProgram();
//		glAttachShader(program, vs);
//		glAttachShader(program, fs);
//		glLinkProgram(program);
//		VerifyProgram();
//	}
//
//	void SetUniformBlockBuffer(const char* name, BufferType type, int binding, int buffer) {
//		auto index = glGetUniformBlockIndex(program, name);
//		glUniformBlockBinding(program, index, binding);
//		auto err = glGetError();
//		glBindBufferBase((GLenum)type, binding, buffer);
//		err = glGetError();
//		auto asd = true;
//	}
//
//	void SetUniformUInt32(const char* name, uint32_t value) {
//		glUniform1ui(glGetUniformLocation(program, name), value);
//	}
//
//	void Use() {
//		glUseProgram(program);
//	}
//
//
//private:
//	void __forceinline VerifyShader(GLuint id) {
//		GLint compiled;
//		GLchar log[256];
//
//		glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
//
//		if (!compiled) {
//			glGetShaderInfoLog(id, sizeof(log), nullptr, log);
//			LogError(log);
//		}
//	}
//
//	void __forceinline VerifyProgram() {
//		GLint linked;
//		GLchar log[256];
//
//		glGetProgramiv(program, GL_LINK_STATUS, &linked);
//
//		if (!linked) {
//			glGetProgramInfoLog(program, sizeof(log), nullptr, log);
//			LogError(log);
//		}
//	}
//
//
//	GLuint vs, fs, program;
//};