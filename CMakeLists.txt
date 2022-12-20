cmake_minimum_required(VERSION 3.8)
project(OpenGLWrapper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_EXAMPLES "Build examples" ON)

include_directories(./include/)
aux_source_directory(./include/openglwrapper/ header_files)

add_library(OpenGLWrapper SHARED
	${header_files}
	src/OpenGL.cpp
	src/Texture.cpp
	src/Shader.cpp
	src/VAO.cpp
	src/VBO.cpp
	thirdparty/soil2/SOIL2.c
	thirdparty/soil2/etc1_utils.c
	thirdparty/soil2/image_helper.c
	thirdparty/soil2/image_DXT.c
)



if(BUILD_EXAMPLES)
	add_executable(samples
		samples/Samples
		samples/Camera
		samples/Simple/Main
		samples/SimpleCompute/Main
		samples/ComplexCompute/Main
		samples/DrawMultiIndirectBuffer/Main
		samples/Particles_1/Main
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






