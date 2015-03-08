#pragma once

#include <numeric>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <iterator>

#include "card.h"
#include "player.h"
#include "rules.h"
#include "deck.h"

namespace poker {
    template<typename rules>
    class game_t {
    public:
        game_t(std::vector<std::unique_ptr<player_t>> players, uint32_t initial_cash)
                : players(std::move(players))
        {
            for (size_t i = 0; i < this->players.size(); ++i) {
                hands.push_back(hand_t(initial_cash));
            }
        }

        game_t(game_t const&) = delete;
        game_t& operator=(game_t const&) = delete;

        bool is_game_over() const {
            return players.size() == 1;
        }

        void play_round() {
            if (is_game_over())
                throw std::runtime_error("Game is already over");
            prepare_round();

            place_blinds();
            trade_round(next_player(dealer, 3));
            exchange_round();
            trade_round(next_player(dealer));
            showdown();

            end_round();
        }
    private:
        std::vector<std::unique_ptr<player_t>> players;
        std::vector<hand_t> hands;
        deck_t deck;
        std::vector<uint32_t> current_bets;
        size_t dealer = static_cast<size_t>(-1);

        size_t next_player(size_t current_player, size_t dist = 1) const {
            return (current_player + dist) % players.size();
        }

        void prepare_round() {
            deck.reshuffle();
            for (size_t i = 0; i < 5; ++i) {
                for (hand_t & hand : hands) {
                    hand.add_card(deck.take());
                }
            }
            current_bets.assign(players.size(), 0);
            dealer = next_player(dealer);
        }

        void place_blinds() {
            size_t small = next_player(dealer);
            size_t big = next_player(small);
            current_bets[small] = rules::SMALL_BLIND;
            current_bets[big] = rules::BIG_BLIND;
            hands[small].take_cash(rules::SMALL_BLIND);
            hands[big].take_cash(rules::BIG_BLIND);
            std::cout << "[" << players[small]->get_name() << "] - small blind, "
                    << "[" << players[big]->get_name() << "] - big blind" << std::endl;
        }

        uint32_t get_max_bet() const {
            return std::accumulate(
                    current_bets.begin(),
                    current_bets.end(),
                    uint32_t(0),
                    [](uint32_t max, uint32_t bet) {
                        return std::max(max, bet);
                    }
            );
        }

        bool are_bets_done() const {
            uint32_t max_bet = get_max_bet();
            for (size_t i = 0; i < players.size(); ++i)
                if (current_bets[i] != max_bet && hands[i].has_cards() && hands[i].get_cash() != 0)
                    return false;
            return true;
        }

        void trade_round(size_t first_player) {
            for (size_t i = 0; i < players.size() || !are_bets_done(); ++i) {
                size_t idx = next_player(first_player, i);
                if (hands[idx].get_cash() == 0 || !hands[idx].has_cards())
                    continue;
                uint32_t max_bet = get_max_bet();
                uint32_t cur_bet = current_bets[idx];
                uint32_t bet = players[idx]->bet(cur_bet, max_bet, hands[idx]);
                if (cur_bet + bet < max_bet && bet < hands[idx].get_cash()) {
                    fold(hands[idx]);
                    std::cout << "[" + players[idx]->get_name() + "] "
                            << "Folded his hand" << std::endl;
                } else {
                    current_bets[idx] += bet;
                    hands[idx].take_cash(bet);
                    std::cout << "[" + players[idx]->get_name() + "] ";
                    if (bet && cur_bet + bet == max_bet)
                        std::cout << "Called";
                    else if (bet)
                        std::cout << "Raised to " << cur_bet + bet;
                    else
                        std::cout << "Checked";
                    std::cout << std::endl;
                }
            }
        }

        void exchange_round() {
            for (size_t i = 0; i < players.size(); ++i) {
                size_t idx = next_player(dealer, i);
                if (!hands[idx].has_cards())
                    continue;
                for (card_t & card : players[idx]->exchange(hands[idx])) {
                    deck.put(std::move(card));
                    hands[idx].add_card(deck.take());
                }
            }
        }

        void showdown() {
            size_t winner = 0;
            uint32_t max_rank = 0;
            for (size_t i = 0; i < players.size(); ++i) {
                if (!hands[i].has_cards())
                    continue;
                uint32_t rank = rules::get_combination_rank(hands[i]);
                if (rank > max_rank) {
                    max_rank = rank;
                    winner = i;
                }
            }
            std::cout << "[" << players[winner]->get_name() << "] won with "
                        << get_combination_name(max_rank) << " ";
            auto const& cards = hands[winner].get_cards();
            std::copy(cards.begin(), cards.end(), std::ostream_iterator<card_t>(std::cout, ", "));
            std::cout << std::endl;
        }

        void fold(hand_t & hand) {
            while (hand.has_cards())
                deck.put(hand.take_card(0));
        }

        void end_round() {
            for (hand_t & hand : hands) {
                fold(hand);
            }
            for (size_t i = 0; i < players.size(); ++i) {
                size_t index = players.size() - i - 1;
                if (hands[index].get_cash() < rules::BIG_BLIND) {
                    players.erase(players.begin() + index);
                    hands.erase(hands.begin() + index);
                }
            }
        }
    };
}