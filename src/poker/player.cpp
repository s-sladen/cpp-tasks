#include <poker/player.h>

namespace poker {
    player_t::player_t(std::string name)
        : name(std::move(name))
    {}

    player_t::~player_t() {
    }

    std::string const& player_t::get_name() const {
        return name;
    }
}