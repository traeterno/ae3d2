in = -I./include -std=c++17 -Wall

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
		obj/ui.o obj/bind.o obj/camera.o obj/font.o
	g++ $^ obj/etc/glad.o -o bin/ae3d$(ext) -ljsoncpp -lglfw3 -llua $(libs)



obj/main.o: src/main.cpp
	g++ -c src/main.cpp -o obj/main.o $(in)

obj/server.o: src/server.cpp
	g++ -c src/server.cpp -o obj/server.o $(in)

obj/etc/glad.o: src/glad.c
	gcc -c src/glad.c -o obj/etc/glad.o



obj/window.o: src/ae/window.cpp
	g++ -c src/ae/window.cpp -o obj/window.o $(in)

obj/global.o: src/ae/global.cpp
	g++ -c src/ae/global.cpp -o obj/global.o $(in)

obj/sync.o: src/ae/sync.cpp
	g++ -c src/ae/sync.cpp -o obj/sync.o $(in)

obj/ui.o: src/ae/ui.cpp
	g++ -c src/ae/ui.cpp -o obj/ui.o $(in)

obj/bind.o: src/ae/bind.cpp
	g++ -c src/ae/bind.cpp -o obj/bind.o $(in)

obj/camera.o: src/ae/camera.cpp
	g++ -c src/ae/camera.cpp -o obj/camera.o $(in)

obj/font.o: src/ae/font.cpp
	g++ -c src/ae/font.cpp -o obj/font.o $(in)

obj/network.o: $(src)
	g++ -c $(src) -o obj/network.o $(in)



clean:
	rm -rf bin/*.exe obj/*.o