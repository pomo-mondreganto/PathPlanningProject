//
// Created by Roman Nikitin on 22.12.2019.
//

#ifndef PATHPLANNING_DIJKSTRA_H
#define PATHPLANNING_DIJKSTRA_H

#include <type_traits>
#include <limits>
#include <cmath>
#include <algorithm>
#include <set>
#include <map>
#include <tuple>
#include <memory>
#include "node.h"
#include "searchresult.h"
#include "search.h"
#include "xmllogger.h"
#include "map.h"
#include "environmentoptions.h"


template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y, int ulp) {
    return std::abs(x - y) <= std::numeric_limits<T>::epsilon()
                              * std::max(std::abs(x), std::abs(y))
                              * ulp;
}

struct node_compare {
    bool operator()(const std::shared_ptr<Node> &n1, const std::shared_ptr<Node> &n2) const {
        if (!almost_equal(n1->g, n2->g, 8)) {
            return n1->g < n2->g;
        }
//        return std::tie(n1.g, n1.i, n1.j) < std::tie(n2.g, n2.i, n2.j);
        return n1->v_key() < n2->v_key();
    }
};

class Dijkstra : public Search {
protected:
    std::set<std::shared_ptr<Node>, node_compare> state;
    std::map<std::pair<int, int>, std::shared_ptr<Node>> which;

public:
    Dijkstra();

    ~Dijkstra() override;

    SearchResult
    startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) override;

};


#endif //PATHPLANNING_DIJKSTRA_H
