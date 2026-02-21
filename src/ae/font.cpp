#include <glad/glad.h>
#include <ae/font.hpp>
#include <ae/global.hpp>
#include <ae/camera.hpp>

using namespace ae::text;

Font::Font(const char* id, Texture t)
{
	auto f = ae::fs::readJSON(
		ae::str::format("res/fonts/%s.json", id)
	);
	if (f.empty())
	{
		printf("ERROR: FONT NOT FOUND\n");
		return;
	}

	this->texSize = { t.width, t.height };

	this->height = f["lineHeight"].asFloat();
	this->glyphs.clear();

	for (auto c: f["glyphs"])
	{
		Glyph g;
		g.rect = glm::vec4(
			c["x"].asFloat(), c["y"].asFloat(),
			c["w"].asFloat(), c["h"].asFloat()
		);
		g.offset = glm::vec2(
			c["ox"].asFloat(), c["oy"].asFloat()
		);
		g.advance = c["advance"].asFloat();
		this->glyphs.insert({c["id"].asUInt(), g});
	}
}

Font::~Font()
{
	glyphs.clear();
}

glm::vec3 Font::build(std::string text)
{
	std::vector<glm::vec4> vertices;
	glm::vec2 pos = {0.0, 0.0};
	glm::vec2 bounds = {0.0, 0.0};
	for (usize i = 0; i < text.length(); i++)
	{
		u16 c = 0;
		u8 byte = *reinterpret_cast<u8*>(&text[i]);
		if (byte < 192) { c = byte; }
		else
		{
			c = (byte & 63) << 6;
			c += *reinterpret_cast<u8*>(&text[i + 1]) & 63;
			i++;
		}
		
		if (this->glyphs.find(c) == this->glyphs.end())
		{
			printf("Glyph not found: %i(%c)\n", c, c);
			continue;
		}
		auto g = this->glyphs[c];
		glm::vec4 tl, tr, bl, br;
		tl = {
			pos.x + g.offset.x, pos.y + g.offset.y,
			g.rect.x / this->texSize.x, g.rect.y / this->texSize.y
		};
		tr = {
			pos.x + g.offset.x + g.rect.z, pos.y + g.offset.y,
			(g.rect.x + g.rect.z) / this->texSize.x, g.rect.y / this->texSize.y
		};
		br = {
			pos.x + g.offset.x + g.rect.z, pos.y + g.offset.y + g.rect.w,
			(g.rect.x + g.rect.z) / this->texSize.x, (g.rect.y + g.rect.w) / this->texSize.y
		};
		bl = {
			pos.x + g.offset.x, pos.y + g.offset.y + g.rect.w,
			g.rect.x / this->texSize.x, (g.rect.y + g.rect.w) / this->texSize.y
		};
		vertices.push_back(tl); vertices.push_back(tr); vertices.push_back(br);
		vertices.push_back(br); vertices.push_back(bl); vertices.push_back(tl);
		pos.x += g.advance;
		bounds.x = glm::max(bounds.x, br.x);
		bounds.y = glm::max(bounds.y, br.y);
	}
	glBufferData(
		GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4),
		vertices.data(), GL_STATIC_DRAW
	);
	return {
		vertices.size(),
		bounds.x, bounds.y
	};
}