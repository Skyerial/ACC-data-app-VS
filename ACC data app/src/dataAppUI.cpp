#include <stdio.h>
#include <string>
#include <windows.h>

#include "dataUITransfer.h"
#include "dataAppUI.h"
#include "dataToFile.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

// imgui includes
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void UIRenderer(std::wstring mydoc_path, ui_data_pair& pair) {
    /////////////////
    // IMGUI stuff //
    /////////////////
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return;
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create fullscreen window with graphics context at 1080p
    GLFWwindow* window = glfwCreateWindow(1920 / 1.5, 1080 / 1.5, "ACC data app (title in progress)", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "error: window creation failed!\n");
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImFont* cascadia = io.Fonts->AddFontFromFileTTF("C:\\Windows\\fonts\\CascadiaMono.ttf", 16);
    if (cascadia == NULL)
    {
        fprintf(stderr, "error: couldn't add font\n");
        return;
    }

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    bool menu_initialized = false;
    bool data_initialized = false;
    bool live_initialized = false;
    ImVec2 menu_win_size = { 0, 0 };
    ImVec2 data_win_size = { 0, 0 };
    ImVec2 live_win_size = { 0, 0 };
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<int> live_laps;

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;

    std::wstring file_to_show = L"";
    /////////////////////
    // end IMGUI stuff //
    /////////////////////

    while (!glfwWindowShouldClose(window))
    {
        /////////////////
        // IMGUI stuff //
        /////////////////
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        /*if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }*/

        // 2.5 our own created windows
        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        // top left window
        {
            if (!menu_initialized)
            {
                ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth / 3, ((float)framebufferHeight / 3) * 2));
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                menu_initialized = true;
            }
            else
            {
                ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth - data_win_size.x, (float)framebufferHeight - live_win_size.y));
                ImGui::SetNextWindowPos(ImVec2(0, 0));
            }
            if (!ImGui::Begin("menu", NULL, window_flags))
            {
                ImGui::End();
                return;
            }
            ImGui::Text("Menu will come here");

            // should read the files only once and create buttons again and again
            // data of files stored in vector of j...
            // if we would do that then we wouldnt need to read the files twice...
            std::wstring data_folder = L"\\ACC app data";
            std::wstring path = mydoc_path + data_folder;
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                // read json into json j
                json j = readFromFile(entry.path());
                // get session from the json j
                int session_number = j["session: "];
                std::string session_s = getSessionType(session_number);
                // create button with session
                // size of the button should be less hardcoded
                if (ImGui::Button(session_s.c_str(), { (float)framebufferWidth - data_win_size.x - 15, 30 }))
                {
                    file_to_show = entry.path();
                }
                // add button to vector, together with j

            }
            menu_win_size = ImGui::GetWindowSize();
            ImGui::End();
        }
        // bottom left window
        {
            if (!live_initialized)
            {
                ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth / 3, (float)framebufferHeight / 3));
                ImGui::SetNextWindowPos(ImVec2(0, ((float)framebufferHeight / 3) * 2));
                live_initialized = true;
            }
            else
            {
                ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth - data_win_size.x, (float)framebufferHeight - menu_win_size.y));
                ImGui::SetNextWindowPos(ImVec2(0, menu_win_size.y));
            }

            if (!ImGui::Begin("live", NULL, window_flags))
            {
                ImGui::End();
                return;
            }
            ImGui::Text("live data will be displayed here");

            if (pair.session_active)
            {
                ImGui::Text("Session being recorded");
                if (live_laps.empty()) // not sure if this is needed, but maybe since back() on empty vector is undefined
                {
                    live_laps.push_back(pair.lapnumber.load());
                }
                if (live_laps.back() != pair.lapnumber.load())
                {
                    live_laps.push_back(pair.lapnumber.load());
                }
                for (auto i : live_laps)
                {
                    ImGui::Text("Completed lap %d", i);
                }
            }

            live_win_size = ImGui::GetWindowSize();
            ImGui::End();
        }
        // right window
        {
            if (!data_initialized)
            {
                ImGui::SetNextWindowSize(ImVec2(((float)framebufferWidth / 3) * 2, (float)framebufferHeight));
                ImGui::SetNextWindowPos(ImVec2((float)framebufferWidth / 3, 0));
                data_initialized = true;
            } 
            else
            {
                ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth - menu_win_size.x, (float)framebufferHeight));
                ImGui::SetNextWindowPos(ImVec2(menu_win_size.x, 0));
            }
            if (!ImGui::Begin("data", NULL, window_flags))
            {
                ImGui::End();
                return;
            }
            ImGui::Text("Data will come here");

            // this doesnt work... but we are getting very close
            if (!file_to_show.empty())
            {
                ImGui::BeginTable(
                    "lap data",
                    5,
                    0,
                    { (float)framebufferWidth - menu_win_size.x, (float)framebufferHeight },
                    (float)framebufferWidth - menu_win_size.x / 5.0f
                );

                ImGui::TableSetupColumn("Lap");
                ImGui::TableSetupColumn("Laptime");
                ImGui::TableSetupColumn("Sector 1");
                ImGui::TableSetupColumn("Sector 2");
                ImGui::TableSetupColumn("Sector 3");
                ImGui::TableHeadersRow();

                json j = readFromFile(file_to_show);
                json laps = j["Laps"];

                for (auto& x : laps.items())
                {
                    //std::cout << "key: " << x.key() << ", value: " << x.value() << '\n';
                    json single_lap = x.value();
                    //std::cout << single_lap.at("laptime: ") << std::endl;
                    ImGui::TableNextColumn();
                    int current_lap = single_lap.at("current lap: ");
                    ImGui::Text("%d", current_lap);
                    ImGui::TableNextColumn();
                    int laptime = single_lap.at("laptime: ");
                    ImGui::Text("%d", laptime);
                    ImGui::TableNextColumn();
                    int sector1 = single_lap.at("sector1: ");
                    ImGui::Text("%d", sector1);
                    ImGui::TableNextColumn();
                    int sector2 = single_lap.at("sector2: ");
                    ImGui::Text("%d", sector2);
                    ImGui::TableNextColumn();
                    int sector3 = single_lap.at("sector3: ");
                    ImGui::Text("%d", sector3);
                }

                ImGui::EndTable();
            }

            data_win_size = ImGui::GetWindowSize();
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
        /////////////////////
        // end IMGUI stuff //
        /////////////////////
    }

    // cleanup IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    // end cleanup IMGUI
}
