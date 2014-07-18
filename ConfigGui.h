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
    ConfigGUI(std::string filepath);

    bool loadGUIConfigFile(const char* config_file);
    bool loadGUIConfigFile(const std::string config_file);

    void writeGUIConfigFile(const char* filename);
    void writeGUIConfigFile(const std::string filename);

    void writeGUIConfigFile();

    std::string getGoATConfig();
    std::string getGoATExe();
    std::string getPhysExe();
    std::string getACQUDir();
    std::string getGoATDir();
    std::string getPhysicsDir();
    std::string getLastFile();
    std::string getLastGoATFile();
    std::string getLastPhysFile();

    std::string getACQUPrefix();
    std::string getGoATPrefix();
    std::string getPhysPrefix();

    std::string getCompleteACQUFile();
    std::string getCompletePhysicsFile();

    void setLastFile(std::string);
    void setLastGoATFile(std::string file);
    void setLastACQUFile(std::string file);
    void setLastPhysFile(std::string file);

    void setCompleteACQUFile(std::string);
    void setCompletePhysicsFile(std::string);

    void setConfigFilePath(std::string);

    void PrintAll();

    static std::string ReadConfig(const std::string& key_in, const Int_t instance, const Char_t* configname);

private:
    std::string GoAT_Config;
    std::string GoAT_Exe;
    std::string Phys_Exe;
    std::string ACQU_Dir;
    std::string GoAT_Dir;
    std::string Physics_Dir;
    std::string LastFile;
    std::string LastGoATFile;
    std::string LastPhysFile;
    std::string ConfigFilePath;

    std::string ACQU_prefix;
    std::string GoAT_prefix;
    std::string Phys_prefix;

    std::string CompleteACQUFile;
    std::string CompletePhysicsFile;
};


#endif // CONFIGGUI_H
