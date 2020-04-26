#include "auxiliary.h"
#include "xmllogger.h"
#include <iostream>

using tinyxml2::XMLElement;
using tinyxml2::XMLNode;

bool XmlLogger::getLog(const char *FileName, const std::string *LogParams) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD) return true;

    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening input XML file" << std::endl;
        return false;
    }

    if (LogParams[CN_LP_PATH] == "" && LogParams[CN_LP_NAME] == "") {
        std::string str;
        str.append(FileName);
        size_t found = str.find_last_of(".");
        if (found != std::string::npos)
            str.insert(found, "_log");
        else
            str.append("_log");
        LogFileName.append(str);
    } else if (LogParams[CN_LP_PATH] == "") {
        LogFileName.append(FileName);
        std::string::iterator it = LogFileName.end();
        while (*it != '\\')
            it--;
        ++it;
        LogFileName.erase(it, LogFileName.end());
        LogFileName.append(LogParams[CN_LP_NAME]);
    } else if (LogParams[CN_LP_NAME] == "") {
        LogFileName.append(LogParams[CN_LP_PATH]);
        if (*(--LogParams[CN_LP_PATH].end()) != '\\') LogFileName.append("\\");
        std::string lfn;
        lfn.append(FileName);
        size_t found = lfn.find_last_of("\\");
        std::string str = lfn.substr(found);
        found = str.find_last_of(".");
        if (found != std::string::npos)
            str.insert(found, "_log");
        else
            str.append("_log");
        LogFileName.append(str);
    } else {
        LogFileName.append(LogParams[CN_LP_PATH]);
        if (*(--LogParams[CN_LP_PATH].end()) != '\\') LogFileName.append("\\");
        LogFileName.append(LogParams[CN_LP_NAME]);
    }

    XMLElement *log, *root = doc.FirstChildElement(CNS_TAG_ROOT);

    if (!root) {
        std::cout << "No '" << CNS_TAG_ROOT << "' element found in XML file" << std::endl;
        std::cout << "Can not create log" << std::endl;
        return false;
    }

    root->InsertEndChild(doc.NewElement(CNS_TAG_LOG));

    root = (root->LastChild())->ToElement();

    if (loglevel != CN_LP_LEVEL_NOPE_WORD) {
        log = doc.NewElement(CNS_TAG_MAPFN);
        log->LinkEndChild(doc.NewText(FileName));
        root->InsertEndChild(log);

        root->InsertEndChild(doc.NewElement(CNS_TAG_SUM));

        root->InsertEndChild(doc.NewElement(CNS_TAG_PATH));

        root->InsertEndChild(doc.NewElement(CNS_TAG_LPLEVEL));

        root->InsertEndChild(doc.NewElement(CNS_TAG_HPLEVEL));
    }

    if (loglevel == CN_LP_LEVEL_FULL_WORD || loglevel == CN_LP_LEVEL_MEDIUM_WORD)
        root->InsertEndChild(doc.NewElement(CNS_TAG_LOWLEVEL));

    return true;
}

void XmlLogger::saveLog() {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD)
        return;
    doc.SaveFile(LogFileName.c_str());
}

void XmlLogger::writeToLogMap(const Map &map, const std::list<Node> &path) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD || loglevel == CN_LP_LEVEL_TINY_WORD)
        return;

    XMLElement *mapTag = doc.FirstChildElement(CNS_TAG_ROOT);
    mapTag = mapTag->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_PATH);

    int iterate = 0;
    bool inPath;
    std::string str;
    for (int i = 0; i < map.getMapHeight(); ++i) {
        XMLElement *element = doc.NewElement(CNS_TAG_ROW);
        element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);

        for (int j = 0; j < map.getMapWidth(); ++j) {
            inPath = false;
            for (const auto &it : path)
                if (it.i == i && it.j == j) {
                    inPath = true;
                    break;
                }
            if (!inPath)
                str += std::to_string(map.getValue(i, j));
            else
                str += CNS_OTHER_PATHSELECTION;
            str += CNS_OTHER_MATRIXSEPARATOR;
        }

        element->InsertEndChild(doc.NewText(str.c_str()));
        mapTag->InsertEndChild(element);
        str.clear();
        iterate++;
    }
}

