module;

#include "../res/resource.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <cstddef>
#include <cstdint>
#include <stack>

export module bof3ext.renderer;

import bof3ext.glGlyphManager;
import bof3ext.helpers;
import bof3ext.shaderProgram;
import bof3ext.singleton;
import bof3.math;
import bof3.render;

import std;


enum class DrawCommandType {
	Psx,
	ReplacedTexture,
	Lines,
	Text
};


struct Texture {
	int width, height;
	uint32_t id;
};

struct Vert {
	Vec3f position;
	Vec4f colour;
	Vec2f uv;
	int tpage;
	int palette;
};

class DrawCommand {
public:
	DrawCommandType type;
	GLuint textureId;
	std::vector<Vert> verts;
	std::vector<GLuint> indices;
	bool valid = true;
};


export constexpr Vec4b White = { 255, 255, 255, 255 };


std::map<uint64_t, Texture*> replacementTextures;


void __stdcall DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	LogDebug(message);
}


export class Renderer : public Singleton<Renderer> {
public:
	void Initialise() {
		auto res = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		solidTexture.width = solidTexture.height = 1;
		glGenTextures(1, &solidTexture.id);

		glBindTexture(GL_TEXTURE_2D, solidTexture.id);
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &White);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glGenBuffers(1, &uboProj);
		glGenBuffers(1, &uboScale);
		glGenBuffers(1, &ssboTPages);

		glBindBuffer(GL_UNIFORM_BUFFER, uboProj);
		{
			glBufferData(GL_UNIFORM_BUFFER, sizeof(projectionMatrix), &projectionMatrix, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_UNIFORM_BUFFER, uboScale);
		{
			glBufferData(GL_UNIFORM_BUFFER, sizeof(scale), &scale, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTPages);
		{
			glBufferData(GL_SHADER_STORAGE_BUFFER, 64 * 256 * sizeof(uint16_t) * 32, nullptr, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		InitialiseShaders();

		glBindVertexArray(vao);
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

			glBindVertexBuffer(0, vbo, 0, sizeof(Vert));

			glEnableVertexAttribArray(0);
			glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vert, position));
			glVertexAttribBinding(0, 0);

			glEnableVertexAttribArray(1);
			glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, offsetof(Vert, colour));
			glVertexAttribBinding(1, 0);

			glEnableVertexAttribArray(2);
			glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(Vert, uv));
			glVertexAttribBinding(2, 0);

			glEnableVertexAttribArray(3);
			glVertexAttribIFormat(3, 1, GL_INT, offsetof(Vert, tpage));
			glVertexAttribBinding(3, 0);

