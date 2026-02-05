in = -I./include -std=c++17 -Wall

all: bin/ae3d.exe

bin/ae3d.exe: obj/main.o obj/window.o obj/global.o obj/sync.o
	g++ obj/*.o obj/etc/glad.o -o bin/ae3d.exe -ljsoncpp -lglfw3 -lopengl32 -lgdi32

obj/main.o: src/main.cpp
	g++ -c src/main.cpp -o obj/main.o $(in)

obj/window.o: src/ae/window.cpp
	g++ -c src/ae/window.cpp -o obj/window.o $(in)

obj/global.o: src/ae/global.cpp
	g++ -c src/ae/global.cpp -o obj/global.o $(in)

obj/sync.o: src/ae/sync.cpp
	g++ -c src/ae/sync.cpp -o obj/sync.o $(in)

clean:
	rm -rf bin/ae3d.exe obj/*.o