#include <iostream>

#include <GL/gl3w.h>

#include "Window.h"

#include "cxxopts.hpp"

#include "AI/RandomController.h"
#include "HumanGameController.h"
#include "IGameController.h"

cxxopts::ParseResult parse_opts(int argc, char** argv);
std::unique_ptr<IGameController> create_controller(
        const std::string& name, int delay = 10);

int main(int argc, char** argv) {
    cxxopts::Options options(
            "2048_AI", "A 2048 implementation for testing AIs.");
    options.add_options()("c,controller",
            "The AI controller to use",
            cxxopts::value<std::string>()->default_value("HumanController"))(
            "d,delay",
            "The delay time between AI moves",
            cxxopts::value<int>()->default_value("1"))("h,help", "Print help");
    auto args = options.parse(argc, argv);

    if(args.count("help") > 0) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    auto controller_name = args["controller"].as<std::string>();
    std::cout << "Selecting " << controller_name << "..." << std::endl;
    auto controller =
            create_controller(controller_name, args["delay"].as<int>());
    if(!controller) {
        std::cerr << "Unknown controller '" << controller_name << "'."
                  << std::endl;
        return -1;
    }

    Window w;
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
        const std::string& name, int delay) {
    if(name == "HumanController") {
        return std::make_unique<HumanGameController>();
    } else if(name == "RandomController") {
        return std::make_unique<RandomController>(delay);
    } else {
        return nullptr;
    }
}
