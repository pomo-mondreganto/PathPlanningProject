#ifndef SEARCH_H
#define SEARCH_H

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
    //CODE HERE

    //Hint 1. You definetely need class variables for OPEN and CLOSE

    //Hint 2. It's a good idea to define a heuristic calculation function, that will simply return 0
    //for non-heuristic search methods like Dijkstra

    //Hint 3. It's a good idea to define function that given a node (and other stuff needed)
    //will return it's sucessors, e.g. unordered list of nodes

    //Hint 4. working with OPEN and CLOSE is the core
    //so think of the data structures that needed to be used, about the wrap-up classes (if needed)
    //Start with very simple (and ineffective) structures like list or vector and make it work first
    //and only then begin enhancement!

    std::list<std::shared_ptr<Node>> generateAdjacent(int i, int j, const Map &map,
                                                      const EnvironmentOptions &options);

    static double
    getDistance(const std::shared_ptr<Node> &first, const std::shared_ptr<Node> &second,
                const EnvironmentOptions &options);

    void buildHPPath();

    SearchResult sresult;
    std::list<Node> lppath, hppath; //

    //CODE HERE to define other members of the class
};

#endif