			glEnableVertexAttribArray(4);
			glVertexAttribIFormat(4, 1, GL_INT, offsetof(Vert, palette));
			glVertexAttribBinding(4, 0);
		}
		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//glEnable(GL_DITHER);
		glEnable(GL_BLEND);
		glDisable(GL_MULTISAMPLE);
		//glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(4);

		glDebugMessageCallback(DebugMessageCallback, nullptr);
	}

	void SetViewport(int x, int y, int w, int h) {
		glViewport(x, y, w, h);

		projectionMatrix = {
			 2.f / w,  0,       0, 0,
			 0,       -2.f / h, 0, 0,
			 0,        0,       1, 0,
			-1,        1,       0, 1
		};

		glBindBuffer(GL_UNIFORM_BUFFER, uboProj);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projectionMatrix), &projectionMatrix);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		scale[0] = g_RenderScaleX::Get();
		scale[1] = g_RenderScaleY::Get();

		glBindBuffer(GL_UNIFORM_BUFFER, uboScale);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(scale), &scale);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	Texture* LoadTexture(const char* filename) {
		auto t = BorrowTexture();

		auto data = stbi_load(filename, &t->width, &t->height, nullptr, 4);

		glGenTextures(1, &t->id);

		glBindTexture(GL_TEXTURE_2D, t->id);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->width, t->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);

		return t;
	}

	void DeleteTexture(Texture* texture) {
		ReturnTexture(texture);
	}

	void LoadTexturePageData(PSX_RECT* rect, uint8_t* data) {
		if (rect->x + rect->w > 1024
			|| rect->y + rect->h > 512)
			return;

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTPages);

		auto offset = rect->y * 2048 + rect->x * sizeof(uint16_t);
		auto _w = rect->w * sizeof(uint16_t);
		auto _a = 4 * (_w >> 2);
		auto _b = _w & 3;

		for (int i = 0; i < rect->h; ++i) {
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, _a, data);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + _a, _b, &data[_a]);
			offset += 2048;
			data += _w;
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		if (rect->x == 0) {
			auto asd = true;
		}
	}

	void Begin() {
		commandQueue.clear();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void End() {
		for (auto* cmd : commandQueue) {
			switch (cmd->type) {
				case DrawCommandType::Psx:
					psxShader.Use();

					break;

				case DrawCommandType::ReplacedTexture:
					replacedTextureShader.Use();
					replacedTextureShader.SetUniformInt32("tex", 0);
					glBindTexture(GL_TEXTURE_2D, cmd->textureId);

					break;

				case DrawCommandType::Text:
					textShader.Use();
					textShader.SetUniformInt32("tex", 0);
					glBindTexture(GL_TEXTURE_2D, cmd->textureId);

					break;
			}

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			{
				glBufferData(GL_ARRAY_BUFFER, cmd->verts.size() * sizeof(Vert), cmd->verts.data(), GL_STATIC_DRAW);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, cmd->indices.size() * sizeof(GLuint), cmd->indices.data(), GL_STATIC_DRAW);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glBindVertexArray(vao);
			{
				if (cmd->type == DrawCommandType::Lines)
					glDrawElements(GL_LINE_STRIP, cmd->indices.size(), GL_UNSIGNED_INT, (void*)0);
				else
					glDrawElements(GL_TRIANGLES, cmd->indices.size(), GL_UNSIGNED_INT, (void*)0);
			}
			glBindVertexArray(0);

			ReturnDrawCommand(cmd);
		}
	}

	//void DrawTexturedQuad(
	//	Texture* texture,
	//	const Vec3f& v1, const Vec4b& c1, const Vec2f& uv1,
	//	const Vec3f& v2, const Vec4b& c2, const Vec2f& uv2,
	//	const Vec3f& v3, const Vec4b& c3, const Vec2f& uv3,
	//	const Vec3f& v4, const Vec4b& c4, const Vec2f& uv4) {
	//
	//	basicShader.Use();
	//
	//	Vert verts[] = {
	//		{ v1, { c1.r / 255.f, c1.g / 255.f, c1.b / 255.f, c1.a / 255.f }, uv1 },
	//		{ v2, { c2.r / 255.f, c2.g / 255.f, c2.b / 255.f, c2.a / 255.f }, uv2 },
	//		{ v3, { c3.r / 255.f, c3.g / 255.f, c3.b / 255.f, c3.a / 255.f }, uv3 },
	//		{ v4, { c4.r / 255.f, c4.g / 255.f, c4.b / 255.f, c4.a / 255.f }, uv4 }
	//	};
	//
	//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//	{
	//		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	//	}
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//
	//	GLuint indices[] = {
	//		0, 1, 2,
	//		2, 1, 3,
	//	};
	//
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//	{
	//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//	}
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//
	//	glBindTexture(GL_TEXTURE_2D, texture->id);
	//
	//	glBindVertexArray(vao);
	//	{
	//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//		glBindVertexBuffer(0, vbo, 0, sizeof(Vert));
	//
	//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//	}
	//	glBindVertexArray(0);
	//}

	GLuint GetReplacementTextureId(uint16_t tpage, uint16_t palette) {
		auto key = ((uint64_t)tpage << 32) | palette;

		if (replacementTextures.contains(key))
			return replacementTextures[key]->id;

		const auto& filename = std::format("NewData\\Textures\\texture_{}_{}.png", tpage, palette);

		if (std::filesystem::exists(filename)) {
			replacementTextures[key] = LoadTexture(filename.c_str());

			return replacementTextures[key]->id;
		}

		return -1;
	}

	void ProcessPrim_Tri(GpuPrim_Tri* p) {

	}

	void ProcessPrim_QuadTexture(GpuPrim_QuadTexture* p) {
		auto rTexId = GetReplacementTextureId(p->texturePage, p->palette);

		DrawCommand* cmd;

		if (rTexId != -1)
			cmd = CreateOrGetDrawCommand(DrawCommandType::ReplacedTexture, rTexId);
		else
			cmd = CreateOrGetDrawCommand(DrawCommandType::Psx);

		Vec4f c = { p->colour.r / 255.f, p->colour.g / 255.f, p->colour.b / 255.f, 255.f };

		auto count = cmd->verts.size();

		cmd->verts.push_back({ p->v1, c, { (float)p->t1.x, (float)p->t1.y }, p->texturePage, p->palette });
		cmd->verts.push_back({ p->v2, c, { (float)p->t2.x, (float)p->t2.y }, p->texturePage, p->palette });
		cmd->verts.push_back({ p->v3, c, { (float)p->t3.x, (float)p->t3.y }, p->texturePage, p->palette });
		cmd->verts.push_back({ p->v4, c, { (float)p->t4.x, (float)p->t4.y }, p->texturePage, p->palette });

		cmd->indices.push_back(count); cmd->indices.push_back(count + 1); cmd->indices.push_back(count + 2);
		cmd->indices.push_back(count + 2); cmd->indices.push_back(count + 1); cmd->indices.push_back(count + 3);
	}

	void ProcessPrim_QuadGouraud(GpuPrim_QuadGouraud* p) {
		auto* cmd = CreateOrGetDrawCommand(DrawCommandType::Psx);;

		Vec4f c1 = { p->colour.r / 255.f, p->colour.g / 255.f, p->colour.b / 255.f, 255.f };
		Vec4f c2 = { p->c2.r / 255.f, p->c2.g / 255.f, p->c2.b / 255.f, 255.f };
		Vec4f c3 = { p->c3.r / 255.f, p->c3.g / 255.f, p->c3.b / 255.f, 255.f };
		Vec4f c4 = { p->c4.r / 255.f, p->c4.g / 255.f, p->c4.b / 255.f, 255.f };

		auto count = cmd->verts.size();

		cmd->verts.push_back({ p->v1, c1, { 0, 0 }, -1, 0 });
		cmd->verts.push_back({ p->v2, c2, { 0, 0 }, -1, 0 });
		cmd->verts.push_back({ p->v3, c3, { 0, 0 }, -1, 0 });
		cmd->verts.push_back({ p->v4, c4, { 0, 0 }, -1, 0 });

		cmd->indices.push_back(count); cmd->indices.push_back(count + 1); cmd->indices.push_back(count + 2);
		cmd->indices.push_back(count + 2); cmd->indices.push_back(count + 1); cmd->indices.push_back(count + 3);
	}

	void ProcessPrim_RectWHTexture(GpuPrim_RectWHTexture* p) {
		auto rTexId = GetReplacementTextureId(g_DrawEnv::Get().tpage, p->palette);

		DrawCommand* cmd;

		if (rTexId != -1)
			cmd = CreateOrGetDrawCommand(DrawCommandType::ReplacedTexture, rTexId);
		else
			cmd = CreateOrGetDrawCommand(DrawCommandType::Psx);

		Vec4f c = { p->colour.r / 255.f, p->colour.g / 255.f, p->colour.b / 255.f, 255.f };
		float x = p->v1.x;
		float y = p->v1.y;
		float z = p->v1.z;
		auto tx = static_cast<float>(p->t1.x);
		auto ty = static_cast<float>(p->t1.y);
		uint16_t w = p->size.x;
		uint16_t h = p->size.y;

		auto count = cmd->verts.size();

		cmd->verts.push_back({ { x    , y    , z }, c, { tx,         ty			}, g_DrawEnv::Get().tpage, p->palette });
		cmd->verts.push_back({ { x + w, y    , z }, c, { tx + w - 1, ty			}, g_DrawEnv::Get().tpage, p->palette });
		cmd->verts.push_back({ { x    , y + h, z }, c, { tx,         ty + h - 1 }, g_DrawEnv::Get().tpage, p->palette });
		cmd->verts.push_back({ { x + w, y + h, z }, c, { tx + w - 1, ty + h - 1 }, g_DrawEnv::Get().tpage, p->palette });

		cmd->indices.push_back(count); cmd->indices.push_back(count + 1); cmd->indices.push_back(count + 2);
		cmd->indices.push_back(count + 2); cmd->indices.push_back(count + 1); cmd->indices.push_back(count + 3);
	}

	void ProcessPrim_Lines(GpuPrim_Line* p, int count) {
		auto* cmd = CreateOrGetDrawCommand(DrawCommandType::Lines);

		Vec4f c = { p->colour.r / 255.f, p->colour.g / 255.f, p->colour.b / 255.f, 255.f };

		cmd->verts.push_back({ { p->v1.x, p->v1.y, p->v1.z }, c, { 0, 0 }, -1, 0 });
		cmd->verts.push_back({ { p->v2.x, p->v2.y, p->v2.z }, c, { 0, 0 }, -1, 0 });

		cmd->indices.push_back(0); cmd->indices.push_back(1);

		if (count > 1) {
			auto* _p = reinterpret_cast<GpuPrim_TwoLines*>(p);

			cmd->verts.push_back({ { _p->v3.x, _p->v3.y, _p->v3.z }, c, { 0, 0 }, -1, 0 });

			cmd->indices.push_back(2);
		}

		if (count > 2) {
			auto* _p = reinterpret_cast<GpuPrim_ThreeLines*>(p);

			cmd->verts.push_back({ { _p->v4.x, _p->v4.y, _p->v4.z }, c, { 0, 0 }, -1, 0 });

			cmd->indices.push_back(3);
		}
	}

	void ProcessPrim_TextGlyph(GpuPrim_TextGlyph* p) {
		const auto& glyphMgr = GlGlyphManager::Get();
		auto& glyphUv = glyphMgr.GetGlyphUv(p->charCode);

		auto* cmd = CreateOrGetDrawCommand(DrawCommandType::Text, glyphMgr.GetTextureId());

		auto width = p->v4.x - p->v1.x;
		auto scale = width / 12.f;

		Vec4f c = { p->colour.r / 255.f, p->colour.g / 255.f, p->colour.b / 255.f, 255.f };
		float x1 = p->v1.x + 2.0f;
		float y1 = p->v1.y - 1.5f;
		float x2 = p->v1.x + 2.0f + floor(glyphMgr.GetCellWidth() / g_RenderScaleX::Get()) * scale;
		float y2 = p->v1.y - 1.5f + floor(glyphMgr.GetCellHeight() / g_RenderScaleY::Get()) * scale;
		float z = 0.99000001f;
		auto tx1 = glyphUv.x;
		auto ty1 = glyphUv.y;
		auto tx2 = glyphUv.z;
		auto ty2 = glyphUv.w;

		auto count = cmd->verts.size();

		cmd->verts.push_back({ { x1, y1, z }, c, { tx1, ty1 }, 0, 0 });
		cmd->verts.push_back({ { x2, y1, z }, c, { tx2, ty1 }, 0, 0 });
		cmd->verts.push_back({ { x2, y2, z }, c, { tx2, ty2 }, 0, 0 });
		cmd->verts.push_back({ { x1, y2, z }, c, { tx1, ty2 }, 0, 0 });

		cmd->indices.push_back(count); cmd->indices.push_back(count + 1); cmd->indices.push_back(count + 2);
		cmd->indices.push_back(count + 2); cmd->indices.push_back(count + 3); cmd->indices.push_back(count);
	}

	void ProcessPrim_Sprite(GpuPrim_Sprite* p) {
		auto* cmd = CreateOrGetDrawCommand(DrawCommandType::Psx);

		Vec4f c = { p->colour.r / 255.f, p->colour.g / 255.f, p->colour.b / 255.f, 255.f };

		for (int i = p->blockIndex; i < p->blockIndex + p->blockCount; ++i) {
			auto& block = g_TextureBlockInfos::At(i);

			auto w = 8 * (block.size & 0xF);
			auto h = (block.size >> 1) & 0x78;

			float x1, x2;

			if (p->texturePage & 0x400) {
				x1 = p->position.x - block.relX;
				x2 = x1 - w;
			} else {
				x1 = p->position.x + block.relX;
				x2 = x1 + w;
			}

			auto tx1 = (int)block.tx;
			auto tx2 = tx1 + w;

			if (block.texturePage & 0x200)
				std::swap(tx1, tx2);

			auto y1 = p->position.y + block.relY;
			auto y2 = y1 + h;

			auto ty1 = (int)block.ty;
			auto ty2 = ty1 + h;

			auto count = cmd->verts.size();

			//LogDebug("(w: %i, h: %i) tx: %i - %i, ty: %i - %i\n", w, h, tx1, tx2, ty1, ty2);

			cmd->verts.push_back({ { x1, y1, 0.99f }, c, { (float)tx1, (float)ty1 }, block.texturePage, p->palette });
			cmd->verts.push_back({ { x2, y1, 0.99f }, c, { (float)tx2, (float)ty1 }, block.texturePage, p->palette });
			cmd->verts.push_back({ { x2, y2, 0.99f }, c, { (float)tx2, (float)ty2 }, block.texturePage, p->palette });
			cmd->verts.push_back({ { x1, y2, 0.99f }, c, { (float)tx1, (float)ty2 }, block.texturePage, p->palette });

			cmd->indices.push_back(count); cmd->indices.push_back(count + 1); cmd->indices.push_back(count + 2);
			cmd->indices.push_back(count + 2); cmd->indices.push_back(count + 3); cmd->indices.push_back(count);
		}
	}

	//void DrawQuad(
	//	const Vec3f& v1, const Vec4b& c1,
	//	const Vec3f& v2, const Vec4b& c2,
	//	const Vec3f& v3, const Vec4b& c3,
	//	const Vec3f& v4, const Vec4b& c4) {
	//
	//	DrawTexturedQuad(&solidTexture, v1, c1, { 0, 0 }, v2, c2, { 0, 0 }, v3, c3, { 0, 0 }, v4, c4, { 0, 0 });
	//}

	//void DrawTexture(
	//	Texture* texture,
	//	const Rectf& rect,
	//	const Vec4b& c1, const Vec4b& c2, const Vec4b& c3, const Vec4b& c4) {
	//
	//	DrawTexturedQuad(
	//		texture,
	//		{ rect.x,          rect.y, 0 }, c1, { 0.f, 1.f },
	//		{ rect.x + rect.w, rect.y, 0 }, c2, { 1.f, 1.f },
	//		{ rect.x,          rect.y + rect.h, 0 }, c3, { 0.f, 0.f },
	//		{ rect.x + rect.w, rect.y + rect.h, 0 }, c4, { 1.f, 0.f }
	//	);
	//}

	//void DrawTexture(Texture* texture, const Rectf& rect, const Vec4b& colour = White) {
	//	DrawTexture(texture, rect, colour, colour, colour, colour);
	//}

	//void DrawRectangle(const Rectf& rect, const Vec4b& colour = White) {
	//	DrawTexture(&solidTexture, rect, colour);
	//}

	//void DrawRectangle(const Rectf& rect, const Vec4b& c1, const Vec4b& c2, const Vec4b& c3, const Vec4b& c4) {
	//	DrawTexture(&solidTexture, rect, c1, c2, c3, c4);
	//}

private:
	void __forceinline InitialiseShader(ShaderProgram& shader, int vsId, int fsId) {
		shader.Compile(vsId, fsId);
		shader.SetUniformBlockBuffer("ProjectionMatrix", BufferType::Uniform, 0, uboProj);
		shader.SetUniformBlockBuffer("Scale", BufferType::Uniform, 1, uboScale);
	}

	void __forceinline InitialiseShaders() {
		InitialiseShader(basicShader, IDB_SHADER_BASIC_VS, IDB_SHADER_BASIC_FS);
		InitialiseShader(psxShader, IDB_SHADER_PSX_VS, IDB_SHADER_PSX_FS);
		InitialiseShader(replacedTextureShader, IDB_SHADER_REPLACED_TEXTURE_VS, IDB_SHADER_REPLACED_TEXTURE_FS);
		InitialiseShader(textShader, IDB_SHADER_TEXT_VS, IDB_SHADER_TEXT_FS);

		psxShader.SetUniformBlockBuffer("TexturePages", BufferType::ShaderStorage, 2, ssboTPages);
	}

	Texture* BorrowTexture() {
		if (texturePool.empty())
			return new Texture;

		auto t = texturePool.top();
		texturePool.pop();

		return t;
	}

	void ReturnTexture(Texture* t) {
		glDeleteTextures(1, &t->id);
		texturePool.push(t);
	}

	DrawCommand* BorrowDrawCommand() {
		if (commandPool.empty())
			return new DrawCommand;

		auto c = commandPool.top();
		commandPool.pop();

		return c;
	}

	void ReturnDrawCommand(DrawCommand* c) {
		c->textureId = 0;
		c->verts.clear();
		c->indices.clear();

		commandPool.push(c);
	}

	DrawCommand* CreateOrGetDrawCommand(DrawCommandType type, int textureId = -1) {
		DrawCommand* cmd;

		if (commandQueue.size() > 0) {
			auto* lastCmd = commandQueue[commandQueue.size() - 1];

			if ((lastCmd->type == type && (type == DrawCommandType::Psx || type == DrawCommandType::Text))
				|| (lastCmd->type == type && type == DrawCommandType::ReplacedTexture && lastCmd->textureId == textureId)) {
				cmd = lastCmd;
			} else {
				cmd = BorrowDrawCommand();
				commandQueue.push_back(cmd);
			}
		} else {
			cmd = BorrowDrawCommand();
			commandQueue.push_back(cmd);
		}

		cmd->type = type;
		cmd->textureId = textureId;

		return cmd;
	}


	Mat4f projectionMatrix;
	float scale[2];
	ShaderProgram basicShader, psxShader, replacedTextureShader, textShader;
	GLuint vao, vbo, ebo;
	GLuint uboProj, uboScale, ssboTPages;
	Texture solidTexture;
	std::stack<Texture*> texturePool;
	std::stack<DrawCommand*> commandPool;
	std::vector<DrawCommand*> commandQueue;
};