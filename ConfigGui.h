#ifndef CONFIGGUI_H
#define CONFIGGUI_H

#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <TROOT.h>



class ConfigGUI
{
public:
    static const std::string nullValue;

    ConfigGUI();

    bool loadGUIConfigFile(const char* config_file);
    bool loadGUIConfigFile(const std::string config_file);

    void writeGUIConfigFile(const char* filename);
    void writeGUIConfigFile(const std::string filename);

    std::string getGoATConfig();
    std::string getGoATExe();
    std::string getACQUDir();
    std::string getGoATDir();
    std::string getPhysicsDir();
    std::string getLastFile();

    void setLastACQUFile(std::string file);

    void PrintAll();

    static std::string ReadConfig(const std::string& key_in, const Int_t instance, const Char_t* configname);

private:
    std::string GoAT_Config;
    std::string GoAT_Exe;
    std::string ACQU_Dir;
    std::string GoAT_Dir;
    std::string Physics_Dir;
    std::string LastFile;

};


#endif // CONFIGGUI_H
