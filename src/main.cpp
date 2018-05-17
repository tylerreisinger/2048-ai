#include <iostream>

#include <GL/gl3w.h>

#include "Window.h"

#include "cxxopts.hpp"

#include "AI/MctsController.h"
#include "AI/RandomController.h"
#include "AI/TestController.h"
#include "HumanGameController.h"
#include "IGameController.h"

cxxopts::ParseResult parse_opts(int argc, char** argv);
std::unique_ptr<IGameController> create_controller(
        const std::string& name, uint64_t seed = 0);

int main(int argc, char** argv) {
    cxxopts::Options options(
            "2048_AI", "A 2048 implementation for testing AIs.");
    options.add_options()("c,controller",
            "The AI controller to use",
            cxxopts::value<std::string>()->default_value("HumanController"))(
            "d,delay",
            "The delay time between AI moves, in milliseconds",
            cxxopts::value<double>()->default_value("0.0"))(
            "h,help", "Print help")("s,seed",
            "The initial seed to use for random number generators",
            cxxopts::value<uint64_t>());

    auto args = options.parse(argc, argv);

    if(args.count("help") > 0) {
        std::cout << options.help() << std::endl;
        std::cout << "Available Controllers:\n"
                  << "\tHumanController\n"
                  << "\tRandomController\n"
                  << "\tMctsController\n"
                  << "\tTestController\n";
        return 0;
    }

    uint64_t seed_val = 0;
    if(args.count("seed") > 0) {
        seed_val = args["seed"].as<uint64_t>();
    } else {
        auto now = std::chrono::high_resolution_clock::now();
        seed_val = now.time_since_epoch().count();
    }

    auto controller_name = args["controller"].as<std::string>();
    std::cout << "Selecting " << controller_name << "..." << std::endl;
    auto controller = create_controller(controller_name, seed_val);
    if(!controller) {
        std::cerr << "Unknown controller '" << controller_name << "'."
                  << std::endl;
        return -1;
    }

    Window w(seed_val + 1);
    w.set_delay(std::chrono::duration<double, std::milli>(
            args["delay"].as<double>()));
    w.run(std::move(controller));

    return 0;
}

cxxopts::ParseResult parse_opts(int argc, char** argv) {
    cxxopts::Options options(
            "2048_AI", "A 2048 implementation for testing AIs.");
    options.add_options()("c,controller",
            "The AI controller to use",
            cxxopts::value<std::string>()->default_value("HumanController"));
    return options.parse(argc, argv);
}

std::unique_ptr<IGameController> create_controller(
        const std::string& name, uint64_t seed) {
    if(name == "HumanController") {
        return std::make_unique<HumanGameController>();
    } else if(name == "RandomController") {
        return std::make_unique<RandomController>(seed);
    } else if(name == "MctsController") {
        return std::make_unique<MctsController>(seed);
    } else if(name == "TestController") {
        return std::make_unique<TestController>(seed);
    } else {
        return nullptr;
    }
}
