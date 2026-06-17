#include "precomp.h"

#ifdef INDIGO_EDITOR



void Editor::init()
{
    register_self();

    float main_scale = engine.get_window()->get_monitor_ui_scale(); 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    
    io.IniFilename = "saved/EditorLayout.ini"; // TODO: add saved folder path to file handler and use that here

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_Impl_Init();
}

void Editor::tick(float _delta)
{
    auto window = engine.get_window();
    valid_ptr(window)
        
    // Begin Frame shit...
    ImGui_Impl_NewFrame();

    // Docking Space
    
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;


    static bool truethat = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &truethat, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
            {
                8.0f,
                7.0f,
            });
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
            {
                8.0f,
                7.0f,
            });
        ImGui::BeginMainMenuBar();

        if (ImGui::BeginMenu("File"))
        {
        }

        if (ImGui::BeginMenu("Modules"))
        {
            for (auto window : registered_windows)
            {
                auto title = window->get_title();
                if (!title.empty())
                {
                    ImGui::MenuItem(title.c_str(), nullptr, &opened_windows[title]);
                }
                else
                {
                    title = "[" + title + "]";
                    ImGui::MenuItem(title.c_str(), nullptr, nullptr);
                }
            }
            ImGui::EndMenu();
        }

        ImGui::SameLine();
        const auto frameTime = _delta * 1000.0f;
        ImGui::Text("        Frame Time: %2.2fms", frameTime);
        ImGui::EndMainMenuBar();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    }



    // Regular Editor Windows

    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, (float)window->get_window_size().x, (float)window->get_window_size().y);

    // Special Cases
    // Modules
    {
        ImGui::Begin("Modules");
        ImGui::BeginTable("Module", 2, ImGuiTableColumnFlags_NoResize | ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Use Count");
        ImGui::TableHeadersRow();
        ImGui::TableNextColumn();
        for (auto window : registered_windows)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text(window->get_title().c_str());
            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(window.use_count()).c_str());
        }
        ImGui::EndTable();
        ImGui::End();
    }
        ImGui::End();

    // Render Interfaces
    for (auto window : registered_windows)
    {
        ImGui::Begin(window->get_title().c_str());

        // should loop through its registered_interfaces
        window->interface_window();

        ImGui::End();
    }

    ImGui::Render();
    ImGui_Impl_RenderDrawData(ImGui::GetDrawData());

    // clean up
    clean_up();
}

void Editor::shutdown()
{
}

void Editor::register_window(std::shared_ptr<EditorWindow> _window)
{
    registered_windows.push_back(_window);
    opened_windows.insert({ _window->get_title(), false });
}
void Editor::interface_window()
{
    ImGui::Separator();

}
void Editor::clean_up()
{
    // clean up expired pointers
    auto it = registered_windows.begin();
    for (auto interface : registered_windows)
    {
        if (interface.use_count() <= 1)
        {
            registered_windows.erase(it);
        }
        it++;
    }
}


#else
void Editor::init(){}
void Editor::tick(float _delta){}
void Editor::shutdown(){}
#endif // INDIGO_EDITOR