void XmlLogger::writeToLogOpenClose(const BTSet &open, const FPSet &close, int step, bool last) {
    if (loglevel != CN_LP_LEVEL_FULL_WORD && !(loglevel == CN_LP_LEVEL_MEDIUM_WORD && last))
        return;
    if (loglevel == CN_LP_LEVEL_FULL_WORD && last)
        return;

    XMLElement *lowlevel = doc.FirstChildElement(CNS_TAG_ROOT)->FirstChildElement(
            CNS_TAG_LOG)->FirstChildElement(CNS_TAG_LOWLEVEL);

    if (lowlevel == nullptr) {
        lowlevel = doc.NewElement(CNS_TAG_LOWLEVEL);
        doc.FirstChildElement(CNS_TAG_ROOT)->FirstChildElement(CNS_TAG_LOG)->InsertEndChild(
                lowlevel);
    }

    XMLElement *stepEl = doc.NewElement(CNS_TAG_STEP);
    stepEl->SetAttribute(CNS_TAG_ATTR_NUM, step);
    lowlevel->InsertEndChild(stepEl);

    XMLElement *openEl = doc.NewElement(CNS_TAG_OPEN);
    stepEl->InsertEndChild(openEl);

    for (const auto &it : open) {
        XMLElement *elem = doc.NewElement(CNS_TAG_POINT);
        elem->SetAttribute(CNS_TAG_ATTR_X, it->j);
        elem->SetAttribute(CNS_TAG_ATTR_Y, it->i);
        elem->SetAttribute(CNS_TAG_ATTR_F, it->F);
        elem->SetAttribute(CNS_TAG_ATTR_G, it->g);
        if (it->parent != nullptr) {
            elem->SetAttribute(CNS_TAG_ATTR_PARX, it->parent->j);
            elem->SetAttribute(CNS_TAG_ATTR_PARY, it->parent->i);
        }
        openEl->InsertEndChild(elem);
    }

    XMLElement *closeEl = doc.NewElement(CNS_TAG_CLOSE);
    stepEl->InsertEndChild(closeEl);

    for (const auto &it : close) {
        XMLElement *elem = doc.NewElement(CNS_TAG_POINT);
        elem->SetAttribute(CNS_TAG_ATTR_X, it->j);
        elem->SetAttribute(CNS_TAG_ATTR_Y, it->i);
        elem->SetAttribute(CNS_TAG_ATTR_F, it->F);
        elem->SetAttribute(CNS_TAG_ATTR_G, it->g);
        if (it->parent != nullptr) {
            elem->SetAttribute(CNS_TAG_ATTR_PARX, it->parent->j);
            elem->SetAttribute(CNS_TAG_ATTR_PARY, it->parent->i);
        }
        closeEl->InsertEndChild(elem);
    }
}

void XmlLogger::writeToLogPath(const std::list<Node> &path) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD || loglevel == CN_LP_LEVEL_TINY_WORD || path.empty())
        return;
    int iterate = 0;
    XMLElement *lplevel = doc.FirstChildElement(CNS_TAG_ROOT);
    lplevel = lplevel->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_LPLEVEL);

    for (const auto &it : path) {
        XMLElement *element = doc.NewElement(CNS_TAG_POINT);
        element->SetAttribute(CNS_TAG_ATTR_X, it.j);
        element->SetAttribute(CNS_TAG_ATTR_Y, it.i);
        element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);
        lplevel->InsertEndChild(element);
        iterate++;
    }
}

void XmlLogger::writeToLogHPpath(const std::list<Node> &hppath) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD || loglevel == CN_LP_LEVEL_TINY_WORD ||
        hppath.empty())
        return;
    int partnumber = 0;
    XMLElement *hplevel = doc.FirstChildElement(CNS_TAG_ROOT);
    hplevel = hplevel->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_HPLEVEL);
    auto iter = hppath.begin();
    auto it = hppath.begin();

    while (iter != --hppath.end()) {
        XMLElement *part = doc.NewElement(CNS_TAG_SECTION);
        part->SetAttribute(CNS_TAG_ATTR_NUM, partnumber);
        part->SetAttribute(CNS_TAG_ATTR_STX, it->j);
        part->SetAttribute(CNS_TAG_ATTR_STY, it->i);
        ++iter;
        part->SetAttribute(CNS_TAG_ATTR_FINX, iter->j);
        part->SetAttribute(CNS_TAG_ATTR_FINY, iter->i);
        part->SetAttribute(CNS_TAG_ATTR_LENGTH, iter->g - it->g);
        hplevel->LinkEndChild(part);
        ++it;
        ++partnumber;
    }
}

void XmlLogger::writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated,
                                  float length, double time, double cellSize) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD)
        return;
    std::cout << "Writing summary, loglevel is " << loglevel << std::endl;

    XMLElement *summary = doc.FirstChildElement(CNS_TAG_ROOT);
    summary = summary->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_SUM);
    XMLElement *element = summary->ToElement();
    element->SetAttribute(CNS_TAG_ATTR_NUMOFSTEPS, numberofsteps);
    element->SetAttribute(CNS_TAG_ATTR_NODESCREATED, nodescreated);
    element->SetAttribute(CNS_TAG_ATTR_LENGTH, length);
    element->SetAttribute(CNS_TAG_ATTR_LENGTH_SCALED, length * cellSize);
    element->SetAttribute(CNS_TAG_ATTR_TIME, std::to_string(time).c_str());
}

void XmlLogger::writeToLogNotFound() {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD)
        return;

    XMLElement *node = doc.FirstChildElement(CNS_TAG_ROOT)->FirstChildElement(
            CNS_TAG_LOG)->FirstChildElement(CNS_TAG_PATH);
    node->InsertEndChild(doc.NewText("Path NOT found!"));
}

void XmlLogger::simpleWriteNodeInfo(const char *type, std::shared_ptr<Node> n) {
    if (loglevel != CN_LP_LEVEL_MEDIUM_WORD && loglevel != CN_LP_LEVEL_FULL_WORD) {
        return;
    }

    std::cerr << type << " " << n->i << " " << n->j << std::endl;
}

void XmlLogger::simpleWriteNodeInfo(const char *type, int i, int j) {
    if (loglevel != CN_LP_LEVEL_MEDIUM_WORD && loglevel != CN_LP_LEVEL_FULL_WORD) {
        return;
    }

    std::cerr << type << " " << i << " " << j << std::endl;
}
