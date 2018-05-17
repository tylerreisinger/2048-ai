#ifndef MINIMAXCONTROLLER_H_
#define MINIMAXCONTROLLER_H_

#include "AiController.h"
#include "Board.h"

#include <limits>
#include <random>

enum class MaybeMove {
    InvalidMove = -1,
    Down = 0,
    Left = 1,
    Right = 2,
    Up = 3,
};

struct MinimaxResult {
    double best_score = 0.0;
    ShiftDirection move;
    int nodes_visited = 0;
};

struct MinimaxStats {
    double max_score = std::numeric_limits<double>::min();
    int nodes_evaluated = 0;
};

class MinimaxController : public AiController {
public:
    MinimaxController(uint64_t seed = 0) : AiController(seed) {}
    ~MinimaxController() = default;

    MinimaxController(const MinimaxController& other) = delete;
    MinimaxController(MinimaxController&& other) noexcept = default;
    MinimaxController& operator=(const MinimaxController& other) = delete;
    MinimaxController& operator=(MinimaxController&& other) noexcept = default;

    virtual void do_turn(Board& board, const GameTime& time) override;
    virtual void seed(std::seed_seq& seed) override { m_rng.seed(seed); }

    virtual void draw_state(const Board& board, const GameTime& time) override;

private:
    std::tuple<MaybeMove, double> minimax(Board& board,
            int depth,
            MinimaxStats& node_count);
    std::tuple<MaybeMove, double> minimax_max(
            Board& board, int depth, MinimaxStats& node_count);
    double minimax_min(Board& board, int depth, MinimaxStats& node_count);
    double score_board(const Board& board);
    double score_move(ShiftDirection dir);

    std::default_random_engine m_rng;
    MinimaxStats m_stats;
};

#endif
