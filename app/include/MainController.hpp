//
// Created by nikolina on 21.10.25..
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
namespace app {

class MainController : public engine::core::Controller{
    void initialize() override;

    bool loop() override;

    void update_camera();

    void update() override;

    void draw_boat();

    void draw_plane();

    void begin_draw() override;

    void draw_skybox();

    void draw() override;

    void end_draw() override;


public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

} // app

#endif //MAINCONTROLLER_HPP
