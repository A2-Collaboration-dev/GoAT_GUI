#include <functional>
#include <cstring>
#include <vector>
#include <sstream>
#include <string>
 #include <QCoreApplication>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

#include <fstream>
#include <QDir>

#include <QFuture>
#include <QtConcurrentRun>
#include <stdlib.h>

static void getNamesList(std::string lookfor, std::vector<std::string> &pass, std::string config)
{
    bool keepLooking = false;

    std::istringstream pstream(config);

    std::string line;
    while( std::getline(pstream, line) )
    {
        if (line == std::string(lookfor))
        {
            keepLooking = true;
            continue;
        }

        if (line == std::string("END"))
            keepLooking = false;

        if (keepLooking == true && line[0] != '#')
            pass.push_back(line);

    }
}

/*
static std::string GUISettingsGet(std::string lookupKey)
{

    std::ifstream t(QCoreApplication::applicationDirPath().toStdString() + std::string("/config/settings.gui"));
    if (!t)
    {
        std::cout << "No GUI Settings file found." << std::endl;
        t.close();
        return "empty";
    }

    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    std::istringstream is_file(str.c_str());

    std::string line;
    while( std::getline(is_file, line) )
    {
      std::istringstream is_line(line);
      std::string key;
      if( std::getline(is_line, key, '=') )
      {
        std::string value;
        if( std::getline(is_line, value) )
          if (key == lookupKey)
              t.close();
              return value;
      }
    }
    t.close();
    return std::string("empty");
}
*/

static std::string getNewestFile(std::string iDirIn, char* extension)
{
    QString iDir = QString(iDirIn.c_str());
    QDir dir;
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Time);
    dir.setCurrent(iDir);

    QStringList filters;
    filters << "*.root";
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    if (list.empty())
        return "";

    return (list.at(0).absoluteFilePath().toStdString());
}


