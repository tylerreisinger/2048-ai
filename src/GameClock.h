#include "GameTime.h"
#include <chrono>

class GameClock {
public:
    GameClock();
    ~GameClock() = default;


    GameClock(const GameClock& other) = delete;
    GameClock(GameClock&& other) noexcept = delete;
    GameClock& operator=(const GameClock& other) = delete;
    GameClock& operator=(GameClock&& other) noexcept = delete;

    double get_game_time_multiplier() const;
    void set_game_time_multiplier(double multiplier);
    std::chrono::high_resolution_clock::time_point get_game_start_time() const;
    const GameTime& get_last_frame() const;
    unsigned int get_frame_number() const;
    GameTime tick(std::chrono::duration<double> time_step);

private:
    unsigned int m_frame_number;
    std::chrono::high_resolution_clock::time_point m_game_start;
    GameTime m_last_frame;
    double m_game_time_multiplier = 1.0;
};
