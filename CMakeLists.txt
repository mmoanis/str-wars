# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Graphics_Lab3)

find_package(OpenGL REQUIRED)

# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-1c73e35")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
	external/AntTweakBar-1.16/include/
	external/glfw-2.7.6/include/
	external/glm-0.9.4.0/
	external/glew-1.9.0/include/
	external/assimp-3.0.1270/include/
	external/bullet-2.81-rev2613/src/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	GLFW_276
	GLEW_190
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)


# data
add_executable(data
	data/bullet.cpp
	data/bullet.h
	data/DEFS.h
	data/engine.cpp
	data/engine.h
	data/gameobject.cpp
	data/gameobject.h
	data/main.cpp
	data/monster.cpp
	data/monster.h
	data/obstecle.cpp
	data/obstecle.h
	data/player.cpp
	data/player.h
	data/scene.cpp
	data/scene.h
	data/scenemanager.cpp
	data/scenemanager.h
	
	common/shader.cpp
	common/shader.hpp
    common/controls.cpp
	common/controls.hpp
)
target_link_libraries(1DefineTransformationsMatrices
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(data PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data/")
create_target_launcher(data WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data/")


SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET data POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/data${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/data/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

