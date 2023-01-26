cmake_minimum_required(VERSION 3.8)
project(OpenGLWrapper)
cmake_policy(SET CMP0115 OLD)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_EXAMPLES "Build examples" ON)

add_subdirectory(thirdparty/SOIL2)
add_subdirectory(thirdparty/assimp)

include_directories(./include/)
aux_source_directory(./include/openglwrapper/ header_files)

add_library(OpenGLWrapper SHARED
	${header_files}
	src/OpenGL.cpp
	src/Texture.cpp
	src/Shader.cpp
	src/VAO.cpp
	src/VBO.cpp
	src/AssimpLoader.cpp
)
target_link_libraries(OpenGLWrapper soil2 assimp)



if(BUILD_EXAMPLES)
	add_executable(samples
		samples/DefaultCameraAndOtherConfig
		samples/Camera
		
		samples/AssimpModelWithShading/Main
		samples/Samples
		samples/Simple/Main
		samples/SimpleCompute/Main
		samples/ComplexCompute/Main
		samples/SimpleDrawMultiIndirectBuffer/Main
		samples/Particles_1/Main
		samples/CircleLine/Main
		samples/Texture/Main
	)
	target_link_libraries(samples OpenGLWrapper)
endif()



if(UNIX)
	target_link_libraries(OpenGLWrapper
		m
		pthread
		GL
		GLU
		glfw
		X11
		Xxf86vm
		Xrandr
		Xi
		dl
		GLEW
	)
elseif(WIN32)
	target_link_libraries(OpenGLWrapper
		gdi32
		user32
		ole32
		gdi32
		shlwapi
		winmm
		kernel32
		comdlg32
		m
		pthread
		opengl32
		glfw3
		glew32
		glu32 
	)
	target_link_libraries(OpenGLWrapper
		gdi32
		user32
		ole32
		gdi32
		shlwapi
		winmm
		kernel32
		comdlg32
		m
		pthread
		opengl32
		glfw3
		glew32
		glu32 
	)
else()
	# print error message: not tested platform
endif()






