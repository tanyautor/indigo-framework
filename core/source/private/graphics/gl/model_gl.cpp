#include "precomp.h"


using namespace glm;

//Model::Model()
//{
//    transform.SetFromMatrix(identity<mat4>());
//}
//
//void Model::load_from_file_obj(std::string _path)
//{
//    float bmin[3], bmax[3]; 
//    if(LoadObjAndConvert(bmin, bmax, &meshes, materials, textures, _path.c_str()))
//    {
//        transform.name = _path;
//    }
//}
//void Model::load_from_file_gltf(std::string _path)
//{
//}
//
//void Model::render()
//{
//    for (auto& mesh : meshes)
//    {
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, textures[materials[mesh.material_id].diffuse_texname]);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, textures[materials[mesh.material_id].normal_texname]);
//        mesh.render();
//    }
//}
//void Model::tick(float _delta)
//{
//#ifdef INDIGO_EDITOR
//    static ImGuizmo::OPERATION operation{ ImGuizmo::OPERATION::TRANSLATE };
//    if (!input.get_mouse_button(MouseButton::Right))
//    {
//        if (input.get_keyboard_key_once(KeyboardKey::W))
//        {
//            operation = ImGuizmo::OPERATION::TRANSLATE;
//        }
//        if (input.get_keyboard_key_once(KeyboardKey::E))
//        {
//            operation = ImGuizmo::OPERATION::ROTATE;
//        }
//        if (input.get_keyboard_key_once(KeyboardKey::R))
//        {
//            operation = ImGuizmo::OPERATION::SCALE;
//        }
//    }
//
//    // TODO: AAAAAAAAAAAAAAAAAAH
//    auto camera = engine.get_editor_camera();
//    glm::mat4 tmp = transform.World(); 
//    glm::mat4 view = glm::lookAt(camera.transform.GetTranslation(), camera.transform.GetTranslation() + glm::eulerAngles(camera.transform.GetRotation()), glm::vec3(0, 1, 0));
//
//    static std::hash<std::string> hasher;
//    uint32 id = static_cast<int32>(hasher(transform.name));
//
//    ImGuizmo::PushID(id);
//    if(ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(engine.get_editor_camera().projection),
//        operation, ImGuizmo::MODE::WORLD, glm::value_ptr(tmp)))
//    {
//        transform.SetFromMatrix(tmp);
//    }
//
//    ImGuizmo::PopID();
//
//#endif
//}