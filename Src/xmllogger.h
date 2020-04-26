#ifndef XMLLOGGER_H
#define    XMLLOGGER_H

#include <utility>

#include "auxiliary.h"
#include "tinyxml2.h"
#include "ilogger.h"


class XmlLogger : public ILogger {

public:
    explicit XmlLogger(std::string loglevel) : ILogger(std::move(loglevel)) {}

    ~XmlLogger() override = default;

    bool getLog(const char *FileName, const std::string *LogParams) override;

    void saveLog() override;

    void writeToLogMap(const Map &Map, const std::list<Node> &path) override;

    void writeToLogOpenClose(const BTSet &open, const FPSet &close, int step, bool last) override;

    void writeToLogPath(const std::list<Node> &path) override;

    void writeToLogHPpath(const std::list<Node> &hppath) override;

    void writeToLogNotFound() override;


    void writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, float length,
                           double time, double cellSize) override;

    void simpleWriteNodeInfo(const char *type, std::shared_ptr<Node> n) override;

    void simpleWriteNodeInfo(const char *type, int i, int j) override;

private:
    std::string LogFileName;
    tinyxml2::XMLDocument doc;
};

#endif

