//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// This file is responsible for the rendering of the app window and its
// subwindows within it.
//////////////////////////////////////////////////////////////////////////////

// .h of this .cpp
#include "dataUITransfer.h" // ui_data_pair
#include "dataAppUI.h"

// C system headers
#include <stdio.h>
#include <windows.h>

// C++ standard library headers
#include <string>
#include <iostream>

// Other libraries
#include "nlohmann/json.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// Own .h
#include "dataToFile.h"

using json = nlohmann::json;

ActiveWindow active_window = Sessions_window; // This is the window that is open
                                              // when the app starts up
std::string active_window_strings[] = {
    "Progression",
    "Sessions",
    "Car/Track combo",
    "Stats",
    "Fuel"
};

// Anonymous namespace used instead of static for local functions and types, 
// since static cant be used to make types local in a translation unit
namespace {
    //////////////////////////////////////////////////////////////////////////////
    // Used for intitializing and declaring the state
    //////////////////////////////////////////////////////////////////////////////
    struct WinState
    {
        bool show_demo_window;
        bool show_another_window;
        bool menu_initialized;
        bool live_initialized;
        bool data_initialized;
        ImVec2 menu_win_size;
        ImVec2 data_win_size;
        ImVec2 live_win_size;
        ImVec4 clear_color;

        // needed to display data in the live window
        std::vector<int> live_laps;
        int consecutive_laps;
        bool was_in_pit;

        ImGuiWindowFlags window_flags;
        std::wstring file_to_show;      // not used atm..
    };

    void InitializeState(WinState& win_state)
    {
        win_state.show_demo_window = false;
        win_state.show_another_window = false;
        win_state.menu_initialized = false;
        win_state.live_initialized = false;
        win_state.data_initialized = false;

        win_state.menu_win_size = { 0,0 };
        win_state.data_win_size = { 0,0 };
        win_state.live_win_size = { 0,0 };
        win_state.clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // needed to display data in the live window
        win_state.consecutive_laps = 0;
        win_state.was_in_pit = false;

        win_state.window_flags = 0;
        win_state.window_flags |= ImGuiWindowFlags_NoTitleBar;
        win_state.window_flags |= ImGuiWindowFlags_NoMove;

        win_state.file_to_show = L"";
    }

    //////////////////////////////////////////////////////////////////////////////
    // Needed for OpenGL and Dear ImGui
    //////////////////////////////////////////////////////////////////////////////
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    //////////////////////////////////////////////////////////////////////////////
    // HELPER function: takes int value for time (ie. laptime or sectortime)
    // and makes it into a good looking string
    //////////////////////////////////////////////////////////////////////////////
    std::string FormatTime(int time)
    {
        using namespace std::chrono;
        auto ms = milliseconds(time);
        auto secs = duration_cast<seconds>(ms);
        ms -= duration_cast<milliseconds>(secs);
        auto mins = duration_cast<minutes>(secs);
        secs -= duration_cast<seconds>(mins);

        std::string time_s = std::format("{:02d}:{:02d}:{:03d}", mins.count(), secs.count(), ms.count());
        return time_s;
    }

    std::string FormatDate(std::string unformatted_date)
    {
        std::string year = unformatted_date.substr(0, 4);
        std::string month = unformatted_date.substr(4, 2);
        std::string day = unformatted_date.substr(6, 2);
        std::string hour = unformatted_date.substr(8, 2);
        std::string min = unformatted_date.substr(10, 2);

        return day + "/" + month + "/" + year + " " + hour + ":" + min;
    }

