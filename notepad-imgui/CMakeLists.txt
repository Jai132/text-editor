cmake_minimum_required(VERSION 3.10)
project(ImguiNotepad VERSION 1.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# ImGui source files
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)



#for glfw3
set(GLFW3_DIR "/usr/lib/x86_64-linux-gnu/cmake/glfw3/glfw3Config.cmake")



# First create an imgui library
add_library(imgui STATIC ${IMGUI_SOURCES})

# Include directories for ImGui library
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${GLFW3_INCLUDE_DIRS}
)

# Create the executable
add_executable(ImGuiNotepad ${CMAKE_SOURCE_DIR}/main.cc)

# Include directories for the main executable
target_include_directories(ImGuiNotepad PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${GLFW3_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(ImGuiNotepad PRIVATE
    imgui
    glfw
    ${OPENGL_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

# Platform specific settings
if(APPLE)
    target_link_libraries(ImGuiNotepad PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
    )
elseif(UNIX AND NOT APPLE)
    # Linux specific libraries
    find_package(X11 REQUIRED)
    target_link_libraries(ImGuiNotepad PRIVATE
        ${X11_LIBRARIES}
        pthread
    )
endif()

# Installation
install(TARGETS ImGuiNotepad
    RUNTIME DESTINATION bin
)
