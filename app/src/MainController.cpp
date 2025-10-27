//
// Created by nikolina on 21.10.25..
//

#include "MainController.hpp"

#include "GuiController.hpp"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Mesh.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (!gui_controller->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}

void MainController::initialize() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;
}

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw_plane() {
    //Model
    if(!plane_active) return;
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *plane = resources->model("plane");

    //Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, plane_pos);
    model = glm::scale(model, glm::vec3(0.03f));
    shader->set_mat4("model", model);
    plane->draw(shader);

}

void MainController::draw_boat() {
    //Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *boat = resources->model("boat");

    //Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    for (int i = 0; i < 5; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(i * 2.0f - 7.0f, -4.0f - pow(-1,i), -15.0f + pow(-1, i)));
        model = glm::scale(model, glm::vec3(0.002f + (0.0003 * pow(-1, i))));
        shader->set_mat4("model", model);
        boat->draw(shader);
    }

}
void MainController::draw_helicopter() {
    //Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *helicopter = resources->model("helicopter");

    //Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-6.0f, 0.0f, -15.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.2f));
    shader->set_mat4("model", model);
    helicopter->draw(shader);

}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("exosystem");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (gui_controller->is_enabled()) {
        return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    float dt = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_RIGHT).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
}
void MainController::update_plane() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if ((platform->key(engine::platform::KeyId::KEY_P).state() == engine::platform::Key::State::JustPressed) && !plane_active) {
        plane_active = true;
        plane_pos = plane_start;
    }

    if (plane_active) {
        float speed = 5.0f;
        float dt = platform->dt();

        plane_pos.z += speed * dt;

        if (plane_pos.z >= 80.0f) {
            plane_active = false;
        }
    }
}

void MainController::update_spotlight() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (platform->key(engine::platform::KeyId::KEY_S).state() == engine::platform::Key::State::JustPressed || platform->key(engine::platform::KeyId::KEY_RIGHT).state() == engine::platform::Key::State::JustPressed) {
        spotlight_active = true;
        spotlight_timer = 0.0f;
    }

    if (platform->key(engine::platform::KeyId::KEY_Q).state() == engine::platform::Key::State::JustPressed) {
        spotlight_active = false;
    }

    if (spotlight_active) {
        spotlight_timer += dt;
    }

}
void MainController::update() {
    update_plane();
    update_camera();
    update_spotlight();
}

void MainController::draw() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    shader->use();

    shader->set_vec3("dirLightDir",glm::vec3(-2.0f, -7.0f, -2.0f) );
    shader->set_vec3("dirLightAmbient",  glm::vec3(0.3f, 0.3f, 0.33f));
    shader->set_vec3("dirLightDiffuse",  glm::vec3(0.8f, 0.8f, 0.9f));
    shader->set_vec3("dirLightSpecular", glm::vec3(0.5f, 0.5f, 0.6f));

    glm::vec3 heliPos = glm::vec3(-6.0f, 0.0f, -15.0f);
    glm::vec3 boatsCenter = glm::vec3(0.0f, -5.0f, -15.0f);
    glm::vec3 lightDir = glm::normalize(boatsCenter - heliPos);

    shader->set_vec3("spotLight.position", heliPos);
    shader->set_vec3("spotLight.direction", lightDir);
    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(15.0f)));     // unutrašnji ugao
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(25.0f))); // spoljašnji prsten

    shader->set_float("spotLight.constant", 1.0f);
    shader->set_float("spotLight.linear", 0.05f);
    shader->set_float("spotLight.quadratic", 0.01f);

    float flicker = (sin(spotlight_timer * 3.0f) * 0.5f + 0.5f);

    if(spotlight_active) {
        shader->set_vec3("spotLight.ambient", glm::vec3(0.1f * flicker, 0.0f, 0.0f));
        shader->set_vec3("spotLight.diffuse", glm::vec3(1.0f * flicker, 0.0f, 0.0f));
        shader->set_vec3("spotLight.specular", glm::vec3(1.0f * flicker, 0.0f, 0.0f));

    }
    else {
        shader->set_vec3("spotLight.ambient", glm::vec3(0.0f));
        shader->set_vec3("spotLight.diffuse", glm::vec3(0.0f));
        shader->set_vec3("spotLight.specular", glm::vec3(0.0f));
    }


    draw_helicopter();
    draw_boat();
    draw_plane();
    draw_skybox();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// app