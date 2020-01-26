#ifndef SEARCH_H
#define SEARCH_H

#include "auxiliary.h"
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include <list>
#include <cmath>
#include <limits>
#include <chrono>

class Search {
public:
    Search();

    virtual ~Search() = 0;

    virtual SearchResult
    startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) = 0;

protected:
    std::list<std::shared_ptr<Node>> generateAdjacent(int i, int j, const Map &map,
                                                      const EnvironmentOptions &options);

    static double
    getDistance(const std::shared_ptr<Node> &first, const std::shared_ptr<Node> &second,
                const EnvironmentOptions &options);

    static double
    getTime(const TP &start, const TP &end);

    void buildHPPath();

    SearchResult sresult;
    std::list<Node> lppath, hppath;

    BTSet OPEN;
    FPSet CLOSED;
};

#endif
