//
// Created by Roman Nikitin on 22.12.2019.
//

#include "auxiliary.h"
#include "dijkstra.h"
#include "search.h"
#include <list>
#include <chrono>

Dijkstra::Dijkstra() {
    sresult.nodescreated = 0;
}

SearchResult
Dijkstra::startSearch(ILogger *, const Map &map, const EnvironmentOptions &options) {
    OPEN = BTSet(g_node_compare{});
    auto start_time = std::chrono::high_resolution_clock::now();

    std::shared_ptr<Node> start = map.get_start_node();
    std::shared_ptr<Node> goal = map.get_goal_node();

    sresult.nodescreated += 2;

    OPEN.insert(start);
    while (!OPEN.empty() && CLOSED.count(goal) == 0) {
        ++sresult.numberofsteps;

        std::shared_ptr<Node> cur = *OPEN.begin();
        OPEN.erase(OPEN.begin());
        CLOSED.insert(cur);

        std::list<std::shared_ptr<Node>> adj = generateAdjacent(cur->i, cur->j, map, options);

        for (auto &n: adj) {
            if (CLOSED.count(n)) {
                continue;
            }
            double dist = getDistance(cur, n, options);

            n->g = cur->g + dist;
            n->parent = cur;

            OPEN.insert(n);
        }
    }

    if (CLOSED.count(goal) == 0) {
        sresult.pathfound = false;
        sresult.pathlength = 0;
        return sresult;
    }

    std::shared_ptr<Node> real_goal = *CLOSED.find(goal);
    sresult.pathfound = true;
    sresult.pathlength = static_cast<float>(real_goal->g);
    goal = real_goal;

    while (goal->v_key() != start->v_key()) {
        lppath.push_front(*goal);
        goal = goal->parent;
    }
    lppath.push_front(*goal);

    auto end_time = std::chrono::high_resolution_clock::now();
    sresult.time = (double) std::chrono::duration_cast<std::chrono::seconds>(
            end_time - start_time).count();

    buildHPPath();

    sresult.lppath = &lppath;
    sresult.hppath = &hppath;

    return sresult;
}

Dijkstra::~Dijkstra() = default;
