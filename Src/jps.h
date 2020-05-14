//
// Created by Roman Nikitin on 26.04.2020.
//

#ifndef PATHPLANNING_JPS_H
#define PATHPLANNING_JPS_H

#include "search.h"
#include "node.h"

class Jps : public Search {
public:
    Jps(ILogger *logger, const Map &map, const EnvironmentOptions &options);

    ~Jps() override;

    SearchResult
    startSearch() override;

protected:
    std::list<std::shared_ptr<Node>>
    horSearch(const std::shared_ptr<Node> &from, int dj);

    std::list<std::shared_ptr<Node>>
    vertSearch(const std::shared_ptr<Node> &from, int di);

    std::list<std::shared_ptr<Node>>
    diagonalSearch(const std::shared_ptr<Node> &from, int di, int dj);

    std::list<std::shared_ptr<Node>>
    runScan(const std::shared_ptr<Node> &from);
};


#endif //PATHPLANNING_JPS_H
