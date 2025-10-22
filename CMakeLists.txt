cmake_minimum_required(VERSION 3.8)
project(OpenGLWrapper)
cmake_policy(SET CMP0115 OLD)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(OGLW_BUILD_EXAMPLES "Build OpenGLWrapper examples" ON)

add_subdirectory(thirdparty/SOIL2)

set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_USE_CCACHE OFF)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
add_subdirectory(thirdparty/assimp)

include_directories(./include/)
aux_source_directory(./include/openglwrapper/ header_files)
aux_source_directory(./include/openglwrapper/basic_mesh_loader/ header_files_basic_mesh_loader)
aux_source_directory(./src/ source_files)
aux_source_directory(./src/basic_mesh_loader/ source_files_basic_mesh_loader)

add_library(OpenGLWrapper
	${header_files}
	${header_files_basic_mesh_loader}
	${source_files}
	${source_files_basic_mesh_loader}
)
target_link_libraries(OpenGLWrapper soil2 assimp)

if(OGLW_BUILD_EXAMPLES)
	add_executable(samples
		samples/DefaultCameraAndOtherConfig
		samples/Camera
		
		samples/AssimpModelWithShading/Main
		samples/AssimpAnimatedModel/Main
		samples/Samples
		samples/Simple/Main
		samples/SimpleCompute/Main
		samples/ComplexCompute/Main
		samples/SimpleDrawMultiIndirectBuffer/Main
		samples/DrawMultiTestPerformanceVsInstanced/Main
		samples/MultiRenderTargetTextureFBO/Main
		samples/Particles_1/Main
		samples/CircleLine/Main
		samples/Texture/Main
		samples/CameraFBO/Main
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

