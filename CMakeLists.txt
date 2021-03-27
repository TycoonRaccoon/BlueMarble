cmake_minimum_required(VERSION 3.12)

project(BlueMarble)

add_executable(BlueMarble main.cpp)

target_include_directories(BlueMarble PRIVATE deps/GLM
											  deps/STB
											  deps/GLFW/include
											  deps/GLEW/include)

target_link_directories(BlueMarble PRIVATE deps/GLFW/lib-vc2019
										   deps/GLEW/lib/release/x64)

target_link_libraries(BlueMarble PRIVATE glfw3.lib
										 glew32.lib
										 opengl32.lib)

add_custom_command(
	TARGET BlueMarble POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/GLEW/bin/Release/x64/glew32.dll" "${CMAKE_BINARY_DIR}/glew32.dll"
	COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/shaders"
	COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/textures" "${CMAKE_BINARY_DIR}/textures"
)

add_executable(Matrices matrices.cpp)
target_include_directories(Matrices PRIVATE deps/GLM)
