//
// Created by Roman Nikitin on 22.12.2019.
//

#include <chrono>
#include "dijkstra.h"

Dijkstra::Dijkstra() {
    sresult.nodescreated = 0;
}

SearchResult
Dijkstra::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::shared_ptr<Node> start = map.get_start_node();

    ++sresult.nodescreated;

    state.insert(start);
    while (!state.empty()) {
        ++sresult.numberofsteps;

        std::shared_ptr<Node> cur = *state.begin();
        state.erase(state.begin());

        std::cout << "Generating adjacent" << std::endl;
        std::list<std::shared_ptr<Node>> adj = generateAdjacent(cur->i, cur->j, map, options);
        std::cout << "Looking at " << cur->i << ' ' << cur->j << " " << cur->g << ' '
                  << map.getValue(cur->i, cur->j) << std::endl;

        for (auto &n: adj) {
            double new_dist = cur->g + getDistance(cur, n, options);
            std::cout << "Adjacent " << n->i << ' ' << n->j << ' ' << new_dist << ' '
                      << map.getValue(n->i, n->j) << std::endl;

            std::pair<int, int> key = n->v_key();
            bool not_in_state = (which.count(key) == 0);
            if (not_in_state || which[key]->g > new_dist) {
                if (!not_in_state) {
                    state.erase(which[key]);
                }
                n->g = new_dist;
                n->parent = cur;
                which[key] = n;

                state.insert(n);
            }
            std::cout << "Done adjacent" << std::endl;
        }
        std::cout << std::endl;
    }

    std::cout << "Complete" << std::endl;

    std::pair<int, int> key = map.get_goal_node()->v_key();

    if (!which.count(key)) {
        sresult.pathfound = false;
        sresult.pathlength = 0;
        return sresult;
    }

    std::shared_ptr<Node> goal = which[key];

    sresult.pathfound = true;
    sresult.pathlength = static_cast<float>(which[key]->g);
    std::cout << "Start:" << start->v_key().first << " " << start->v_key().second << std::endl;
    while (goal->v_key() != start->v_key()) {
        std::cout << "Restoring at " << goal->i << ' ' << goal->j << " " << goal->g << std::endl;
        lppath.push_front(*goal);
        goal = goal->parent;
    }
    lppath.push_front(*goal);

    sresult.lppath = &lppath;
    sresult.hppath = &lppath;

    auto end_time = std::chrono::high_resolution_clock::now();
    sresult.time = (double) std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time).count();
    return sresult;
}

Dijkstra::~Dijkstra() = default;
