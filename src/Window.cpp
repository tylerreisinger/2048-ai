#include "Window.h"

#include "imgui/imgui.h"

#include <chrono>
#include <cstring>

#include "HumanGameController.h"

#include "AI/RandomController.h"

using namespace result;

std::string get_shader_log(GLuint shader) {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    std::string log;
    log.resize(length, '\0');
    glGetShaderInfoLog(shader, length, &length, log.data());

    return log;
}

static const char* vertex_shader_src =
        R"(
#version 410

uniform mat4 proj;
in vec2 position;
in vec2 uv;
in vec4 color;

out VertexData {
    vec2 uv;
    vec4 color;
} out_data;

void main() {
    out_data.uv = uv;
    out_data.color = color;
    gl_Position = proj * vec4(position, 0.0, 1.0);
}
)";

static const char* frag_shader_src =
        R"(
#version 410

uniform sampler2D tex;

in VertexData {
    vec2 uv;
    vec4 color;
} vertex_data;

out vec4 color;

 float apply_srgb_component(float component) {
    const float srgb_coeff = 12.92;
    const float exponent = 2.4;
    const float cutoff = 0.04045;
    const float a = 0.055;

    if(component < cutoff) {
        return component/srgb_coeff;
    } else {
        return pow((component + a)/(1.0 + a), exponent);
    }
}

vec4 apply_srgb(vec4 color) {
    return vec4(
        apply_srgb_component(color.r), 
        apply_srgb_component(color.g), 
        apply_srgb_component(color.b), 
        color.a
    );
}

void main() {
    color = apply_srgb(vertex_data.color * texture(tex, vertex_data.uv));
}
)";

void debug_callback(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam) {
    std::cout << "Source: " << source << " -- Type " << type << " -- ID " << id
              << " Severity: " << severity << " Msg: " << message << std::endl;
}

template <typename T>
ImVec2 to_imvec2(sf::Vector2<T> in) {
    return ImVec2(static_cast<float>(in.x), static_cast<float>(in.y));
}

Window::Window(uint64_t seed) : m_board(5, 5, seed) {}

Window::~Window() {
    glDeleteTextures(1, &m_font_tex);
    glDeleteProgram(m_imgui_shader);
}


void Window::initialize(std::unique_ptr<IGameController> controller) {
    auto ctx_settings = sf::ContextSettings();
    ctx_settings.majorVersion = 4;
    ctx_settings.minorVersion = 5;
    ctx_settings.depthBits = 24;
    ctx_settings.sRgbCapable = true;
    ctx_settings.attributeFlags =
            sf::ContextSettings::Core | sf::ContextSettings::Debug;
    m_window.create(sf::VideoMode(1600, 1200),
            "Starlight Glimmer",
            sf::Style::Default,
            ctx_settings);
    m_window.setFramerateLimit(60);
    m_window.setKeyRepeatEnabled(false);

    m_controller = std::move(controller);
    init_gl().expect("Unable to initialize OpenGL!");
    init_imgui();

    m_board.add_new_block();
}

void Window::run(std::unique_ptr<IGameController> controller) {
    initialize(std::move(controller));
    m_clock.tick(std::chrono::duration<double>(0.0));

    while(m_window.isOpen()) {
        auto game_time = m_clock.tick(
                std::chrono::duration<double, std::milli>(1.0 / 60.0));
        event_loop();
        update(game_time);
        draw(game_time);

        m_window.display();
    }
}

void Window::draw(const GameTime& time) {
    auto start = std::chrono::high_resolution_clock::now();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_imgui(ImGui::GetDrawData());
    auto end = std::chrono::high_resolution_clock::now();
    m_draw_time = std::chrono::duration_cast<
            std::chrono::duration<double, std::milli>>(end - start);
}
void Window::update(const GameTime& time) {
    m_delay_elapsed += time.get_elapsed_wall_time();
    if(m_delay_elapsed > m_delay) {
        auto start = std::chrono::high_resolution_clock::now();
        m_controller->do_turn(m_board, time);
        m_delay_elapsed = std::chrono::duration<double>(0.0);
        auto end = std::chrono::high_resolution_clock::now();
        m_ai_time = std::chrono::duration_cast<
                std::chrono::duration<double, std::milli>>(end - start);
    }
    update_imgui(time);
}

Result<unit_t, InitErrorKind> Window::init_gl() {
    if(gl3wInit()) {
        return Err(InitErrorKind::Gl3wError);
    }

    auto vert_shader = glCreateShader(GL_VERTEX_SHADER);
    auto frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    int vert_length = std::strlen(vertex_shader_src);
    glShaderSource(vert_shader, 1, &vertex_shader_src, &vert_length);
    glCompileShader(vert_shader);
    int frag_length = std::strlen(frag_shader_src);
    glShaderSource(frag_shader, 1, &frag_shader_src, &frag_length);
    glCompileShader(frag_shader);

    int success = 0;
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        std::cout << vertex_shader_src << std::endl;
        std::cerr << get_shader_log(vert_shader) << std::endl;
        return Err(InitErrorKind::ShaderCompileError);
    }
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        std::cout << frag_shader_src << std::endl;
        std::cerr << get_shader_log(vert_shader) << std::endl;
        return Err(InitErrorKind::ShaderCompileError);
    }

    m_imgui_shader = glCreateProgram();
    glAttachShader(m_imgui_shader, vert_shader);
    glAttachShader(m_imgui_shader, frag_shader);
    glLinkProgram(m_imgui_shader);

    glGetProgramiv(m_imgui_shader, GL_LINK_STATUS, &success);
    if(success == GL_FALSE) {
        return Err(InitErrorKind::ShaderLinkError);
    }

    glDebugMessageCallback(debug_callback, nullptr);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    return Ok();
}

