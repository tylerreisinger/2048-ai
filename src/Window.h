#ifndef WINDOW_H_
#define WINDOW_H_

#include <chrono>
#include <memory>

#include <GL/gl3w.h>
#include <SFML/Graphics.hpp>

#include "BoardRender.h"
#include "GameClock.h"
#include "IGameController.h"
#include "result/result.h"

enum class InitErrorKind {
    Gl3wError,
    ShaderCompileError,
    ShaderLinkError,
};

class ImDrawData;

class Window {
public:
    Window(uint64_t seed = 0);
    ~Window();

    Window(const Window& other) = delete;
    Window(Window&& other) = default;
    Window& operator=(const Window& other) = delete;
    Window& operator=(Window&& other) = default;

    void set_delay(std::chrono::duration<double> val) { m_delay = val; }
    void run(std::unique_ptr<IGameController> controller);

private:
    void update(const GameTime& time);
    void draw(const GameTime& time);
    void initialize(std::unique_ptr<IGameController> controller);
    void event_loop();

    void init_imgui();
    void update_imgui(const GameTime& time);
    void draw_imgui(ImDrawData* draw_data);

    result::Result<result::unit_t, InitErrorKind> init_gl();

    sf::RenderWindow m_window;

    GLuint m_font_tex = 0;
    GLuint m_imgui_shader = 0;

    std::chrono::duration<double> m_delay = std::chrono::duration<double>(0.0);
    std::chrono::duration<double> m_delay_elapsed =
            std::chrono::duration<double>(0.0);

    std::chrono::duration<double, std::milli> m_ai_time;
    std::chrono::duration<double, std::milli> m_draw_time;

    BoardRenderer m_renderer;
    Board m_board;
    GameClock m_clock;
    std::unique_ptr<IGameController> m_controller;
};

#endif
