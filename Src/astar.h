//
// Created by Roman Nikitin on 15.01.2020.
//

#ifndef PATHPLANNING_ASTAR_H
#define PATHPLANNING_ASTAR_H

#include <type_traits>
#include <limits>
#include <cmath>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <tuple>
#include <memory>
#include <functional>
#include "node.h"
#include "searchresult.h"
#include "search.h"
#include "xmllogger.h"
#include "map.h"
#include "environmentoptions.h"
#include "dijkstra.h"
#include "auxiliary.h"

class AStar : public Search {
protected:
    BTSet state;
    NMap which;

public:
    AStar();

    ~AStar() override;

    SearchResult
    startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) override;

};


#endif //PATHPLANNING_ASTAR_H
