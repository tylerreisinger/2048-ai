#include "GameClock.h"

using namespace std::chrono_literals;


GameClock::GameClock()
    : m_frame_number(0),
      m_game_start(std::chrono::high_resolution_clock::now()),
      m_last_frame(0s, 0s, 0s, 0s, m_game_start, m_frame_number) {}

std::chrono::high_resolution_clock::time_point
GameClock::get_game_start_time() const {
    return m_game_start;
}

const GameTime& GameClock::get_last_frame() const { return m_last_frame; }

unsigned int GameClock::get_frame_number() const { return m_frame_number; }

double GameClock::get_game_time_multiplier() const {
    return m_game_time_multiplier;
}

void GameClock::set_game_time_multiplier(double multiplier) {
    m_game_time_multiplier = multiplier;
}

GameTime GameClock::tick(std::chrono::duration<double> time_step) {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed_wall_time = now - m_last_frame.get_frame_start_time();
    auto elapsed_game_time = time_step * m_game_time_multiplier;
    auto wall_time = now - m_game_start;
    auto game_time = m_last_frame.get_game_time() + elapsed_game_time;
    auto frame_number = m_frame_number + 1;
    m_frame_number = frame_number;

    auto next_frame_time = GameTime(wall_time,
            game_time,
            elapsed_wall_time,
            elapsed_game_time,
            now,
            frame_number);
    m_last_frame = next_frame_time;
    return next_frame_time;
}