void Window::event_loop() {
    sf::Event e;
    auto& io = ImGui::GetIO();
    while(m_window.pollEvent(e)) {
        switch(e.type) {
        case sf::Event::Closed: {
            m_window.close();
            break;
        }
        case sf::Event::MouseMoved: {
            io.MousePos = ImVec2(e.mouseMove.x, e.mouseMove.y);
            break;
        }
        case sf::Event::MouseWheelScrolled: {
            io.MouseWheel = e.mouseWheelScroll.delta;
            break;
        }
        default:
            break;
        }
        m_controller->handle_event(m_board, e);
    }
}

void Window::init_imgui() {
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    glGenTextures(1, &m_font_tex);
    glBindTexture(GL_TEXTURE_2D, m_font_tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D,
            0,
            0,
            0,
            width,
            height,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixels);
    io.Fonts->SetTexID(reinterpret_cast<void*>(m_font_tex));
}

void Window::update_imgui(const GameTime& time) {
    auto& io = ImGui::GetIO();
    auto win_size = to_imvec2(m_window.getSize());
    io.DisplaySize = win_size;
    io.DisplayFramebufferScale = ImVec2(1.0, 1.0);

    io.MouseDown[0] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    io.MouseDown[1] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
    io.MouseDown[2] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);
    io.MouseDown[3] = sf::Mouse::isButtonPressed(sf::Mouse::Button::XButton1);
    io.MouseDown[4] = sf::Mouse::isButtonPressed(sf::Mouse::Button::XButton2);

    io.DeltaTime = time.get_elapsed_wall_time().count();

    ImGui::NewFrame();

    ImGui::ShowMetricsWindow();

    ImGui::Begin("Window");
    auto draw_list = ImGui::GetWindowDrawList();
    m_renderer.draw(m_board, draw_list);
    ImGui::End();

    ImGui::Begin("Board Statistics");
    ImGui::LabelText("##Header", "Current State:");
    ImGui::BulletText("Score: %d", static_cast<int>(m_board.compute_score()));
    ImGui::BulletText("Free Cells: %d", m_board.free_spaces());
    ImGui::BulletText("Turn #: %d", m_board.turn());
    ImGui::BulletText("Last Move Time: %fms", m_ai_time.count());
    ImGui::BulletText("Last Draw Time: %fms", m_draw_time.count());
    ImGui::End();

    ImGui::Render();
}

void Window::draw_imgui(ImDrawData* draw_data) {
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);

    glViewport(0, 0, fb_width, fb_height);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint last_scissor_box[4];
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);

    const float ortho_projection[4][4] = {
            {2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f},
            {0.0f, 0.0f, -1.0f, 0.0f},
            {-1.0f, 1.0f, 0.0f, 1.0f},
    };
    glUseProgram(m_imgui_shader);
    glUniformMatrix4fv(0, 1, GL_FALSE, &ortho_projection[0][0]);
    glUniform1i(1, 0);
    glBindSampler(0, 0);

    GLuint vbo;
    GLuint ibo;

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ImDrawVert),
            (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(1,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ImDrawVert),
            (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(2,
            4,
            GL_UNSIGNED_BYTE,
            GL_TRUE,
            sizeof(ImDrawVert),
            (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

    for(int i = 0; i < draw_data->CmdListsCount; ++i) {
        auto& cmd_list = draw_data->CmdLists[i];

        glBufferData(GL_ARRAY_BUFFER,
                (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
                (const GLvoid*)cmd_list->VtxBuffer.Data,
                GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
                (const GLvoid*)cmd_list->IdxBuffer.Data,
                GL_STREAM_DRAW);

        std::uintptr_t buffer_offset = 0;

        for(int j = 0; j < cmd_list->CmdBuffer.size(); ++j) {
            const auto& draw_cmd = cmd_list->CmdBuffer[j];

            if(draw_cmd.UserCallback) {
                draw_cmd.UserCallback(cmd_list, &draw_cmd);
            } else {
                glBindTexture(GL_TEXTURE_2D,
                        (GLuint)(std::intptr_t)draw_cmd.TextureId);
                glScissor((int)draw_cmd.ClipRect.x,
                        (int)(fb_height - draw_cmd.ClipRect.w),
                        (int)(draw_cmd.ClipRect.z - draw_cmd.ClipRect.x),
                        (int)(draw_cmd.ClipRect.w - draw_cmd.ClipRect.y));
                glDrawElements(GL_TRIANGLES,
                        (GLsizei)draw_cmd.ElemCount,
                        sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT :
                                                 GL_UNSIGNED_INT,
                        (void*)buffer_offset);
            }
            buffer_offset += draw_cmd.ElemCount * sizeof(ImDrawIdx);
        }
    }
    glScissor(last_scissor_box[0],
            last_scissor_box[1],
            last_scissor_box[2],
            last_scissor_box[3]);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}
