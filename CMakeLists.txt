cmake_minimum_required(VERSION 3.2)
project(Wilson-Dashboard)
set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


#set(EMSCRIPTEN ON) # Change this to ON if building for the webclient and use wsl and vscode, this will not work in vs2022
set(BUILD_SHARED_LIBS OFF) # always statically link so you dont need to have a bunch of DLLs with your executable
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(HELLOIMGUI_DOWNLOAD_FREETYPE_IF_NEEDED ON CACHE BOOL "" FORCE)
set(HELLOIMGUI_DOWNLOAD_SDL_IF_NEEDED ON CACHE BOOL "" FORCE)
set(implot_dir ${PROJECT_SOURCE_DIR}/modules/implot)

set(HELLOIMGUI_USE_SDL_OPENGL3 ON CACHE BOOL "" FORCE)

add_subdirectory(modules/hello-imgui)

file(GLOB_RECURSE SOURCE_CXX CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE SOURCE_H CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.h")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_CXX} ${SOURCE_H})


set(HELLOIMGUI_WIN32_NO_CONSOLE OFF)

add_library(IMPLOT 	
    modules/implot/implot.h
    modules/implot/implot_internal.h
    modules/implot/implot.cpp
    modules/implot/implot_demo.cpp
    modules/implot/implot_items.cpp)
target_link_libraries(IMPLOT PUBLIC imgui)
include_directories(modules/implot)

# Build your app
# ==============
hello_imgui_add_app(Dashboard ${SOURCE_CXX} ${SOURCE_H} ${IMPLOT_SRC} ASSETS_LOCATION res/)

target_link_libraries(Dashboard PUBLIC IMPLOT)

target_include_directories(Dashboard PUBLIC
    ${implot_dir}
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Dashboard)

target_precompile_headers(Dashboard PUBLIC src/main_pch.h)
target_compile_definitions(
    Dashboard 
    PUBLIC 
    $<$<CONFIG:Debug>:W_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:W_DEBUG>
    $<$<CONFIG:Release>:W_RELEASE>
    $<$<CONFIG:MinSizeRel>:W_RELEASE>)

if (NOT EMSCRIPTEN)

    # to have open ssl on windows you need to build it from source and install 
    # search online how to do that dont forget to restart ur computer when u add the openssl directory to windows path.
    # on linux u need libssl-dev to be installed.
    find_package(OpenSSL)
    
    set(OPENSSL_USE_STATIC_LIBS TRUE)
    set(OPENSSL_MSVC_STATIC_RT TRUE)
    # I dont think we should ship with a ssl lib statically linked because it is a security risk but this is only an issue on the desktop client.
    target_link_libraries(Dashboard PUBLIC OpenSSL::SSL OpenSSL::Crypto)


    if (MSVC)
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /MT")
    else()
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    endif()


    # Link against pthreads under Unix systems
    if (NOT MSVC AND NOT MINGW)
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    endif()

    # Under MinGW, statically link against the standard libraries
    if (MINGW)
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    endif()

    add_definitions(-D_WEBSOCKETPP_CPP11_STRICT_)
    add_compile_definitions(ASIO_STANDALONE)
    # Add Websocket++
    # Its header only (hpp) therefore we only have to include it not build as lib. Nice!
    include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/modules/asio-1.28.0/include)
    include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/modules/websocketpp)

    add_compile_definitions(W_STANDALONE)
    if(MSVC)
        target_compile_options(Dashboard PUBLIC "/ZI")
        target_link_options(Dashboard PUBLIC "/INCREMENTAL")
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

if (EMSCRIPTEN) 
add_compile_definitions(W_EMSCRIPTEN) 
target_link_libraries(Dashboard PUBLIC websocket.js)
target_link_options(Dashboard PUBLIC 
-Os 
--closure=1 
#-sWASM=0 
-sALLOW_MEMORY_GROWTH=1 
-fno-rtti 
-flto
#-sDISABLE_EXCEPTION_CATCHING=0
#-sWEBSOCKET_DEBUG=1
-sJS_MATH=1
)
#target_link_options(Dashboard PUBLIC 
#)
message("Compiling Emscripten")
endif()
