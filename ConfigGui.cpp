#include "ConfigGui.h"

const std::string ConfigGUI::nullValue = "nullValue";

ConfigGUI::ConfigGUI()
{
    GoAT_Config = nullValue;
    GoAT_Exe = nullValue;
    Phys_Exe = nullValue;
    ACQU_Dir = nullValue;
    GoAT_Dir = nullValue;
    LastFile = nullValue;
    Physics_Dir = nullValue;
    LastGoATFile = nullValue;
    LastPhysFile = nullValue;
    CompleteACQUFile = nullValue;
    CompletePhysicsFile = nullValue;
    ACQU_prefix = "Acqu_";
    GoAT_prefix = "GoAT_";
    Phys_prefix = "Physics_";
}

ConfigGUI::ConfigGUI(std::string filepath)
{
    GoAT_Config = nullValue;
    GoAT_Exe = nullValue;
    Phys_Exe = nullValue;
    ACQU_Dir = nullValue;
    GoAT_Dir = nullValue;
    LastFile = nullValue;
    Physics_Dir = nullValue;
    LastGoATFile = nullValue;
    LastPhysFile = nullValue;
    ACQU_prefix = "Acqu_";
    GoAT_prefix = "GoAT_";
    Phys_prefix = "Physics_";
    ConfigFilePath = filepath;

}

void ConfigGUI::PrintAll()
{
    std::cout << this->GoAT_Exe << std::endl;
    std::cout << this->Phys_Exe << std::endl;
    std::cout << this->ACQU_Dir << std::endl;
    std::cout << this->GoAT_Dir << std::endl;
    std::cout << this->Physics_Dir << std::endl;
    std::cout << this->LastFile << std::endl;
    std::cout << this->LastGoATFile << std::endl;
}

void ConfigGUI::setConfigFilePath(std::string str){
    this->ConfigFilePath = str;
}

bool ConfigGUI::loadGUIConfigFile(std::string config_file)
{
    ifstream cfile(config_file.c_str());
    if(!cfile)
    {
        std::cout << "Config file '" << config_file << "' could not be found." << std::endl;
        return false;
    }

    this->GoAT_Config = ReadConfig("GoAT-Config",0,(Char_t*)config_file.c_str());
    this->GoAT_Exe = ReadConfig("GoAT-Executable",0,(Char_t*)config_file.c_str());
    this->Phys_Exe = ReadConfig("Phys-Executable",0,(Char_t*)config_file.c_str());
    this->ACQU_Dir = ReadConfig("ACQU-Dir",0,(Char_t*)config_file.c_str());
    this->GoAT_Dir = ReadConfig("GoAT-Dir",0,(Char_t*)config_file.c_str());
    this->Physics_Dir = ReadConfig("Physics-Dir",0,(Char_t*)config_file.c_str());
    this->LastFile = ReadConfig("LastFile",0,(Char_t*)config_file.c_str());
    this->LastGoATFile = ReadConfig("LastGoATFile",0,(Char_t*)config_file.c_str());
    this->LastPhysFile = ReadConfig("LastPhysFile",0,(Char_t*)config_file.c_str());
    this->ACQU_prefix = ReadConfig("ACQU-prefix",0,(Char_t*)config_file.c_str());
    this->GoAT_prefix = ReadConfig("GoAT-prefix",0,(Char_t*)config_file.c_str());
    this->Phys_prefix = ReadConfig("Phys-prefix",0,(Char_t*)config_file.c_str());
    this->CompleteACQUFile = ReadConfig("Complete-acqu",0,(Char_t*)config_file.c_str());
    this->CompletePhysicsFile = ReadConfig("Complete-physics",0,(Char_t*)config_file.c_str());
    return true;
}

bool ConfigGUI::loadGUIConfigFile(const char* config_file)
{
   return loadGUIConfigFile(std::string(config_file));
}

void ConfigGUI::setLastACQUFile(std::string file)
{
    this->LastFile = file;
}

void ConfigGUI::setLastGoATFile(std::string file)
{
    this->LastGoATFile = file;
}

void ConfigGUI::setLastPhysFile(std::string file)
{
    this->LastPhysFile = file;
}

