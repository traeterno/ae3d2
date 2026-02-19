#ifndef aeFont
#define aeFont

#include <glm/glm.hpp>
#include <ae/types.hpp>
#include <unordered_map>
#include <string>

namespace ae { struct Texture; }

namespace ae::text
{

struct Glyph
{
	glm::vec4 rect;
	glm::vec2 offset;
	f32 advance;
};

class Font
{
public:
	Font(const char* id, Texture t);
	~Font();
	usize build(std::string text);
private:
	f32 height;
	glm::vec2 texSize;
	std::unordered_map<u16, Glyph> glyphs;
};

}

#endif