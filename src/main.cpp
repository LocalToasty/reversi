#include <chrono>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "human.hpp"
#include "minimax.hpp"
#include "reversi.hpp"

namespace po = boost::program_options;
using boost::optional;

optional<std::function<Move(Board const&, Player, optional<duration>)>>
parse_actor(std::string actor_name) {
  using ret_type =
      std::function<Move(Board const&, Player, optional<duration>)>;

  if (actor_name == "human") {
    return (ret_type)(human_actor);
  } else if (actor_name == "minimax") {
    return (ret_type)(minimax_actor);
  } else {
    // unrecognized actor name
    return boost::none;
  }
}

int main(int argc, char** argv) {
  // set up arguments
  std::string dark;
  std::string light;
  int dark_time;
  int light_time;

  po::options_description desc("Options");
  // clang-format off
  desc.add_options()
    ("help,h", "this message")
    ("dark,d", po::value<std::string>(&dark)->default_value("human"),
     "dark actor")
    ("light,l", po::value<std::string>(&light)->default_value("minimax"),
     "light actor")
    ("dark-time", po::value<int>(&dark_time)->default_value(0),
     "time budget of the dark actor in minutes")
    ("light-time", po::value<int>(&light_time)->default_value(0),
     "time budget of the light actor in minutes")
    ("verbose,v", "");
  // clang-format on

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc;
    return 0;
  }

  auto dark_actor = parse_actor(dark);
  auto light_actor = parse_actor(light);

  if (!(light_actor && dark_actor)) {
    std::cerr << desc;
    return -1;
  }

  // if the budget is set to 0, there is no time limit
  optional<duration> dark_budget =
      (dark_time == 0)
          ? boost::none
          : (optional<duration>)(std::chrono::duration_cast<duration>(
                std::chrono::minutes(dark_time)));
  optional<duration> light_budget =
      (light_time == 0)
          ? boost::none
          : (optional<duration>)(std::chrono::duration_cast<duration>(
                std::chrono::minutes(light_time)));

  bool verbose = (vm.count("verbose") != 0);

  // play a game of reversi
  Player winner = play_reversi(*dark_actor, dark_budget, *light_actor,
                               light_budget, verbose);

  // announce winner
  switch (winner) {
    case Player::dark:
      std::cout << "Dark wins the game" << std::endl;
      break;

    case Player::light:
      std::cout << "Light wins the game" << std::endl;
      break;

    default:
      std::cout << "It's a draw" << std::endl;
      break;
  }
}
