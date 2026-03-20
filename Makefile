FLAGS = -g

in = -I./include -std=c++17 -Wall $(FLAGS)

ifeq ($(OS),Windows_NT)
	libs = -lopengl32 -lgdi32 -lws2_32
	netSrc = src/ae/networkWIN.cpp
	ext = .exe
else
	libs = ""
	netSrc = src/ae/networkUNIX.cpp
	ext =
endif

all: bin/ae3d$(ext)

server: bin/server$(ext)

bin/server$(ext): obj/server.o \
		obj/ae/network.o obj/ae/global.o obj/ae/sync.o \
		obj/envell/players.o obj/envell/config.o
	g++ $^ -o bin/server$(ext) -llua $(libs)

bin/ae3d$(ext): obj/main.o \
		obj/ae/window.o obj/ae/global.o obj/ae/sync.o obj/ae/network.o \
		obj/ae/ui.o obj/ae/bind.o obj/ae/camera.o obj/ae/font.o obj/ae/mesh.o \
		obj/ae/world.o obj/ae/gltf.o
	g++ $^ obj/etc/glad.o -o bin/ae3d$(ext) -lglfw3 -llua $(libs)

obj/main.o: src/main.cpp
	g++ -c src/main.cpp -o obj/main.o $(in)

obj/server.o: src/server.cpp
	g++ -c src/server.cpp -o obj/server.o $(in)

obj/etc/glad.o: src/glad.c
	gcc -c src/glad.c -o obj/etc/glad.o

obj/ae/network.o: $(netSrc)
	g++ -c $^ -o $@ $(in)

obj/ae/%.o: src/ae/%.cpp
	g++ -c $^ -o $@ $(in)

obj/envell/%.o: src/envell/%.cpp
	g++ -c $^ -o $@ $(in)

clean:
	rm -rf bin/*.exe obj/*.o obj/ae/*.o obj/envell/*.o