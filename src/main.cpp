#include <glad/glad.h>
#include <ae/window.hpp>

#include <ae/font.hpp>

using namespace ae;

int main(int argc, char* argv[])
{
	Window win("res/game.json", argc, argv);

	ae::text::Font f;
	f.load("envell", win.getCamera());

	u32 vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	usize len = f.build("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ");

	while (win.isOpen())
	{
		win.update();
		win.clear();
		win.getCamera()->clear();
		win.getCamera()->useProjection(false);
		win.getCamera()->shaderUse("text");
		win.getCamera()->textureUse("fonts/envell");
		win.getCamera()->drawText(vbo, len);
		win.getCamera()->display();
		win.getUI()->render();
		win.display();
		// win.render();
	}
	return 0;
}