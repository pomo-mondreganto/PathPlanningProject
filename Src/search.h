#ifndef SEARCH_H
#define SEARCH_H

#include "auxiliary.h"
#include "ilogger.h"
#include "searchresult.h"
#include "map.h"
#include "environmentoptions.h"
#include <list>
#include <cmath>
#include <limits>
#include <chrono>

class Search {
public:
    Search(const Map &map, const EnvironmentOptions &options);

    virtual ~Search() = 0;

    virtual SearchResult
    startSearch(ILogger *Logger) = 0;

protected:
    std::list<std::shared_ptr<Node>> generateAdjacent(int i, int j);

    static double
    getDistance(const std::shared_ptr<Node> &first, const std::shared_ptr<Node> &second,
                int metrictype);

    static double
    getTime(const TP &start, const TP &end);

    void buildHPPath();

    SearchResult sresult;
    std::list<Node> lppath, hppath;

    BTSet OPEN;
    FPSet IN_OPEN;
    FPSet CLOSED;

    const Map &_map;
    const EnvironmentOptions &_options;
    std::shared_ptr<Node> _goal;
};

#endif
