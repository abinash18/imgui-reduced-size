add_library(IMPLOT STATIC
	implot.h
    implot_internal.h
    implot.cpp
    implot_demo.cpp
    implot_items.cpp)
	target_link_libraries(IMPLOT PUBLIC imgui)