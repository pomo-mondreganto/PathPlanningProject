#include "search.h"

Search::Search(const Map &map, const EnvironmentOptions &options) : _map(map), _options(options),
                                                                    _goal(map.get_goal_node()) {}

Search::~Search() = default;

std::list<std::pair<int, int>> Search::generateAdjacent(int i, int j) {
    std::list<std::pair<int, int>> result;

    /*
     526
     1.3
     847
     */

    std::vector<int> di = {0, -1, 0, 1};
    std::vector<int> dj = {-1, 0, 1, 0};
    for (size_t d = 0; d < di.size(); ++d) {
        int ni = i + di[d];
        int nj = j + dj[d];
        if (_map.CellOnGrid(ni, nj) && _map.CellIsTraversable(ni, nj)) {
            result.emplace_back(ni, nj);
        }
    }

    if (!_options.allowdiagonal) {
        return result;
    }

    std::vector<int> diag_di = {-1, -1, 1, 1};
    std::vector<int> diag_dj = {-1, 1, 1, -1};
    for (size_t d = 0; d < diag_di.size(); ++d) {
        int ni = i + diag_di[d];
        int nj = j + diag_dj[d];

        if (!_map.CellOnGrid(ni, nj) || !_map.CellIsTraversable(ni, nj)) {
            continue;
        }

        int a1i = i + di[d];
        int a1j = j + dj[d];
        int a2i = i + di[(d + 1) % di.size()];
        int a2j = j + dj[(d + 1) % dj.size()];

        int cnt_blocked = 0;
        if (!_map.CellIsTraversable(a1i, a1j)) {
            ++cnt_blocked;
        }

        if (!_map.CellIsTraversable(a2i, a2j)) {
            ++cnt_blocked;
        }

        if (cnt_blocked == 0 ||
            (cnt_blocked == 1 && _options.cutcorners) ||
            (cnt_blocked == 2 && _options.allowsqueeze)) {
            result.emplace_back(ni, nj);
        }
    }

    return result;
}

double
Search::getDistance(const std::shared_ptr<Node> &first, const std::shared_ptr<Node> &second,
                    int metrictype) {
    int di = std::abs(first->i - second->i);
    int dj = std::abs(first->j - second->j);
    switch (metrictype) {
        case CN_SP_MT_MANH:
            return std::abs(di) + std::abs(dj);
        case CN_SP_MT_EUCL:
            return std::sqrt(di * di + dj * dj);
        case CN_SP_MT_CHEB:
            return std::max(di, dj);
        case CN_SP_MT_DIAG:
            return std::max(di, dj) - std::min(di, dj) + std::min(di, dj) * std::sqrt(2);
        default:
            return -1;
    }
}

std::shared_ptr<Node>
Search::createNode(const std::shared_ptr<Node> &prev, int i, int j, int di, int dj, bool add) {
    std::shared_ptr<Node> to_add = std::make_shared<Node>(i, j, 0);
    to_add->di = di;
    to_add->dj = dj;
    to_add->g = prev->g + getDistance(prev, to_add, CN_SP_MT_EUCL);
    to_add->H = getDistance(to_add, _goal, _options.metrictype);
    to_add->F = to_add->g + _options.heuristicheight * to_add->H;

    if (add && CLOSED.find(to_add) == CLOSED.end()) {
        auto it = IN_OPEN.find(to_add);
        if (it != IN_OPEN.end()) {
            if ((*it)->F > to_add->F) {
                IN_OPEN.erase(it);
                IN_OPEN.insert(to_add);
                OPEN.erase(*it);
                OPEN.insert(to_add);
            }
        } else {
            IN_OPEN.insert(to_add);
            OPEN.insert(to_add);
        }
    }

    return to_add;
}

void Search::buildHPPath() {
    for (auto it = lppath.begin(); it != lppath.end(); ++it) {
        if (it == lppath.begin()) {
            hppath.push_back(*it);
            continue;
        }
        auto prev = it;
        --prev;
        if (prev == lppath.begin()) {
            continue;
        }

        auto pprev = prev;
        --pprev;

        int di1 = it->i - prev->i;
        int dj1 = it->j - prev->j;
        int di2 = prev->i - pprev->i;
        int dj2 = prev->j - pprev->j;

        if (di1 != di2 || dj1 != dj2) {
            hppath.push_back(*prev);
        }
    }
    hppath.push_back(lppath.back());
}

double Search::getTime(const TP &start, const TP &end) {
    double duration = static_cast<double>(
            std::chrono::duration_cast<std::chrono::microseconds>(
                    end - start).count()
    );
    duration /= 1000000.0;
    return duration;
}
