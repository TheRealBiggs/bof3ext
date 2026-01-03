//module;
//
//#include <GLAD/glad.h>
//#include <GLFW/glfw3.h>
//#include <stb/stb_image.h>
//
//#include <cstddef>
//#include <cstdint>
//#include <cstring>
//#include <stack>
//
//export module bof3ext.renderer;
//
//import bof3ext.math;
//import bof3ext.shaderProgram;
//import bof3ext.singleton;
//import bof3.render;
//
//
//export struct Texture {
//	int width, height;
//	uint32_t id;
//};
//
//export struct Vert {
//	Vec3f position;
//	Vec4f colour;
//	Vec2f uv;
//};
//
//
//export constexpr Vec4b White = { 255, 255, 255, 255 };
//
//
//export class Renderer : public Singleton<Renderer> {
//public:
//	void Initialise() {
//		auto res = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
//
//		solidTexture.width = solidTexture.height = 1;
//		glGenTextures(1, &solidTexture.id);
//
//		glBindTexture(GL_TEXTURE_2D, solidTexture.id);
//		{
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &White);
//		}
//		glBindTexture(GL_TEXTURE_2D, 0);
//
//		glGenVertexArrays(1, &vao);
//		glGenBuffers(1, &vbo);
//		glGenBuffers(1, &ebo);
//		glGenBuffers(1, &uboProj);
//		glGenBuffers(1, &ssboTPages);
//
//		glBindBuffer(GL_UNIFORM_BUFFER, uboProj);
//		{
//			glBufferData(GL_UNIFORM_BUFFER, sizeof(projectionMatrix), &projectionMatrix, GL_STATIC_DRAW);
//		}
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
//
//		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTPages);
//		{
//			glBufferData(GL_SHADER_STORAGE_BUFFER, 64 * 256 * sizeof(uint16_t) * 32, nullptr, GL_STATIC_DRAW);
//		}
//		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//		InitialiseShaders();
//
//		glBindVertexArray(vao);
//		{
//			glEnableVertexAttribArray(0);
//			glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vert, position));
//			glVertexAttribBinding(0, 0);
//
//			glEnableVertexAttribArray(1);
//			glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, offsetof(Vert, colour));
//			glVertexAttribBinding(1, 0);
//
//			glEnableVertexAttribArray(2);
//			glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(Vert, uv));
//			glVertexAttribBinding(2, 0);
//		}
//		glBindVertexArray(0);
//
//		glEnable(GL_DITHER);
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	}
//
//	void SetViewport(int x, int y, int w, int h) {
//		glViewport(x, y, w, h);
//
//		projectionMatrix = {
//			 2.f / w,  0,       0, 0,
//			 0,       -2.f / h, 0, 0,
//			 0,        0,       1, 0,
//			-1,        1,       0, 1
//		};
//
//		glBindBuffer(GL_UNIFORM_BUFFER, uboProj);
//		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projectionMatrix), &projectionMatrix);
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	}
//
//	Texture* LoadTexture(const char* filename) {
//		auto t = BorrowTexture();
//
//		auto data = stbi_load(filename, &t->width, &t->height, nullptr, 4);
//
//		glGenTextures(1, &t->id);
//
//		glBindTexture(GL_TEXTURE_2D, t->id);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->width, t->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//
//		glBindTexture(GL_TEXTURE_2D, 0);
//
//		stbi_image_free(data);
//
//		return t;
//	}
//
//	void DeleteTexture(Texture* texture) {
//		ReturnTexture(texture);
//	}
//
//	void LoadTexturePageData(PSX_RECT* rect, uint8_t* data) {
//		if (rect->x + rect->w > 1024
//			|| rect->y + rect->h > 512)
//			return;
//
//		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTPages);
//
//		auto offset = rect->y * 2048 + rect->x * sizeof(uint16_t);
//		auto _w = rect->w * sizeof(uint16_t);
//		auto _a = 4 * (_w >> 2);
//		auto _b = _w & 3;
//
//		for (int i = 0; i < rect->h; ++i) {
//			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, _a, data);
//			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + _a, _b, &data[_a]);
//			offset += 2048;
//			data += _w;
//		}
//
//		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//		if (rect->x == 0) {
//			auto asd = true;
//		}
//	}
//
//	void Begin() {
//		glClear(GL_COLOR_BUFFER_BIT);
//	}
//
//	void End() {
//
//	}
//
//	void DrawTexturedQuad(
//		Texture* texture,
//		const Vec3f& v1, const Vec4b& c1, const Vec2f& uv1,
//		const Vec3f& v2, const Vec4b& c2, const Vec2f& uv2,
//		const Vec3f& v3, const Vec4b& c3, const Vec2f& uv3,
//		const Vec3f& v4, const Vec4b& c4, const Vec2f& uv4) {
//
//		basicShader.Use();
//
//		Vert verts[] = {
//			{ v1, { c1.r / 255.f, c1.g / 255.f, c1.b / 255.f, c1.a / 255.f }, uv1 },
//			{ v2, { c2.r / 255.f, c2.g / 255.f, c2.b / 255.f, c2.a / 255.f }, uv2 },
//			{ v3, { c3.r / 255.f, c3.g / 255.f, c3.b / 255.f, c3.a / 255.f }, uv3 },
//			{ v4, { c4.r / 255.f, c4.g / 255.f, c4.b / 255.f, c4.a / 255.f }, uv4 }
//		};
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
//		{
//			glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//		}
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//		GLuint indices[] = {
//			0, 1, 2,
//			2, 1, 3,
//		};
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//		{
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//		}
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		glBindTexture(GL_TEXTURE_2D, texture->id);
//		
//		glBindVertexArray(vao);
//		{
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//			glBindVertexBuffer(0, vbo, 0, sizeof(Vert));
//
//			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//		}
//		glBindVertexArray(0);
//	}
//
//	void ProcessPrim_TexturedQuad(GpuPrim_TexturedQuad* prim) {
//		psxShader.Use();
//		psxShader.SetUniformUInt32("tpage", prim->texturePage);
//		psxShader.SetUniformUInt32("palette", prim->palette);
//
//		Vec4f colour = { prim->colour.r / 255.f, prim->colour.g / 255.f, prim->colour.b / 255.f, 255.f };
//
//		Vert verts[] = {
//			{ { prim->v1.x * *g_RenderScaleX, prim->v1.y * *g_RenderScaleY, prim->v1.z }, colour, { (float)prim->t1.x, (float)prim->t1.y } },
//			{ { prim->v2.x * *g_RenderScaleX, prim->v2.y * *g_RenderScaleY, prim->v2.z }, colour, { (float)prim->t2.x, (float)prim->t2.y } },
//			{ { prim->v3.x * *g_RenderScaleX, prim->v3.y * *g_RenderScaleY, prim->v3.z }, colour, { (float)prim->t3.x, (float)prim->t3.y } },
//			{ { prim->v4.x * *g_RenderScaleX, prim->v4.y * *g_RenderScaleY, prim->v4.z }, colour, { (float)prim->t4.x, (float)prim->t4.y } }
//		};
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
//		{
//			glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//		}
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//		GLuint indices[] = {
//			0, 1, 2,
//			2, 1, 3,
//		};
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//		{
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//		}
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		glBindVertexArray(vao);
//		{
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//			glBindVertexBuffer(0, vbo, 0, sizeof(Vert));
//
//			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//		}
//		glBindVertexArray(0);
//	}
//
//	void ProcessPrim_TexturedRectWH(GpuPrim_TexturedRectWH* prim) {
//		psxShader.Use();
//		psxShader.SetUniformUInt32("tpage", g_DrawEnv->tpage);
//		psxShader.SetUniformUInt32("palette", prim->palette);
//
//		Vec4f colour = { prim->colour.r / 255.f, prim->colour.g / 255.f, prim->colour.b / 255.f, 255.f };
//
//		Vert verts[] = {
//			{ { (prim->v1.x               ) * *g_RenderScaleX, (prim->v1.y               ) * *g_RenderScaleY, prim->v1.z }, colour, {  prim->t1.x,				        prim->t1.y } },
//			{ { (prim->v1.x + prim->size.x) * *g_RenderScaleX, (prim->v1.y               ) * *g_RenderScaleY, prim->v1.z }, colour, { (prim->t1.x + prim->size.x - 1),  prim->t1.y } },
//			{ { (prim->v1.x               ) * *g_RenderScaleX, (prim->v1.y + prim->size.y) * *g_RenderScaleY, prim->v1.z }, colour, {  prim->t1.x,					   (prim->t1.y + prim->size.y - 1) } },
//			{ { (prim->v1.x + prim->size.x) * *g_RenderScaleX, (prim->v1.y + prim->size.y) * *g_RenderScaleY, prim->v1.z }, colour, { (prim->t1.x + prim->size.x - 1), (prim->t1.y + prim->size.y - 1) } }
//		};
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
//		{
//			glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//		}
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//		GLuint indices[] = {
//			0, 1, 2,
//			2, 1, 3,
//		};
//		
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//		{
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//		}
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		glBindVertexArray(vao);
//		{
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//			glBindVertexBuffer(0, vbo, 0, sizeof(Vert));
//
//			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//		}
//		glBindVertexArray(0);
//	}
//
//	void DrawQuad(
//		const Vec3f& v1, const Vec4b& c1,
//		const Vec3f& v2, const Vec4b& c2,
//		const Vec3f& v3, const Vec4b& c3,
//		const Vec3f& v4, const Vec4b& c4) {
//
//		DrawTexturedQuad(&solidTexture, v1, c1, { 0, 0 }, v2, c2, { 0, 0 }, v3, c3, { 0, 0 }, v4, c4, { 0, 0 });
//	}
//
//	void DrawTexture(Texture* texture, const Rectf& rect, const Vec4b& c1, const Vec4b& c2, const Vec4b& c3, const Vec4b& c4) {
//		DrawTexturedQuad(
//			texture,
//			{ rect.x,          rect.y, 0 },          c1, { 0.f, 1.f },
//			{ rect.x + rect.w, rect.y, 0 },          c2, { 1.f, 1.f },
//			{ rect.x,          rect.y + rect.h, 0 }, c3, { 0.f, 0.f },
//			{ rect.x + rect.w, rect.y + rect.h, 0 }, c4, { 1.f, 0.f }
//		);
//	}
//
//	void DrawTexture(Texture* texture, const Rectf& rect, const Vec4b& colour = White) {
//		DrawTexture(texture, rect, colour, colour, colour, colour);
//	}
//
//	void DrawRectangle(const Rectf& rect, const Vec4b& colour = White) {
//		DrawTexture(&solidTexture, rect, colour);
//	}
//
//	void DrawRectangle(const Rectf& rect, const Vec4b& c1, const Vec4b& c2, const Vec4b& c3, const Vec4b& c4) {
//		DrawTexture(&solidTexture, rect, c1, c2, c3, c4);
//	}
//
//private:
//	void __forceinline InitialiseShaders() {
//		basicShader.Compile(
//			// Vertex shader
//			R"(
//				#version 430 core
//
//				layout (location = 0) in vec3 pos;
//				layout (location = 1) in vec4 color;
//				layout (location = 2) in vec2 uv;
//
//				layout(std140, binding = 0) uniform ProjectionMatrix {
//					mat4 proj;
//				};
//
//				out vec4 VertColor;
//				out vec2 TexCoord;
//
//				void main() {
//					gl_Position = proj * vec4(pos.x, pos.y, pos.z, 1.0);
//					VertColor = color;
//					TexCoord = vec2(uv.x, 1.0 - uv.y);
//				}
//			)",
//
//			// Fragment shader
//			R"(
//				#version 430 core
//
//				out vec4 FragColor;
//
//				in vec2 TexCoord;
//				in vec4 VertColor;
//
//				uniform sampler2D tex;
//
//				void main() {
//					FragColor = texture(tex, TexCoord) * VertColor;
//				}
//			)"
//		);
//
//		psxShader.Compile(
//			// Vertex shader
//			R"(
//				#version 430 core
//
//				layout (location = 0) in vec3 pos;
//				layout (location = 1) in vec4 color;
//				layout (location = 2) in vec2 uv;
//
//				layout(std140, binding = 0) uniform ProjectionMatrix {
//					mat4 proj;
//				};
//
//				out vec4 VertColor;
//				out vec2 TexCoord;
//
//				void main() {
//					gl_Position = proj * vec4(pos.x, pos.y, pos.z, 1.0);
//					VertColor = color;
//					TexCoord = uv;
//				}
//			)",
//
//			// Fragment shader
//			R"(
//				#version 450 core
//				#extension GL_NV_gpu_shader5 : enable
//
//				layout(std430, binding = 1) buffer TexturePages {
//					uint8_t tpagebuffer[];
//				};
//
//				out vec4 FragColor;
//
//				in vec2 TexCoord;
//				in vec4 VertColor;
//
//				uniform uint tpage;
//				uniform uint palette;
//
//				void main() {
//					uint tx = (tpage & 0xF) * 64;
//					uint ty = ((tpage >> 4) & 0x1) * 256;
//
//					ty += uint(floor(TexCoord.y));
//
//					uint f = (tpage >> 7) & 3;
//					uint t = ty * 2048 + tx * 2;
//
//					uint cx = palette & 0x3F;
//					uint cy = palette >> 6;
//
//					if (f == 0) {
//						t += uint(floor(TexCoord.x / 2.0));
//
//						uint tr = uint(floor(TexCoord.x)) % 2;
//
//						uint tp = tpagebuffer[t];
//						tp = (tp >> (tr * 4)) & 0xF;
//
//						uint pt = cy * 2048 +  tp * 2;
//						uint pp = tpagebuffer[pt];
//						pp |= uint(tpagebuffer[pt + 1]) << 8;
//						uint r = pp & 0x1F;
//						uint g = (pp >> 5) & 0x1F;
//						uint b = (pp >> 10) & 0x1F;
//						uint a = (pp >> 15) & 0x1;
//
//						if (!(r == 0 && g == 0 && b == 0))
//							a = 1;
//
//						FragColor = vec4(r / 31.0, g / 31.0, b / 31.0, a) * vec4(VertColor.rgb * 2, VertColor.a);
//					} else if (f == 1) {
//						t += uint(floor(TexCoord.x));
//
//						uint tp = tpagebuffer[t];
//
//						uint pt = cy * 2048 + cx * 2 + tp * 2;
//						uint pp = tpagebuffer[pt];
//						pp |= uint(tpagebuffer[pt + 1]) << 8;
//						uint r = pp & 0x1F;
//						uint g = (pp >> 5) & 0x1F;
//						uint b = (pp >> 10) & 0x1F;
//						uint a = (pp >> 15) & 0x1;
//
//						if (!(r == 0 && g == 0 && b == 0))
//							a = 1;
//
//						FragColor = vec4(r / 31.0, g / 31.0, b / 31.0, a) * vec4(VertColor.rgb * 2, VertColor.a);
//					} else {
//						FragColor = VertColor;
//					}
//				}
//			)"
//		);
//
//		basicShader.SetUniformBlockBuffer("ProjectionMatrix", BufferType::Uniform, 0, uboProj);
//
//		psxShader.SetUniformBlockBuffer("ProjectionMatrix", BufferType::Uniform, 0, uboProj);
//		psxShader.SetUniformBlockBuffer("TexturePages", BufferType::ShaderStorage, 1, ssboTPages);
//	}
//
//	Texture* BorrowTexture() {
//		if (texturePool.empty())
//			return new Texture;
//
//		auto t = texturePool.top();
//		texturePool.pop();
//
//		return t;
//	}
//
//	void ReturnTexture(Texture* t) {
//		glDeleteTextures(1, &t->id);
//		texturePool.push(t);
//	}
//
//
//	Mat4f projectionMatrix;
//	ShaderProgram basicShader, psxShader;
//	GLuint vao, vbo, ebo;
//	GLuint uboProj, ssboTPages;
//	Texture solidTexture;
//	std::stack<Texture*> texturePool;
//};