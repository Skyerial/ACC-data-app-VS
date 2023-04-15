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
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
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

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
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
				session.setSessionData(pfs);
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
			Sleep(100); // in miliseconds, tickrate is 1Hz
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
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
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
