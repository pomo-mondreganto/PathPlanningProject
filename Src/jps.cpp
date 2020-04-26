//
// Created by Roman Nikitin on 26.04.2020.
//

#include "jps.h"
#include "auxiliary.h"
#include "node.h"
#include <list>
#include <memory>

Jps::Jps(const Map &map, const EnvironmentOptions &options) : Search(map, options) {
    sresult.nodescreated = 0;
}

std::list<std::shared_ptr<Node>>
Jps::horSearch(const std::shared_ptr<Node> &from, int dj) {
    int i1 = from->i;
    int j1 = from->j;

    std::list<std::shared_ptr<Node>> result;

    while (true) {
        j1 = j1 + dj;

        if (_map.CellIsBlocked(i1, j1)) {
            break;
        }

        if (i1 == _goal->i && j1 == _goal->j) {
            result.push_back(createNode(from, i1, j1, 0, 0));
            break;
        }

        int j2 = j1 + dj;
        if (_map.CellIsBlocked(i1 - 1, j1) && !_map.CellIsBlocked(i1 - 1, j2)) {
            result.push_back(createNode(from, i1, j1, -1, dj));
        }

        if (_map.CellIsBlocked(i1 + 1, j1) && !_map.CellIsBlocked(i1 + 1, j2)) {
            result.push_back(createNode(from, i1, j1, 1, dj));
        }

        if (!result.empty()) {
            result.push_back(createNode(from, i1, j1, 0, dj));
            break;
        }
    }

    return result;
}

std::list<std::shared_ptr<Node>>
Jps::vertSearch(const std::shared_ptr<Node> &from, int di) {
    int i1 = from->i;
    int j1 = from->j;

    std::shared_ptr<Node> goal = _map.get_goal_node();
    std::list<std::shared_ptr<Node>> result;

    while (true) {
        i1 = i1 + di;
        if (_map.CellIsBlocked(i1, j1)) {
            break;
        }

        if (i1 == goal->i && j1 == goal->j) {
            result.push_back(createNode(from, i1, j1, 0, 0));
            break;
        }

        int i2 = i1 + di;
        if (_map.CellIsBlocked(i1, j1 - 1) && !_map.CellIsBlocked(i2, j1 - 1)) {
            result.push_back(createNode(from, i1, j1, di, -1));
        }

        if (_map.CellIsBlocked(i1, j1 + 1) && !_map.CellIsBlocked(i2, j1 + 1)) {
            result.push_back(createNode(from, i1, j1, di, 1));
        }

        if (!result.empty()) {
            result.push_back(createNode(from, i1, j1, di, 0));
            break;
        }
    }

    return result;
}

std::list<std::shared_ptr<Node>>
Jps::diagonalSearch(const std::shared_ptr<Node> &from, int di, int dj) {
    int i1 = from->i;
    int j1 = from->j;

    std::shared_ptr<Node> goal = _map.get_goal_node();
    std::list<std::shared_ptr<Node>> result;

    while (true) {
        int i0 = i1;
        int j0 = j1;
        i1 = i1 + di;
        j1 = j1 + dj;
        if (_map.CellIsBlocked(i1, j1)) {
            break;
        }

        if (i1 == goal->i && j1 == goal->j) {
            result.push_back(createNode(from, i1, j1, 0, 0));
            break;
        }

        int i2 = i1 + di;
        int j2 = j1 + dj;

        if (_map.CellIsBlocked(i0, j1) && !_map.CellIsBlocked(i0, j2)) {
            result.push_back(createNode(from, i1, j1, -di, dj));
        }

        if (_map.CellIsBlocked(i1, j0) && !_map.CellIsBlocked(i2, j0)) {
            result.push_back(createNode(from, i1, j1, di, -dj));
        }

        bool did_horizontal = false;
        bool did_vertical = false;

        if (result.empty()) {
            std::shared_ptr<Node> cur = createNode(from, i1, j1, 0, dj, false);
            std::list<std::shared_ptr<Node>> horizontal_result = horSearch(cur, dj);
            did_horizontal = true;

            if (!horizontal_result.empty()) {
                CLOSED.insert(cur);

                for (auto &it: horizontal_result) {
                    it->parent = cur;
                }
                result.splice(result.end(), horizontal_result);
            }
        }

        if (result.empty()) {
            std::shared_ptr<Node> cur = createNode(from, i1, j1, di, 0, false);
            std::list<std::shared_ptr<Node>> vertical_result = vertSearch(cur, di);
            did_vertical = true;

            if (!vertical_result.empty()) {
                CLOSED.insert(cur);

                for (auto &it: vertical_result) {
                    it->parent = cur;
                }
                result.splice(result.end(), vertical_result);
            }
        }

        if (!result.empty()) {
            if (!did_horizontal) {
                result.push_back(createNode(from, i1, j1, 0, dj));
            }
            if (!did_vertical) {
                result.push_back(createNode(from, i1, j1, di, 0));
            }

            result.push_back(createNode(from, i1, j1, di, dj));
            break;
        }
    }

    return result;
}

SearchResult
Jps::startSearch(ILogger *logger) {
    _goal = _map.get_goal_node();
    f_node_compare comp{_options.breakingties};

    OPEN = BTSet(comp);

    if (_map.getMapHeight() * _map.getMapWidth() > BIG_MAP_THRESHOLD) {
        CLOSED.rehash(INITIAL_FPSET_SIZE);
        IN_OPEN.rehash(INITIAL_FPSET_SIZE);
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    std::shared_ptr<Node> start = _map.get_start_node();
    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            if (!di && !dj) {
                continue;
            }
            createNode(start, start->i, start->j, di, dj);
        }
    }

    while (!OPEN.empty() && CLOSED.count(_goal) == 0) {
        std::shared_ptr<Node> cur = *OPEN.begin();

        OPEN.erase(OPEN.begin());
        IN_OPEN.erase(cur);

        if (CLOSED.count(cur)) {
            continue;
        }
        CLOSED.insert(cur);
        ++sresult.numberofsteps;

        std::list<std::shared_ptr<Node>> jump_points;
        if (cur->di && cur->dj) {
            jump_points = diagonalSearch(cur, cur->di, cur->dj);
        } else if (cur->di) {
            jump_points = vertSearch(cur, cur->di);
        } else {
            jump_points = horSearch(cur, cur->dj);
        }

        for (auto &it : jump_points) {
            it->parent = cur;
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

Jps::~Jps() = default;
