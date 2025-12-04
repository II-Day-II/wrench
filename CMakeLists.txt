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

# the default FindVulkan module currently doesn't export slang,
# so we'll find it manually 
set(_Slang_hint_include_paths 
    "$ENV{VULKAN_SDK}/include"
)
set(_Slang_hint_exe_paths
    "$ENV{VULKAN_SDK}/bin"
)
set(_Slang_hint_lib_paths
    "$ENV{VULKAN_SDK}/lib"
    "$ENV{VULKAN_SDK}/bin"
)
find_path(Slang_INCLUDE_DIR
    NAMES slang/slang.h
    HINTS
    ${_Slang_hint_include_paths}
)
mark_as_advanced(Slang_INCLUDE_DIR)
find_library(Slang_LIBRARY
    NAMES slang
    HINTS
    ${_Slang_hint_lib_paths}
)
mark_as_advanced(Slang_LIBRARY)
find_library(Slang_DEBUG_LIBRARY
    NAMES slangd
    HINTS
    ${_Slang_hint_lib_paths}
)
mark_as_advanced(Slang_DEBUG_LIBRARY)
if((Slang_LIBRARY OR Slang_DEBUG_LIBRARY) AND NOT TARGET slang)
    add_library(slang STATIC IMPORTED)
    set_property(TARGET slang
        PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${Slang_INCLUDE_DIR}")
    if(Slang_LIBRARY)
        set_property(TARGET slang APPEND
            PROPERTY
            IMPORTED_CONFIGURATIONS Release)
        set_property(TARGET slang APPEND
            PROPERTY
            IMPORTED_LOCATION_RELEASE "${Slang_LIBRARY}")
        message(STATUS "Found Slang Release mode")
    endif()
    if(Slang_DEBUG_LIBRARY)
        set_property(TARGET slang APPEND
            PROPERTY
            IMPORTED_CONFIGURATIONS Debug)
        set_property(TARGET slang APPEND
            PROPERTY
            IMPORTED_LOCATION_DEBUG "${Slang_DEBUG_LIBRARY}")
        message(STATUS "Found Slang Debug mode")
    endif()
    if(UNIX)
        find_package(Threads REQUIRED)
        target_link_libraries(slang INTERFACE Threads::Threads)
    endif()
else()
    message(FATAL "Failed to find Slang")
endif()

# specify project files
# headers
file(GLOB_RECURSE HEADER_FILES 
	CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

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

# implementation files
file(GLOB_RECURSE SOURCE_FILES
	CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

function(assign_source_group arg1)
    foreach(_source IN ITEMS ${ARGN})
        get_filename_component(PARENT_DIR "${_source}" DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include" "" GROUP "${PARENT_DIR}")
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src" "" GROUP "${GROUP}")
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${GROUP}")
        source_group("${arg1}/${GROUP}" FILES ${_source})
    endforeach()
endfunction(assign_source_group)
assign_source_group("Sources" ${SOURCE_FILES})
assign_source_group("Shaders" ${SHADER_FILES})
assign_source_group("Headers" ${HEADER_FILES})

# set up the target executable
add_executable(${EXECUTABLE_NAME})
target_sources(${EXECUTABLE_NAME} 
    PRIVATE ${SOURCE_FILES} ${INLINE_FILES} ${SHADER_FILES} ${HEADER_FILES}
)
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_20)
target_compile_options(${EXECUTABLE_NAME} PRIVATE
	/W4 /WX /external:W0 /external:anglebrackets /analyze:external-
	-DNOMINMAX
	-D_HAS_EXCEPTIONS=0
#	-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#	-D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
)
# link third party libraries via interface
target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    third_party
    slang
)


set_target_properties(${EXECUTABLE_NAME} PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")
