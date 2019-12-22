#include "search.h"

Search::Search() = default;

Search::~Search() = default;


//SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
//{
//    //need to implement
//
//    /*sresult.pathfound = ;
//    sresult.nodescreated =  ;
//    sresult.numberofsteps = ;
//    sresult.time = ;
//    sresult.hppath = &hppath; //Here is a constant pointer
//    sresult.lppath = &lppath;*/
//    return sresult;
//}

std::list<std::shared_ptr<Node>> Search::generateAdjacent(int i, int j, const Map &map,
                                                          const EnvironmentOptions &options) {
    std::list<std::shared_ptr<Node>> result;

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
        if (map.CellOnGrid(ni, nj) && map.CellIsTraversable(ni, nj)) {
            result.push_back(std::make_shared<Node>(ni, nj, 0));
            ++sresult.nodescreated;
        }
    }

    if (!options.allowdiagonal) {
        return result;
    }

    std::vector<int> diag_di = {-1, -1, 1, 1};
    std::vector<int> diag_dj = {-1, 1, 1, -1};
    for (size_t d = 0; d < diag_di.size(); ++d) {
        int ni = i + diag_di[d];
        int nj = j + diag_dj[d];

        if (!map.CellOnGrid(ni, nj) || !map.CellIsTraversable(ni, nj)) {
            continue;
        }

        int a1i = i + di[d];
        int a1j = j + dj[d];
        int a2i = i + di[(d + 1) % di.size()];
        int a2j = j + dj[(d + 1) % dj.size()];

        int cnt_blocked = 0;
        if (!map.CellIsTraversable(a1i, a1j)) {
            ++cnt_blocked;
        }

        if (!map.CellIsTraversable(a2i, a2j)) {
            ++cnt_blocked;
        }

        if (cnt_blocked == 0) {
            result.push_back(std::make_shared<Node>(ni, nj, 0));
            ++sresult.nodescreated;
        } else if (cnt_blocked == 1 && options.cutcorners) {
            result.push_back(std::make_shared<Node>(ni, nj, 0));
            ++sresult.nodescreated;
        } else if (cnt_blocked == 2 && options.allowsqueeze) {
            result.push_back(std::make_shared<Node>(ni, nj, 0));
            ++sresult.nodescreated;
        }
    }

    return result;
}

double
Search::getDistance(const std::shared_ptr<Node> &first, const std::shared_ptr<Node> &second,
                    const EnvironmentOptions &options) {
    int di = std::abs(first->i - second->i);
    int dj = std::abs(first->j - second->j);
    switch (options.metrictype) {
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

/*void Search::makePrimaryPath(Node curNode)
{
    //need to implement
}*/

/*void Search::makeSecondaryPath()
{
    //need to implement
}*/