    //////////////////////////////////////////////////////////////////////////////
    // Top left window: MENU
    //////////////////////////////////////////////////////////////////////////////
    void RenderMenuWindow(WinState& win_state, std::wstring mydoc_path, int framebufferWidth, int framebufferHeight)
    {
        if (!win_state.menu_initialized)
        {
            ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth / 3, (float)framebufferHeight / 3));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            win_state.menu_initialized = true;
        }
        else
        {
            ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth - win_state.data_win_size.x, (float)framebufferHeight - win_state.live_win_size.y));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
        }
        if (!ImGui::Begin("menu", NULL, win_state.window_flags))
        {
            ImGui::End();
            return;
        }
        ImGui::Text("Menu will come here");

        for (int i = 0; i < 5; i++)
        {
            bool k = false;
            if (i == active_window)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.04f, 0.10f, 0.25f, 1.0f });
                k = true;
            }
            
            if (ImGui::Button(active_window_strings[i].c_str(), { (float)framebufferWidth - win_state.data_win_size.x - 15, 30 }))
                active_window = (ActiveWindow)i;

            if (k)
                ImGui::PopStyleColor(1);
        }

        win_state.menu_win_size = ImGui::GetWindowSize();
        ImGui::End();
    }

    //////////////////////////////////////////////////////////////////////////////
    // Bottom left window: RECORDER
    //////////////////////////////////////////////////////////////////////////////
    void RenderRecorderWindow(WinState& win_state, ui_data_pair& pair, int framebufferWidth, int framebufferHeight)
    {
        if (!win_state.live_initialized)
        {
            ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth / 3, ((float)framebufferHeight / 3) * 2));
            ImGui::SetNextWindowPos(ImVec2(0, ((float)framebufferHeight / 3) * 2));
            win_state.live_initialized = true;
        }
        else
        {
            ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth - win_state.data_win_size.x, (float)framebufferHeight - win_state.menu_win_size.y));
            ImGui::SetNextWindowPos(ImVec2(0, win_state.menu_win_size.y));
        }

        if (!ImGui::Begin("live", NULL, win_state.window_flags))
        {
            ImGui::End();
            return;
        }

        ImGui::Text("live data will be displayed here");

        if (pair.session_active || !win_state.live_laps.empty())
        {
            ImGui::Text("Session being recorded");
            ImGui::Separator();

            if (win_state.live_laps.empty()) // not sure if this is needed, but maybe since back() on empty vector is undefined
            {
                win_state.live_laps.push_back(pair.lapnumber.load());
                win_state.consecutive_laps++;
            }

            if (win_state.live_laps.back() < pair.lapnumber.load())
            {
                win_state.live_laps.push_back(pair.lapnumber.load());
                win_state.consecutive_laps++;
            }

            if (win_state.live_laps.back() > pair.lapnumber.load())
                win_state.live_laps.clear();

            for (auto i : win_state.live_laps)
            {
                if (i != 0)
                    ImGui::Text("Completed lap %d", i);
            }

            ImGui::Separator();
            ImGui::Text("Consecutive laps: %d", win_state.consecutive_laps);

            if (pair.in_pit)
            {
                ImGui::Text("In pitlane");
                win_state.was_in_pit = true;
            }

            if (!pair.in_pit && win_state.was_in_pit)// reset concurrent laps only when exiting the pit lane
            {
                win_state.consecutive_laps = 0;
                win_state.was_in_pit = false;
            }

            if (!pair.session_active && win_state.live_laps.size() > 1)
                ImGui::Text("Session saved");
            else if (!pair.session_active)
                ImGui::Text("Session didn't saved, not enough laps");

            ImGui::SetScrollHereY();
        }

        win_state.live_win_size = ImGui::GetWindowSize();
        ImGui::End();
    }

    //////////////////////////////////////////////////////////////////////////////
    // Right window: MAIN
    //////////////////////////////////////////////////////////////////////////////
    void SetMainWindowSize(WinState& win_state, int framebufferWidth, int framebufferHeight)
    {
        if (!win_state.data_initialized)
        {
            ImGui::SetNextWindowSize(ImVec2(((float)framebufferWidth / 3) * 2, (float)framebufferHeight));
            ImGui::SetNextWindowPos(ImVec2((float)framebufferWidth / 3, 0));
            win_state.data_initialized = true;
        }
        else
        {
            ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth - win_state.menu_win_size.x, (float)framebufferHeight));
            ImGui::SetNextWindowPos(ImVec2(win_state.menu_win_size.x, 0));
        }
        if (!ImGui::Begin("data", NULL, win_state.window_flags))
        {
            ImGui::End();
            return;
        }
    }

    void RenderProgressionWindow(WinState& win_state, int framebufferWidth, int framebufferHeight)
    {
        SetMainWindowSize(win_state, framebufferWidth, framebufferHeight);
        ImGui::Text("Progression window");

        win_state.data_win_size = ImGui::GetWindowSize();
        ImGui::End();
    }

    void RenderSessionWindow(WinState& win_state, std::wstring mydoc_path, int framebufferWidth, int framebufferHeight)
    {
        SetMainWindowSize(win_state, framebufferWidth, framebufferHeight);
        ImGui::Text("Session window");

        // should read the files only once and create buttons again and again
        // data of files stored in vector of j...
        // if we would do that then we wouldnt need to read the files twice...
        std::wstring data_folder = L"\\ACC app data";
        std::wstring path = mydoc_path + data_folder;
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            // read json into json j
            json j = ReadFromFile(entry.path());
            // get session from the json j
            int session_number = j["session"];
            std::string session_s = GetSessionType(session_number);

            // get date as string from filename
            std::string base_filename = entry.path().string().substr(entry.path().string().find_last_of("/\\") + 1);
            std::string::size_type const p(base_filename.find_last_of('.'));
            std::string file_without_extension = base_filename.substr(0, p);
            std::string clean_date = FormatDate(file_without_extension);
            // create button with session
            // size of the button should be less hardcoded
            if (ImGui::CollapsingHeader((session_s + " " + clean_date).c_str()))
            {
                ImGui::BeginTable(
                    "lap data",
                    5,
                    ImGuiTableFlags_RowBg,
                    { (float)framebufferWidth - win_state.menu_win_size.x, (float)framebufferHeight },
                    (float)framebufferWidth - win_state.menu_win_size.x / 5.0f
                );

                ImGui::TableSetupColumn("Lap");
                ImGui::TableSetupColumn("Laptime");
                ImGui::TableSetupColumn("Sector 1");
                ImGui::TableSetupColumn("Sector 2");
                ImGui::TableSetupColumn("Sector 3");
                ImGui::TableHeadersRow();

                json j = ReadFromFile(entry.path());
                json laps = j["laps"];

                for (auto& x : laps.items())
                {
                    //std::cout << "key: " << x.key() << ", value: " << x.value() << '\n';
                    json single_lap = x.value();
                    //std::cout << single_lap.at("laptime: ") << std::endl;
                    ImGui::TableNextColumn();
                    int current_lap = single_lap.at("current lap");
                    ImGui::Text("%d", current_lap);
                    ImGui::TableNextColumn();
                    int laptime = single_lap.at("laptime");
                    std::string laptime_s = FormatTime(laptime);
                    ImGui::Text(laptime_s.c_str());
                    ImGui::TableNextColumn();
                    int sector1 = single_lap.at("sector1");
                    std::string sector1_s = FormatTime(sector1);
                    ImGui::Text(sector1_s.c_str());
                    ImGui::TableNextColumn();
                    int sector2 = single_lap.at("sector2");
                    std::string sector2_s = FormatTime(sector2);
                    ImGui::Text(sector2_s.c_str());
                    ImGui::TableNextColumn();
                    int sector3 = single_lap.at("sector3");
                    std::string sector3_s = FormatTime(sector3);
                    ImGui::Text(sector3_s.c_str());
                }

                ImGui::EndTable();
            }
        }

        win_state.data_win_size = ImGui::GetWindowSize();
        ImGui::End();
    }

    void RenderComboWindow(WinState& win_state, int framebufferWidth, int framebufferHeight)
    {
        SetMainWindowSize(win_state, framebufferWidth, framebufferHeight);
        ImGui::Text("Combo window");

        win_state.data_win_size = ImGui::GetWindowSize();
        ImGui::End();
    }

    void RenderStatsWindow(WinState& win_state, int framebufferWidth, int framebufferHeight)
    {
        SetMainWindowSize(win_state, framebufferWidth, framebufferHeight);
        ImGui::Text("Stats window");

        win_state.data_win_size = ImGui::GetWindowSize();
        ImGui::End();
    }

    void RenderFuelWindow(WinState& win_state, int framebufferWidth, int framebufferHeight)
    {
        SetMainWindowSize(win_state, framebufferWidth, framebufferHeight);
        ImGui::Text("Fuel window");

        win_state.data_win_size = ImGui::GetWindowSize();
        ImGui::End();
    }

    void RenderMainWindow(WinState& win_state, std::wstring mydoc_path, int framebufferWidth, int framebufferHeight)
    {
        // Render the right window based on the set active window
        // std::cout << active_window_strings[active_window] << std::endl;
        switch (active_window)
        {
        case Progression_window:
            RenderProgressionWindow(win_state, framebufferWidth, framebufferHeight);
            break;
        case Sessions_window:
            RenderSessionWindow(win_state, mydoc_path, framebufferWidth, framebufferHeight);
            break;
        case Best_combo_window:
            RenderComboWindow(win_state, framebufferWidth, framebufferHeight);
            break;
        case Stats_car_track_window:
            RenderStatsWindow(win_state, framebufferWidth, framebufferHeight);
            break;
        case Fuel_window:
            RenderFuelWindow(win_state, framebufferWidth, framebufferHeight);
            break;
        default:
            break;
        }
    }
} // anonymous namespace


