//
// Created by Roman Nikitin on 15.01.2020.
//

#include "astar.h"
#include "gl_const.h"
#include <chrono>

AStar::AStar(const Map &map, const EnvironmentOptions &options) : Search(map, options) {
    sresult.nodescreated = 0;
}

SearchResult
AStar::startSearch(ILogger *logger) {
    f_node_compare comp{_options.breakingties};

    OPEN = BTSet(comp);

    if (_map.getMapHeight() * _map.getMapWidth() > BIG_MAP_THRESHOLD) {
        CLOSED.rehash(INITIAL_FPSET_SIZE);
        IN_OPEN.rehash(INITIAL_FPSET_SIZE);
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    std::shared_ptr<Node> start = _map.get_start_node();

    IN_OPEN.insert(start);
    OPEN.insert(start);
    while (!OPEN.empty() && CLOSED.count(_goal) == 0) {
        std::shared_ptr<Node> cur = *OPEN.begin();

        OPEN.erase(OPEN.begin());
        IN_OPEN.erase(cur);

        if (CLOSED.count(cur)) {
            continue;
        }
        CLOSED.insert(cur);
        ++sresult.numberofsteps;

        std::list<std::pair<int, int>> adj = generateAdjacent(cur->i, cur->j);

        for (auto &n: adj) {
            std::shared_ptr<Node> new_node = createNode(cur, n.first, n.second, 0, 0);
            new_node->parent = cur;
        }

        logger->writeToLogOpenClose(OPEN, CLOSED, static_cast<int>(sresult.numberofsteps) - 1,
                                    false);
    }

    sresult.nodescreated = OPEN.size() + CLOSED.size();

    logger->writeToLogOpenClose(OPEN, CLOSED, static_cast<int>(sresult.numberofsteps) - 1,
                                true);

    if (CLOSED.count(_goal) == 0) {
        sresult.pathfound = false;
        sresult.pathlength = 0;

        auto end_time = std::chrono::high_resolution_clock::now();
        sresult.time = getTime(start_time, end_time);

        return sresult;
    }

    std::shared_ptr<Node> real_goal = *CLOSED.find(_goal);
    sresult.pathfound = true;
    sresult.pathlength = static_cast<float>(real_goal->g);
    std::shared_ptr<Node> goal = real_goal;

    while (goal->v_key() != _map.get_start_node()->v_key()) {
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

AStar::~AStar() = default;
