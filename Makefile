FLAGS = -g

in = -I./include -std=c++17 -Wall $(FLAGS)

ifeq ($(OS),Windows_NT)
	libs = -lopengl32 -lgdi32 -lws2_32
	src = src/ae/networkWIN.cpp
	ext = .exe
else
	libs = ""
	src = src/ae/networkUNIX.cpp
	ext =
endif

all: bin/ae3d$(ext)

bin/server$(ext):\
		obj/server.o obj/network.o obj/global.o
	g++ $^ -o bin/server$(ext) -ljsoncpp $(libs)

bin/ae3d$(ext):\
		obj/main.o obj/window.o obj/global.o obj/sync.o obj/network.o \
		obj/ui.o obj/bind.o obj/camera.o obj/font.o obj/mesh.o \
		obj/world.o obj/gltf.o
	g++ $^ obj/etc/glad.o -o bin/ae3d$(ext) -ljsoncpp -lglfw3 -llua $(libs)

obj/main.o: src/main.cpp
	g++ -c src/main.cpp -o obj/main.o $(in)

obj/server.o: src/server.cpp
	g++ -c src/server.cpp -o obj/server.o $(in)

obj/etc/glad.o: src/glad.c
	gcc -c src/glad.c -o obj/etc/glad.o

obj/network.o: $(src)
	g++ -c $(src) -o obj/network.o $(in)

obj/%.o: src/ae/%.cpp
	g++ -c $^ -o $@ $(in)

clean:
	rm -rf bin/*.exe obj/*.o