void UIRenderer(std::wstring mydoc_path, ui_data_pair& pair)
{
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
    WinState win_state;
    InitializeState(win_state);
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

        ImGui::GetStyle().WindowRounding = 0.0f;
        ImGui::GetStyle().FrameRounding = 5.0f;

        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
        ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(0.36f, 0.36f, 0.36f, 1.0f);
        ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.0f);
        

        // 2.5 our own created windows
        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        // top left window
        RenderMenuWindow(win_state, mydoc_path, framebufferWidth, framebufferHeight);

        // bottom left window
        RenderRecorderWindow(win_state, pair, framebufferWidth, framebufferHeight);

        // right window
        RenderMainWindow(win_state, mydoc_path, framebufferWidth, framebufferHeight);

        if (win_state.show_demo_window) {
            ImGui::ShowDemoWindow(&win_state.show_demo_window);
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(win_state.clear_color.x * win_state.clear_color.w, win_state.clear_color.y * win_state.clear_color.w, win_state.clear_color.z * win_state.clear_color.w, win_state.clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        

        glfwSwapBuffers(window);
        /////////////////////
        // end IMGUI stuff //
        /////////////////////
    }

    // signal collector that we are shutting down
    pair.UI_running = false;

    // cleanup IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    // end cleanup IMGUI
}
