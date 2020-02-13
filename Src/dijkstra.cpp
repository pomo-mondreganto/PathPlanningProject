//
// Created by Roman Nikitin on 22.12.2019.
//

#include "auxiliary.h"
#include "dijkstra.h"
#include "gl_const.h"
#include "search.h"
#include <list>
#include <chrono>

Dijkstra::Dijkstra() {
    sresult.nodescreated = 0;
}

SearchResult
Dijkstra::startSearch(ILogger *logger, const Map &map, const EnvironmentOptions &options) {
    OPEN = BTSet(g_node_compare{});
    TP start_time = std::chrono::high_resolution_clock::now();

    std::shared_ptr<Node> start = map.get_start_node();
    std::shared_ptr<Node> goal = map.get_goal_node();

    OPEN.insert(start);
    while (!OPEN.empty() && CLOSED.count(goal) == 0) {
        std::shared_ptr<Node> cur = *OPEN.begin();
        OPEN.erase(OPEN.begin());
        if (CLOSED.count(cur)) {
            continue;
        }
        CLOSED.insert(cur);
        ++sresult.numberofsteps;

        std::list<std::shared_ptr<Node>> adj = generateAdjacent(cur->i, cur->j, map, options);

        for (auto &n: adj) {
            if (CLOSED.count(n)) {
                continue;
            }
            double dist = getDistance(cur, n, CN_SP_MT_EUCL);

            n->g = cur->g + dist;
            n->parent = cur;

            OPEN.insert(n);
        }
        logger->writeToLogOpenClose(OPEN, CLOSED, static_cast<int>(sresult.numberofsteps) - 1,
                                    false);
    }

    logger->writeToLogOpenClose(OPEN, CLOSED, static_cast<int>(sresult.numberofsteps) - 1,
                                true);

    sresult.nodescreated = OPEN.size() + CLOSED.size();

    if (CLOSED.count(goal) == 0) {
        sresult.pathfound = false;
        sresult.pathlength = 0;

        TP end_time = std::chrono::high_resolution_clock::now();
        sresult.time = getTime(start_time, end_time);

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
    sresult.time = getTime(start_time, end_time);

    buildHPPath();

    sresult.lppath = &lppath;
    sresult.hppath = &hppath;

    return sresult;
}

Dijkstra::~Dijkstra() = default;
