include(hello_imgui_add_app)
hello_imgui_add_app(hello_idbfs hello_idbfs.main.cpp)
if(EMSCRIPTEN)
    target_link_options(hello_idbfs PUBLIC -lidbfs.js)
endif()
