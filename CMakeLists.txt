cmake_minimum_required(VERSION 3.24)

project(Wrench
	VERSION 0.1.0
	DESCRIPTION "A Vulkan renderer for experimentation"
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(EXECUTABLE_NAME ${PROJECT_NAME})

# set output directories (makes dylibs go into build dir)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# multiprocess build if on msvc
if(MSVC)
	add_definitions(/MP)
endif()

# add third party folder
# EXCLUDE_FROM_ALL makes sure its settings dont pollute us
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party EXCLUDE_FROM_ALL)

# add vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

# specify project files
# headers
file(GLOB_RECURSE HEADER_FILES 
	CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)
source_group("Headers" ${HEADER_FILES})

# shader files TODO: remove non-slang stuff
file(GLOB_RECURSE SHADER_FILES 
	CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.slang
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.glsl
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.comp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.frag
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.vert
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.geom
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.rt
)
# shader files arent part of the actual build
set_source_files_properties(${SHADER_FILES}
	PROPERTIES
	VS_TOOL_OVERRIDE
	"None"
)
source_group("Shaders" ${SHADER_FILES})

# implementation files
file(GLOB_RECURSE SOURCE_FILES
	CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
source_group("Source" ${SOURCE_FILES})


# set up the target executable
add_executable(${EXECUTABLE_NAME})
target_sources(${EXECUTABLE_NAME} 
	PRIVATE ${SOURCE_FILES} ${INLINE_FILES} ${SHADER_FILES} 
)
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_20)
target_compile_options(${EXECUTABLE_NAME} PRIVATE
	/W4 /WX /external:W0 /external:anglebrackets /analyze:external-
	-DNOMINMAX
	-D_HAS_EXCEPTIONS=0
#	-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#	-D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
)

#add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${EXECUTABLE_NAME}> $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
#	COMMAND_EXPAND_LISTS
#)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")