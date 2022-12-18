
LIBS = -lm -lpthread -lGL -lGLU -lglfw -lX11 -lXxf86vm -lXrandr -lXi -ldl -lGLEW


CXXFLAGS = -ggdb3 -m64 -std=c++17 -Wint-to-pointer-cast -Wpointer-arith -DGLEW_STATIC
INCLUDES = -I../../thirdparty -I../..
CXX = g++

main.exe: bin/Main.o bin/OpenGL.o bin/Shader.o bin/Texture.o bin/VBO.o bin/VAO.o bin/SOIL2.o bin/etc1_utils.o bin/image_helper.o bin/image_DXT.o
	g++ $(CXXFLAGS $(INCLUDES)) $(LIBS) -o $@ $^ $(LIBS)

bin/%.o: ../../%.cpp
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin/%.o: %.c
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin/%.o: %.cpp
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin/%.o: ../../thirdparty/soil2/%.c
	gcc -c $(CXXFLAGS) $(INCLUDES) -o $@ $<


.PHONY: clean
clean:
	rm bin/*.o main.exe


REGE:
	rem g++ -IC:/Programs/mingw-w64/include -I../../thirdparty Main.cpp -o main.exe -std=c++17 -LC:/Programs/mingw-w64/lib -LC:/Programs/mingw-w64/shared 
