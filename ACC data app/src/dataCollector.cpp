// The first include is needed for the precompiled header file which makes 
// sure that all the cpp files that have includes in this header file as well 
// dont need to have their header files compiled again. This in the end brings a
// speed boost to compiling.
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include "SharedFileOut.h"

// use "" for header files in project folder, <> is used for headers that are 
// outside of project folder
#include "dataInitDismiss.h"
#include "dataToFile.h"
#include "createFile.h"
#include "lapData.h"
#include "sessionData.h"

#include <fstream>
#include "nlohmann/json.hpp"

// imgui includes
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// include needed to get full path of documents folder
// #include <C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.19041.0\\um\\ShlObj_core.h>
#include <ShlObj.h>

using json = nlohmann::json;

//#pragma optimize("",off)

// get full path of known windows document folder
// maybe a bit of an old way to do this, but it works
wchar_t mydoc_path[MAX_PATH];
HRESULT hres = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, mydoc_path);

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// should this be here?...
//template <typename T, unsigned S>
//inline unsigned arraysize(const T(&v)[S])
//{
//	return S;
//}

int _tmain(int argc_, _TCHAR* argv[]) 
{
	initPhysics();
	initGraphics();
	initStatic();

	SPageFileGraphic* pfg = (SPageFileGraphic*)m_graphics.mapFileBuffer;
	SPageFilePhysics* pfp = (SPageFilePhysics*)m_physics.mapFileBuffer;
	SPageFileStatic* pfs = (SPageFileStatic*)m_static.mapFileBuffer;

	// initialize session_check
	AC_SESSION_TYPE lastSessionType = AC_UNKNOWN;
	bool newSession = true;

    /////////////////
    // IMGUI stuff //
    /////////////////
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return 1;
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create fullscreen window with graphics context at 1080p
    GLFWwindow* window = glfwCreateWindow(1920/1.5, 1080/1.5, "ACC data app (title in progress)", NULL, NULL);
    if (window == NULL) 
    {
        fprintf(stderr, "error: window creation failed!\n");
        return 1;
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
        return 1;
    }

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    bool menu_initialized = false;
    bool data_initialized = false;
    ImVec2 prev_win_size = { 0, 0 };
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;

    std::wstring file_to_show = L"";
    /////////////////////
    // end IMGUI stuff //
    /////////////////////

	while(!glfwWindowShouldClose(window)) {
		std::vector<LapData> laps = createLaps();
		SessionData session;
		// 2 means session is running, 3 session running but in esc menu
		// this also means nothing happens when neither is true since it returns 0
		// at the end
		while (pfg->status == 2 || pfg->status == 3)
		{
			if (newSession) {
				newSession = false;
				lastSessionType = pfg->session;
				session.setSessionData(pfs, pfg);
				std::cout << "new session started" << std::endl;
			}
			// break out of session loop when a session changes
			if (lastSessionType != pfg->session) {
				break;
			}
			// if session running create file and record session type(need to check
			// also if new session type starts), time of day at start, car model,
			// track and multiplayer or singleplayer
			// std::cout << pfg->iLastTime << '\n';
			laps = updateLap(pfg, laps);
			Sleep(100); // in miliseconds, tickrate is 10Hz
		}

		if (!newSession) {
			newSession = true;
			newFile(lastSessionType, L".json", mydoc_path);
			json jLaps = laps;
			json jSession = session;
			jSession["Laps"] = jLaps;
			writeToFile(jSession);
			closeFile();
		}

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

        // left window
        {
            if (!menu_initialized)
            {
                ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth / 3, (float)framebufferHeight));
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                menu_initialized = true;
            }
            else
            {
                ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth - prev_win_size.x, (float)framebufferHeight));
                ImGui::SetNextWindowPos(ImVec2(0, 0));
            }
            if (!ImGui::Begin("menu", NULL, window_flags))
            {
                ImGui::End();
                return 1;
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
                if (ImGui::Button(session_s.c_str(), { (float)framebufferWidth - prev_win_size.x - 15, 30}))
                {
                    file_to_show = entry.path();
                }
                // add button to vector, together with j

            }
            prev_win_size = ImGui::GetWindowSize();
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
                ImGui::SetNextWindowSize(ImVec2((float)framebufferWidth - prev_win_size.x, (float)framebufferHeight));
                ImGui::SetNextWindowPos(ImVec2(prev_win_size.x, 0));
            }
            if (!ImGui::Begin("data", NULL, window_flags))
            {
                ImGui::End();
                return 1;
            }
            ImGui::Text("Data will come here");

            // this doesnt work... but we are getting very close
            if (!file_to_show.empty())
            {
                ImGui::BeginTable(
                    "lap data",
                    5,
                    0,
                    { (float)framebufferWidth - prev_win_size.x, (float)framebufferHeight },
                    (float)framebufferWidth - prev_win_size.x / 5.0f
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

            prev_win_size = ImGui::GetWindowSize();
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

	// end of main, needs to be made to run as long as the application is open.
	dismissAll();
	return 0;
}
