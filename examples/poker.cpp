#include <poker/game.h>
#include <poker/human.h>

#include <iostream>

int main() {
    size_t player_count;
    std::cout << "Enter number of players" << std::endl;
    std::cin >> player_count;
    std::cout << "Enter their names" << std::endl;
    std::vector<std::unique_ptr<poker::player_t>> players;
    while (player_count--) {
        std::string name;
        std::cin >> name;
        players.push_back(std::unique_ptr<poker::human_player_t>(new poker::human_player_t(name)));
    }
    poker::game_t<poker::simple_rules> game(std::move(players), 1000);
    while (!game.is_game_over())
        game.play_round();
    return 0;
}