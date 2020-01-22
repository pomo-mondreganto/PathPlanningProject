//
// Created by Roman Nikitin on 22.12.2019.
//

#include "dijkstra.h"
#include <list>
#include <chrono>

Dijkstra::Dijkstra() {
    sresult.nodescreated = 0;
}

SearchResult
Dijkstra::startSearch(ILogger *, const Map &map, const EnvironmentOptions &options) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::shared_ptr<Node> start = map.get_start_node();

    ++sresult.nodescreated;

    state.insert(start);
    while (!state.empty()) {
        ++sresult.numberofsteps;

        std::shared_ptr<Node> cur = *state.begin();
        state.erase(state.begin());

        std::list<std::shared_ptr<Node>> adj = generateAdjacent(cur->i, cur->j, map, options);

        for (auto &n: adj) {
            double new_dist = cur->g + getDistance(cur, n, options);

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
        }
        std::cout << std::endl;
    }

    std::pair<int, int> key = map.get_goal_node()->v_key();

    if (!which.count(key)) {
        sresult.pathfound = false;
        sresult.pathlength = 0;
        return sresult;
    }

    std::shared_ptr<Node> goal = which[key];

    sresult.pathfound = true;
    sresult.pathlength = static_cast<float>(which[key]->g);
    while (goal->v_key() != start->v_key()) {
        lppath.push_front(*goal);
        goal = goal->parent;
    }
    lppath.push_front(*goal);

    buildHPPath();

    sresult.lppath = &lppath;
    sresult.hppath = &hppath;

    auto end_time = std::chrono::high_resolution_clock::now();
    sresult.time = (double) std::chrono::duration_cast<std::chrono::seconds>(
            end_time - start_time).count();
    return sresult;
}

Dijkstra::~Dijkstra() = default;