void ConfigGUI::writeGUIConfigFile(const std::string filename)
{
    std::ofstream outfile (filename.c_str(), std::ofstream::binary);
    std::string outputData("");

    //if (this->GoAT_Config != nullValue)
    //    outputData.append("GoAT-Config: ").append(this->GoAT_Config).append("\n");
    if (this->GoAT_Exe != nullValue)
        outputData.append("GoAT-Executable: ").append(this->GoAT_Exe).append("\n");
    if (this->Phys_Exe != nullValue)
        outputData.append("Phys-Executable: ").append(this->Phys_Exe).append("\n");
    if (this->ACQU_Dir != nullValue)
        outputData.append("ACQU-Dir: ").append(this->ACQU_Dir).append("\n");
    if (this->GoAT_Dir != nullValue)
        outputData.append("GoAT-Dir: ").append(this->GoAT_Dir).append("\n");
    if (this->Physics_Dir != nullValue)
        outputData.append("Physics-Dir: ").append(this->Physics_Dir).append("\n");
    if (this->LastFile != nullValue)
        outputData.append("LastFile: ").append(this->LastFile).append("\n");
    if (this->LastGoATFile != nullValue)
        outputData.append("LastGoATFile: ").append(this->LastGoATFile).append("\n");
    if (this->LastPhysFile != nullValue)
        outputData.append("LastPhysFile: ").append(this->LastPhysFile).append("\n");
    if (this->CompleteACQUFile != nullValue)
        outputData.append("Complete-acqu: ").append(this->CompleteACQUFile).append("\n");
    if (this->CompletePhysicsFile != nullValue)
        outputData.append("Complete-physics: ").append(this->CompletePhysicsFile).append("\n");

        outputData.append("ACQU-prefix: ").append(this->ACQU_prefix).append("\n");
        outputData.append("GoAT-prefix: ").append(this->GoAT_prefix).append("\n");
        outputData.append("Phys-prefix: ").append(this->Phys_prefix).append("\n");

    outfile.write (outputData.c_str(),outputData.size());
    outfile.close();
}

void ConfigGUI::writeGUIConfigFile()
{
    this->writeGUIConfigFile(this->ConfigFilePath);
}

std::string ConfigGUI::ReadConfig(const std::string& key_in, const Int_t instance, const Char_t* configname)
{
    Int_t string_instance = 0;
    std::string key = key_in;
    std::transform(key.begin(), key.end(),key.begin(), ::toupper);

    std::string str;
    std::string values;

    ifstream configfile;

    configfile.open(configname);
    //std::cout << "config: " << configname << std::endl;

   bool found = false;

    if (configfile.is_open())
    {
        while ( getline (configfile,str) )
        {
            std::string::size_type begin = str.find_first_not_of(" \f\t\v");
            if(begin == std::string::npos) continue;
            if(std::string("#").find(str[begin]) != std::string::npos)	continue;
            if(std::string("//").find(str[begin]) != std::string::npos)	continue;

            std::string firstWord;


            try {
                firstWord = str.substr(0,str.find(":"));
            }
            catch(std::exception& e) {
                firstWord = str.erase(str.find_first_of(":"),str.find_first_of(":"));
            }
            std::transform(firstWord.begin(),firstWord.end(),firstWord.begin(), ::toupper);

            values = str.substr(str.find(":")+1,str.length());

            if (strcmp(firstWord.c_str(),key.c_str()) == 0)
            {
                if (string_instance == instance)
                {
                    configfile.close();
                    found = true;
                    break;
                }
                else string_instance++;
            }
        }
        configfile.close();
    }

    if (found == false)
        return nullValue;

    else
    {
          values.erase(0,values.find_first_not_of(" \t"));
          return values;
    }
}

std::string ConfigGUI::getGoATConfig()
{
    return this->GoAT_Config;
}

std::string ConfigGUI::getGoATExe()
{
    return this->GoAT_Exe;
}

std::string ConfigGUI::getPhysExe()
{
    return this->Phys_Exe;
}

std::string ConfigGUI::getACQUDir()
{
    return this->ACQU_Dir;
}

std::string ConfigGUI::getGoATDir()
{
    return this->GoAT_Dir;
}

std::string ConfigGUI::getPhysicsDir()
{
    return this->Physics_Dir;
}

std::string ConfigGUI::getLastFile()
{
    return this->LastFile;
}

std::string ConfigGUI::getLastGoATFile()
{
    return this->LastGoATFile;
}

std::string ConfigGUI::getLastPhysFile()
{
    return this->LastPhysFile;
}

std::string ConfigGUI::getACQUPrefix()
{
    return this->ACQU_prefix;
}

std::string ConfigGUI::getGoATPrefix()
{
    return this->GoAT_prefix;
}

std::string ConfigGUI::getPhysPrefix()
{
    return this->Phys_prefix;
}

std::string ConfigGUI::getCompleteACQUFile()
{
    return this->CompleteACQUFile;
}

std::string ConfigGUI::getCompletePhysicsFile()
{
    return this->CompletePhysicsFile;
}

void ConfigGUI::setCompleteACQUFile(std::string s)
{
    this->CompleteACQUFile = s;
}

void ConfigGUI::setCompletePhysicsFile(std::string s)
{
    this->CompletePhysicsFile = s;
}
