
LIBS = -lgdi32 -LC:\Programs\mingw-w64\lib -LC:\Programs\mingw-w64\shared -lOpenGL32 -lm -lpthread -lglew\glew64s -lwinmm -lgdi32 -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound -lshlwapi -lglu32 -lshlwapi -lOpenGL32 -lm -lpthread -lglew\glew64s -lwinmm -lgdi32 -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound -lshlwapi -lglu32 -lglfw3 -lshlwapi -lgdi32


CXXFLAGS = -m64 -std=c++17 -Wint-to-pointer-cast -Wpointer-arith -DGLEW_STATIC -s -Ofast
INCLUDES = -IC:\Programs\mingw-w64\include -I..\..\thirdparty -I..\..
CXX = g++

main.exe: bin/Main.o bin/OpenGL.o bin/Shader.o bin/Texture.o bin/VBO.o bin/VAO.o bin/SOIL2.o bin/etc1_utils.o bin/image_helper.o bin/image_DXT.o
	g++ $(CXXFLAGS $(INCLUDES)) $(LIBS) -o $@ $^ $(LIBS)

bin/%.o: ../%.cpp
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin/%.o: ../../%.cpp
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin/%.o: %.c
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin/%.o: %.cpp
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin/%.o: ../../thirdparty/soil2/%.c
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<


.PHONY: clean
clean:
	rm main.exe -f
	rm bin/*.o -f


