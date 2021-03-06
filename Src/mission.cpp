#include "mission.h"
#include "dijkstra.h"
#include "astar.h"
#include "jps.h"

Mission::Mission() {
    logger = nullptr;
    fileName = nullptr;
}

Mission::Mission(const char *FileName) {
    fileName = FileName;
    logger = nullptr;
}

Mission::~Mission() {
    delete logger;
}

bool Mission::getMap() {
    return map.getMap(fileName);
}

bool Mission::getConfig() {
    return config.getConfig(fileName);
}

bool Mission::createLog() {
    delete logger;
    logger = new XmlLogger(config.LogParams[CN_LP_LEVEL]);
    return logger->getLog(fileName, config.LogParams);
}

void Mission::createEnvironmentOptions() {
    options.cutcorners = config.SearchParams[CN_SP_CC];
    options.allowsqueeze = config.SearchParams[CN_SP_AS];
    options.allowdiagonal = config.SearchParams[CN_SP_AD];
    options.metrictype = config.SearchParams[CN_SP_MT];
    if (CN_SP_HW < config.N) {
        options.heuristicheight = config.SearchParams[CN_SP_HW];
    }
    if (CN_SP_BT < config.N) {
        options.breakingties = config.SearchParams[CN_SP_BT];
    }
}

void Mission::createSearch() {
    if (config.SearchParams[CN_SP_ST] == CN_SP_ST_ASTAR) {
        std::cout << "Using AStar search" << std::endl;
        search = std::make_unique<AStar>(logger, map, options);
    } else if (config.SearchParams[CN_SP_ST] == CN_SP_ST_JP_SEARCH) {
        std::cout << "Using JP search" << std::endl;
        search = std::make_unique<Jps>(logger, map, options);
    } else {
        std::cout << "Using Dijkstra search" << std::endl;
        search = std::make_unique<Dijkstra>(logger, map, options);
    }
}

void Mission::startSearch() {
    sr = search->startSearch();
}

void Mission::printSearchResultsToConsole() {
    std::cout << "Path ";
    if (!sr.pathfound)
        std::cout << "NOT ";
    std::cout << "found!" << std::endl;
    std::cout << "numberofsteps=" << sr.numberofsteps << std::endl;
    std::cout << "nodescreated=" << sr.nodescreated << std::endl;
    if (sr.pathfound) {
        std::cout << "pathlength=" << sr.pathlength << std::endl;
        std::cout << "pathlength_scaled=" << sr.pathlength * map.getCellSize() << std::endl;
    }
    std::cout << "time=" << sr.time << std::endl;
}

void Mission::saveSearchResultsToLog() {
    logger->writeToLogSummary(sr.numberofsteps, sr.nodescreated, sr.pathlength, sr.time,
                              map.getCellSize());
    if (sr.pathfound) {
        logger->writeToLogPath(*sr.lppath);
        logger->writeToLogHPpath(*sr.hppath);
        logger->writeToLogMap(map, *sr.lppath);
    } else
        logger->writeToLogNotFound();
    logger->saveLog();
}

SearchResult Mission::getSearchResult() {
    return sr;
}

