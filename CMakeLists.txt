project(CG-Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

link_directories(${CMAKE_SOURCE_DIR}/lib)

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

# Set ASSIMP paths for Homebrew installation
set(ASSIMP_INCLUDE_DIR "/opt/homebrew/include")
set(ASSIMP_LIBRARY "/opt/homebrew/lib/libassimp.dylib")

find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

find_package(PkgConfig REQUIRED)
pkg_search_module(GLEW REQUIRED glew)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})

if(APPLE)
  find_package(Freetype REQUIRED)
  message(STATUS "Found Freetype in ${FREETYPE_INCLUDE_DIRS}")
endif(APPLE)

# GLFW configuration - direct path
set(GLFW_INCLUDE_DIR "/opt/homebrew/Cellar/glfw/3.4/include")
set(GLFW_LIBRARY "/opt/homebrew/Cellar/glfw/3.4/lib/libglfw.3.4.dylib")

# Remove pkg-config GLFW check
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(GLFW REQUIRED glfw3>=3.4)

# Add GLFW include directory before any other includes
include_directories(BEFORE ${GLFW_INCLUDE_DIR})
link_directories(/opt/homebrew/Cellar/glfw/3.4/lib)

# Add GLFW version definition
add_definitions(-DGLFW_INCLUDE_NONE)
add_definitions(-DGLFW_VERSION_3_4)

if(WIN32)
  set(LIBS glfw3 opengl32 assimp freetype irrKlang)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  set(LIBS ${GLFW_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype ${ASSIMP_LIBRARY})
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
  add_definitions(-DGL_SILENCE_DEPRECATION)
  include_directories(/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/GLUT.framework/Headers)
  include_directories(/opt/homebrew/include)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  FIND_LIBRARY(GLUT_LIBRARY GLUT)
  set(FREEIMAGE_LIBRARY "/opt/homebrew/lib/libfreeimage.dylib")
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY IOKit_LIBRARY CoreVideo_LIBRARY GLUT_LIBRARY FREEIMAGE_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY} ${GLUT_LIBRARY} ${FREEIMAGE_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE_LIBRARIES})
  set(LIBS ${APPLE_LIBS})
endif()

# 确保 GLEW 静态库始终在最前面
set(LIBS ${GLEW_LIBRARIES} ${LIBS})

# 只保留 GL-Project
set(MY_PROJECTS
    GL-Project/GL-Project
)

# 创建必要的静态库
add_library(STB_IMAGE "src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

macro(makeLink src dest target)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}  DEPENDS  ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()

function(create_myproject_from_sources src_path)
    get_filename_component(demo_dir ${src_path} DIRECTORY)
    get_filename_component(demo_name ${demo_dir} NAME)
    set(NAME "${demo_name}")

    file(GLOB_RECURSE SOURCE
        "src/${demo_dir}/include/*.h"
        "src/${demo_dir}/include/*.hpp"
        "src/${demo_dir}/src/*.cpp"
        "src/${demo_dir}/src/objects/*.cpp"
        "src/${demo_dir}/*.h"
        "src/${demo_dir}/*.hpp"
        "src/${demo_dir}/*.cpp"
        "src/${demo_dir}/*.vs"
        "src/${demo_dir}/*.fs"
        "src/${demo_dir}/*.tcs"
        "src/${demo_dir}/*.tes"
        "src/${demo_dir}/*.gs"
        "src/${demo_dir}/*.cs"
    )

    include_directories(
        ${CMAKE_SOURCE_DIR}/src/${demo_dir}/include
        ${CMAKE_SOURCE_DIR}/src/${demo_dir}/src
        ${CMAKE_SOURCE_DIR}/src/${demo_dir}/include/objects
    )

    message(STATUS "Found source files for ${NAME}:")
    foreach(src ${SOURCE})
        message(STATUS "  ${src}")
    endforeach()

    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} ${LIBS})
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${demo_name}")

    # 复制着色器文件
    file(GLOB SHADER_FILES
        "src/${demo_dir}/*.glsl"
        "src/${demo_dir}/*.vs"
        "src/${demo_dir}/*.fs"
        "src/${demo_dir}/*.tcs"
        "src/${demo_dir}/*.tes"
        "src/${demo_dir}/*.gs"
        "src/${demo_dir}/*.cs"
    )
    foreach(SHADER ${SHADER_FILES})
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SHADER}
            $<TARGET_FILE_DIR:${NAME}>
        )
    endforeach()

    # 复制资源文件夹
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/${demo_dir}/shaders")
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/src/${demo_dir}/shaders"
            "$<TARGET_FILE_DIR:${NAME}>/shaders"
        )
    endif()

    if(EXISTS "${CMAKE_SOURCE_DIR}/src/${demo_dir}/res")
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/src/${demo_dir}/res"
            "$<TARGET_FILE_DIR:${NAME}>/res"
        )
    endif()

    if(EXISTS "${CMAKE_SOURCE_DIR}/src/${demo_dir}/textures")
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/src/${demo_dir}/textures"
            "$<TARGET_FILE_DIR:${NAME}>/textures"
        )
    endif()

    # 复制其他资源文件
    file(GLOB RESOURCES
        "src/${demo_dir}/*.obj"
        "src/${demo_dir}/*.jpg"
        "src/${demo_dir}/*.png"
        "src/${demo_dir}/*.ppm"
    )
    foreach(RES ${RESOURCES})
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${RES}
            $<TARGET_FILE_DIR:${NAME}>
        )
    endforeach()

    if(MSVC)
        configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
    endif(MSVC)
endfunction()

# 只构建 GL-Project
foreach(PROJ_SRC ${MY_PROJECTS})
    create_myproject_from_sources(${PROJ_SRC})
endforeach()

include_directories(${CMAKE_SOURCE_DIR}/includes)
include_directories(${CMAKE_SOURCE_DIR}/src/GL-Project/include)

