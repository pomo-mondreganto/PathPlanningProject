//
// Created by Roman Nikitin on 22.12.2019.
//

#ifndef PATHPLANNING_DIJKSTRA_H
#define PATHPLANNING_DIJKSTRA_H

#include "node.h"
#include "searchresult.h"
#include "search.h"
#include "xmllogger.h"
#include "map.h"
#include "environmentoptions.h"
#include <set>
#include <memory>


class Dijkstra : public Search {
public:
    Dijkstra(ILogger *logger, const Map &map, const EnvironmentOptions &options);

    ~Dijkstra() override;

    SearchResult
    startSearch() override;
};


#endif //PATHPLANNING_DIJKSTRA_H
