//
// Created by Roman Nikitin on 15.01.2020.
//

#ifndef PATHPLANNING_ASTAR_H
#define PATHPLANNING_ASTAR_H

#include "node.h"
#include "searchresult.h"
#include "search.h"
#include "xmllogger.h"
#include "map.h"
#include "environmentoptions.h"

class AStar : public Search {
public:
    AStar(ILogger *logger, const Map &map, const EnvironmentOptions &options);

    ~AStar() override;

    SearchResult
    startSearch() override;
};


#endif //PATHPLANNING_ASTAR_H
