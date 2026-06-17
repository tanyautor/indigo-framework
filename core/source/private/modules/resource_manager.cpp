#include "precomp.h"

void ResourceManager::init()
{
    register_self();
}

void ResourceManager::tick(float _delta)
{
}

void ResourceManager::fixed_tick()
{
    //clean_up();
}

void ResourceManager::shutdown()
{
}

void ResourceManager::interface_window()
{
    ImGui::BeginTable("Module", 3, ImGuiTableColumnFlags_NoResize | ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("Type");
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Use Count");
    ImGui::TableHeadersRow();
    ImGui::TableNextColumn();
    for (auto resource : resources)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(magic_enum::enum_name(resource.second->type).data());
        ImGui::TableNextColumn();
        ImGui::Text(resource.second->get_path().c_str());
        ImGui::TableNextColumn();
        ImGui::Text(std::to_string(resource.second.use_count()).c_str());
    }
    ImGui::EndTable();
}

void ResourceManager::clean_up()
{
    // clean up expired pointers
    auto it = resources.begin();
    for (auto interface : resources)
    {
        if (interface.second.use_count() <= 1)
        {
            resources.erase(it);
        }
        it++;
    }
}