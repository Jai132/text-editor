#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

bool SaveTextToFile(const std::string& text, const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return false;
    
    file << text;
    file.close();
    return true;
}

std::string LoadTextFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return "";
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(800, 600, "ImGui Notepad", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // State
    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    // Notepad state
    static char text[1024 * 16] = ""; // 16K text buffer
    std::string filename = "untitled.txt";
    bool unsaved_changes = false;
    
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main window
        {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
            ImGui::Begin("Notepad", NULL, 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_MenuBar);

            // Menu Bar
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New"))
                    {
                        if (unsaved_changes)
                        {
                            // TODO: Add confirmation dialog
                        }
                        text[0] = '\0';
                        filename = "untitled.txt";
                        unsaved_changes = false;
                    }
                    
                    if (ImGui::MenuItem("Open"))
                    {
                        // TODO: Add proper file dialog
                        static char open_filename[128] = "";
                        ImGui::InputText("Filename", open_filename, IM_ARRAYSIZE(open_filename));
                        if (ImGui::Button("Load"))
                        {
                            std::string loaded_text = LoadTextFromFile(open_filename);
                            if (!loaded_text.empty())
                            {
                                strncpy(text, loaded_text.c_str(), sizeof(text) - 1);
                                text[sizeof(text) - 1] = '\0';
                                filename = open_filename;
                                unsaved_changes = false;
                            }
                        }
                    }
                    
                    if (ImGui::MenuItem("Save", "Ctrl+S"))
                    {
                        if (SaveTextToFile(text, filename))
                            unsaved_changes = false;
                    }
                    
                    if (ImGui::MenuItem("Save As"))
                    {
                        // TODO: Add proper file dialog
                        static char save_filename[128] = "";
                        ImGui::InputText("Filename", save_filename, IM_ARRAYSIZE(save_filename));
                        if (ImGui::Button("Save"))
                        {
                            if (SaveTextToFile(text, save_filename))
                            {
                                filename = save_filename;
                                unsaved_changes = false;
                            }
                        }
                    }
                    
                    ImGui::Separator();
                    
                    if (ImGui::MenuItem("Exit"))
                    {
                        if (unsaved_changes)
                        {
                            // TODO: Add confirmation dialog
                        }
                        glfwSetWindowShouldClose(window, true);
                    }
                    
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Edit"))
                {
                    if (ImGui::MenuItem("Cut", "Ctrl+X"))
                    {
                        ImGui::LogToClipboard();
                        ImGui::LogText("%s", text);
                        ImGui::LogFinish();
                        text[0] = '\0';
                        unsaved_changes = true;
                    }
                    
                    if (ImGui::MenuItem("Copy", "Ctrl+C"))
                    {
                        ImGui::LogToClipboard();
                        ImGui::LogText("%s", text);
                        ImGui::LogFinish();
                    }
                    
                    if (ImGui::MenuItem("Paste", "Ctrl+V"))
                    {
                        // TODO: Implement paste
                    }
                    
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::MenuItem("About"))
                    {
                        ImGui::OpenPopup("About ImGui Notepad");
                    }
                    ImGui::EndMenu();
                }

                // Display filename in the menu bar
                ImGui::SameLine(ImGui::GetWindowWidth() - 150);
                ImGui::Text("%s%s", filename.c_str(), unsaved_changes ? "*" : "");
                
                ImGui::EndMenuBar();
            }
            
            // About popup
            if (ImGui::BeginPopupModal("About ImGui Notepad", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("ImGui Notepad");
                ImGui::Separator();
                ImGui::Text("A simple text editor built with Dear ImGui.");
                ImGui::Text("(c) 2025");
                
                if (ImGui::Button("OK", ImVec2(120, 0)))
                    ImGui::CloseCurrentPopup();
                
                ImGui::EndPopup();
            }

            // Text editor area
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
            bool text_changed = ImGui::InputTextMultiline(
                "##editor", 
                text, 
                IM_ARRAYSIZE(text),
                ImVec2(-1.0f, -ImGui::GetFrameHeightWithSpacing() * 2),
                ImGuiInputTextFlags_AllowTabInput
            );
            
            if (text_changed)
                unsaved_changes = true;
                
            ImGui::PopStyleVar();

            // Status bar
            ImGui::Separator();
            int line_count = 1;
            for (int i = 0; text[i]; i++)
                if (text[i] == '\n')
                    line_count++;
                    
            ImGui::Text("Lines: %d | Characters: %d", line_count, strlen(text));
            
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
