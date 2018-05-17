#include "GameTime.h"


GameTime::GameTime(std::chrono::duration<double> wall_time,
        std::chrono::duration<double> game_time,
        std::chrono::duration<double> wall_time_elapsed,
        std::chrono::duration<double> game_time_elapsed,
        Clock::time_point frame_start_time,
        unsigned int frame_number) {
    m_wall_time = wall_time;
    m_game_time = game_time;
    m_wall_time_elapsed = wall_time_elapsed;
    m_game_time_elapsed = game_time_elapsed;
    m_frame_start_time = frame_start_time;
    m_frame_number = frame_number;
}

std::chrono::duration<double> GameTime::get_wall_time() const {
    return m_wall_time;
}

std::chrono::duration<double> GameTime::get_game_time() const {
    return m_game_time;
}

std::chrono::duration<double> GameTime::get_elapsed_wall_time() const {
    return m_wall_time_elapsed;
}

std::chrono::duration<double> GameTime::get_elapsed_game_time() const {
    return m_game_time_elapsed;
}

GameTime::Clock::time_point GameTime::get_frame_start_time() const {
    return m_frame_start_time;
}

GameTime::Clock::time_point GameTime::get_game_start_time() const {
    return m_game_start_time;
}

unsigned int GameTime::get_frame_number() const { return m_frame_number; }
