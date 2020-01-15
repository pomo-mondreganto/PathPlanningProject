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
#include "auxiliary.h"

class AStar : public Search {
protected:
    BTSet OPEN;
    FPSet CLOSED;

public:
    AStar();

    ~AStar() override;

    SearchResult
    startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) override;
};


#endif //PATHPLANNING_ASTAR_H
