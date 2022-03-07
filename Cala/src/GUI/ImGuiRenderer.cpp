#include "ImGuiRenderer.h"
#include <imgui.h>
#include <iostream>
#include "Path.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

bool ImGuiRenderer::imGuiInitialized = false;

ImGuiRenderer::ImGuiRenderer(const Window& window)
{
    if (!imGuiInitialized)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        ImGui::StyleColorsDark();

        const char* glsl_version = "#version 460";

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window.getWindowPointer(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        io.FontDefault = io.Fonts->AddFontFromFileTTF((Cala::getProjectPath() / "Calion/Fonts/Nunito/Nunito-regular.ttf").string().c_str(), 18.f);

        imGuiInitialized = true;
    }
}

ImGuiRenderer::~ImGuiRenderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiRenderer::beginFrame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiRenderer::endFrame() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}