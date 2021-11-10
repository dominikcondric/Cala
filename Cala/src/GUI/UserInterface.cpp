#include "UserInterface.h"
#include <imgui.h>
#include <iostream>
#include "Path.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

bool UserInterface::imGuiInitialized = false;

UserInterface::UserInterface(const Window& window)
{
    if (!imGuiInitialized)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

       //  Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        const char* glsl_version = "#version 460";

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window.getWindowPointer(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // TODO: Work on resizing of GUI elements
        io.FontDefault = io.Fonts->AddFontFromFileTTF((Cala::getProjectPath() / "Calion/Fonts/Nunito/Nunito-regular.ttf").string().c_str(), 18.f);

        imGuiInitialized = true;
    }
}

UserInterface::~UserInterface()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UserInterface::beginFrame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.293f, 0.7f, 0.293f, 1.f));
}

void UserInterface::endFrame() const
{
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::addLabel(const char* string) const
{
    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored(ImVec4(.42f, .45f, 190.f / 255.f, .75f), string);
    ImGui::SetWindowFontScale(1.f);
    ImGui::Separator();
}