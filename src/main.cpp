

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#include <xif_client.h>

void render();
void refresh_callback(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
float slider = 0.5f;
bool button_pressed = false;

GLuint image_texture = 0;
bool has_image = false;
int width;
int height;

void timestep_callback(uint64_t time)
{
    printf("Timestep callback called at time: %llu\n", time);
}

void image_callback(const xif_image_t* image)
{
    printf("Image callback called with image: %p\n", (void*)image);
    // Here you would typically process the image data, e.g., display it in the UI.
    width = image->width;
    height = image->height;

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    if (image->channels == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
        has_image = true;
    }
    else if (image->channels == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
        has_image = true;
    }

}

int main() 
{
    xifc_init();

    xifc_set_timestep_callback(timestep_callback);
    xifc_set_image_callback(image_callback);

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 700, "XIF Test", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowRefreshCallback(window, refresh_callback);


    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window)) 
    {
        xifc_update();

        glfwPollEvents();
        render();
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void refresh_callback(GLFWwindow* window) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    render();
    glfwSwapBuffers(window);
}

void SetupDockLayout(ImGuiID dockspace_id)
{
    static bool dock_initialized = false;
    if (!dock_initialized)
    {
        dock_initialized = true;

        ImGui::DockBuilderRemoveNode(dockspace_id);              // clear any existing layout
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        // Split the dockspace
        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, nullptr, &dock_main_id);

        // Dock windows
        ImGui::DockBuilderDockWindow("Tools Panel", dock_left);
        ImGui::DockBuilderDockWindow("Image Viewer", dock_main_id);

        ImGui::DockBuilderFinish(dockspace_id);
    }
}

void render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace Demo", nullptr, window_flags);

    ImGui::PopStyleVar();

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        SetupDockLayout(dockspace_id);
    }

    ImGui::End();

    ImGui::Begin("Tools Panel");
    ImGui::Text("Some controls here");
    ImGui::End();

    ImGui::Begin("Image Viewer");
    ImGui::Text("Bitmap goes here");
    if (has_image)
    {
        ImGui::Image((ImTextureID)(intptr_t)image_texture, ImVec2(width, height));
    }

    ImGui::End();

    ImGui::Render();
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
