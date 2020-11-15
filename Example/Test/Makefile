
S = \\

LIBS = -LC:\Programs\mingw-w64\lib -LC:\Programs\mingw-w64\shared     -lglfw3 -lOpenGL32 -lm -lpthread -lglew\glew64s -lwinmm -lgdi32 -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound -lshlwapi -lglu32 -lglfw3 -lshlwapi  -lglfw3 -lOpenGL32 -lm -lpthread -lglew\glew64s -lwinmm -lgdi32 -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound -lshlwapi -lglu32 -lglfw3 -lshlwapi


CXXFLAGS = -m64 -std=c++17 -Wint-to-pointer-cast -Wpointer-arith -DGLEW_STATIC
INCLUDES = -IC:\Programs\mingw-w64\include -I..\..\thirdparty -I..\..
CXX = g++

main.exe: bin\Main.o bin\OpenGL.o bin\Shader.o bin\Texture.o bin\VAO.o bin\SOIL2.o bin\etc1_utils.o bin\image_helper.o bin\image_DXT.o
	g++ $(CXXFLAGS $(INCLUDES)) $(LIBS) -o $@ $^ $(LIBS)

bin$(S)%.o: ..$(S)..$(S)%.cpp
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin$(S)%.o: %.c
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin$(S)%.o: %.cpp
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
bin$(S)%.o: ..$(S)..$(S)thirdparty$(S)soil2$(S)%.c
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<





REGE:
	rem g++ -IC:$(S)Programs$(S)mingw-w64$(S)include -I..$(S)..$(S)thirdparty Main.cpp -o main.exe -std=c++17 -LC:$(S)Programs$(S)mingw-w64$(S)lib -LC:$(S)Programs$(S)mingw-w64$(S)shared 
