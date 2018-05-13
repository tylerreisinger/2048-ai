#ifndef WINDOW_H_
#define WINDOW_H_

#include <memory>

#include <GL/gl3w.h>
#include <SFML/Graphics.hpp>

#include "result/result.h"

enum class InitErrorKind {
    Gl3wError,
    ShaderCompileError,
    ShaderLinkError,
};

class ImDrawData;

class Window {
public:
    Window();
    ~Window();

    Window(const Window& other) = delete;
    Window(Window&& other) = default;
    Window& operator=(const Window& other) = delete;
    Window& operator=(Window&& other) = default;

    void run();

private:
    void update();
    void draw();
    void initialize();
    void event_loop();

    void init_imgui();
    void update_imgui();
    void draw_imgui(ImDrawData* draw_data);

    result::Result<result::unit_t, InitErrorKind> init_gl();

    sf::RenderWindow m_window;

    GLuint m_font_tex = 0;
    GLuint m_imgui_shader = 0;
};

#endif
