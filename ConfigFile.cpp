#include "ConfigFile.h"

#include <iostream>

using namespace std;

const std::string ConfigFile::nullValue = "nokey";

const std::vector<std::string> ConfigFile::TextFields = {
"#=======================================================================\n\
# Global Config file for a GoAT analysis\n\
#=======================================================================\n\
#-----------------------------------------------------------------------\n\
# General Setup\n\
#-----------------------------------------------------------------------\n\n\
Period-Macro:	100000\n\n",


"#-----------------------------------------------------------------------\n\
# Particle Reconstruction\n\
#-----------------------------------------------------------------------\n",

"\n#-----------------------------------------------------------------------\n\
# Meson Reconstruction\n\
#-----------------------------------------------------------------------\n",

"\n#-----------------------------------------------------------------------\n\
# Sorting Preferences\n\
#-----------------------------------------------------------------------\n\
# Just turn off a sort completely to ignore a sorting preference\n\
# Use +, - or = to set counters (ex. 3- will accept 3 or less)\n\n\
# Sort on number of raw particle tracks (total, CB, and TAPS)\n\
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n\
#					Total 	CB		TAPS\n",

"\n#----------------------------------------------------------------------\n\
# Time Cuts\n\
#-----------------------------------------------------------------------\n"
};

ConfigFile::ConfigFile()
{
    this->ParticleReconstruction = std::string("1");
    this->ChargedParticleReconstruction = nullValue;
    this->MesonReconstruction = std::string("0");

    this->Cut_dE_E_CB_Pion = nullValue;
    this->Cut_dE_E_CB_Proton = nullValue;
    this->Cut_dE_E_CB_Electron = nullValue;

    this->Cut_dE_E_TAPS_Pion = nullValue;
    this->Cut_dE_E_TAPS_Proton = nullValue;
    this->Cut_dE_E_TAPS_Electron = nullValue;

    this->Cut_IM_Width_Pi0 = nullValue;
    this->Cut_IM_Width_Eta = nullValue;
    this->Cut_IM_Width_Eta_Prime = nullValue;

    this->SortRawNParticlesTotal = std::string("0+");
    this->SortRawNParticlesCB = std::string("0+");
    this->SortRawNParticlesTAPS = std::string("0+");

    this->SortRawCBEnergySum = nullValue;
    this->SortNParticles = nullValue;

    this->TimeCutMinApparatus1 = nullValue;
    this->TimeCutMaxApparatus1 = nullValue;
    this->TimeCutMinApparatus2 = nullValue;
    this->TimeCutMaxApparatus2 = nullValue;
}

ConfigFile::ConfigFile(std::string file)
{
    this->configFile = file;
}

void ConfigFile::loadGoATConfigFile(std::string config_file)
{
    ifstream cfile(config_file.c_str());
    if(!cfile)
    {
        cout << "Config file '" << config_file << "' could not be found." << endl;
        return;
        /* HERE */
    }

    std::string flag;

    this->ParticleReconstruction = ReadConfig("Do-Particle-Reconstruction",0,(Char_t*)config_file.c_str());
    this->MesonReconstruction = ReadConfig("Do-Meson-Reconstruction",0,(Char_t*)config_file.c_str());

    this->Cut_dE_E_CB_Pion = ReadConfig("Cut-dE-E-CB-Pion",0,(Char_t*)config_file.c_str());
    this->Cut_dE_E_CB_Proton = ReadConfig("Cut-dE-E-CB-Proton",0,(Char_t*)config_file.c_str());
    this->Cut_dE_E_CB_Electron = ReadConfig("Cut-dE-E-CB-Electron",0,(Char_t*)config_file.c_str());

    this->Cut_dE_E_TAPS_Pion = ReadConfig("Cut-dE-E-TAPS-Pion",0,(Char_t*)config_file.c_str());
    this->Cut_dE_E_TAPS_Proton = ReadConfig("Cut-dE-E-TAPS-Proton",0,(Char_t*)config_file.c_str());
    this->Cut_dE_E_TAPS_Electron = ReadConfig("Cut-dE-E-TAPS-Electron",0,(Char_t*)config_file.c_str());

    this->Cut_IM_Width_Pi0 = ReadConfig("Cut-IM-Width-Pi0",0,(Char_t*)config_file.c_str());
    this->Cut_IM_Width_Eta = ReadConfig("Cut-IM-Width-Eta",0,(Char_t*)config_file.c_str());
    this->Cut_IM_Width_Eta_Prime = ReadConfig("Cut-IM-Width-Eta-Prime",0,(Char_t*)config_file.c_str());

    flag = ReadConfig("SortRaw-NParticles",0,(Char_t*)config_file.c_str());
    this->SortRawNParticlesTotal = flag.substr(0, flag.find_first_of(" \t"));
    flag.erase(0,flag.find_first_of(" \t"));
    flag.erase(0, flag.find_first_of("0123456789"));
    this->SortRawNParticlesCB = flag.substr(0, flag.find_first_of(" \t"));
    flag.erase(0,flag.find_first_of(" \t"));
    flag.erase(0, flag.find_first_of("0123456789"));
    this->SortRawNParticlesTAPS = flag.substr(0, flag.find_first_of(" \t \n"));

    this->SortRawCBEnergySum = ReadConfig("SortRaw-CBEnergySum",0,(Char_t*)config_file.c_str());
    this->SortNParticles = ReadConfig("Sort-NParticles",0,(Char_t*)config_file.c_str());

    this->TimeCutMinApparatus1 = ReadConfig("TimeCutMin1",0,(Char_t*)config_file.c_str());
    this->TimeCutMaxApparatus1 = ReadConfig("TimeCutMax1",0,(Char_t*)config_file.c_str());
    this->TimeCutMinApparatus2 = ReadConfig("TimeCutMin2",0,(Char_t*)config_file.c_str());
    this->TimeCutMaxApparatus2 = ReadConfig("TimeCutMax2",0,(Char_t*)config_file.c_str());

}

void ConfigFile::writeGoATConfigFile(const std::string filename)
{
    std::ofstream outfile (filename.c_str(),std::ofstream::binary);
    std::string outputData("");

    outputData.append(TextFields[0]);
    outputData.append(TextFields[1]);
    //outputData.append("Period-Macro:	100000\nCheckCBStability: 1\n");

    if (this->ParticleReconstruction != nullValue)
        outputData.append("Do-Particle-Reconstruction: ").append(this->ParticleReconstruction).append("\n");
    if (this->ChargedParticleReconstruction != nullValue)
        outputData.append("Do-Charged-Particle-Reconstruction: ").append(this->ChargedParticleReconstruction).append("\n");
    if (this->Cut_dE_E_CB_Proton != nullValue)
        outputData.append("Cut-dE-E-CB-Proton: ").append(this->Cut_dE_E_CB_Proton).append("\n");
    if (this->Cut_dE_E_CB_Pion != nullValue)
        outputData.append("Cut-dE-E-CB-Pion: ").append(this->Cut_dE_E_CB_Pion).append("\n");
    if (this->Cut_dE_E_CB_Electron != nullValue)
        outputData.append("Cut-dE-E-CB-Electron: ").append(this->Cut_dE_E_CB_Electron).append("\n");
    if (this->Cut_dE_E_TAPS_Proton != nullValue)
        outputData.append("Cut-dE-E-TAPS-Proton: ").append(this->Cut_dE_E_TAPS_Proton).append("\n");
    if (this->Cut_dE_E_TAPS_Pion != nullValue)
        outputData.append("Cut-dE-E-TAPS-Pion: ").append(this->Cut_dE_E_TAPS_Pion).append("\n");
    if (this->Cut_dE_E_TAPS_Electron != nullValue)
        outputData.append("Cut-dE-E-TAPS-Electron: ").append(this->Cut_dE_E_TAPS_Electron).append("\n");
    if (this->MesonReconstruction != nullValue)

    outputData.append("\n");
    outputData.append(TextFields[2]);
        outputData.append("Do-Meson-Reconstruction: ").append(this->MesonReconstruction).append("\n");
    if (this->Cut_IM_Width_Pi0 != nullValue)
        outputData.append("Cut-IM-Width-Pi0: ").append(this->Cut_IM_Width_Pi0).append("\n");
    if (this->Cut_IM_Width_Eta != nullValue)
        outputData.append("Cut-IM-Width-Eta: ").append(this->Cut_IM_Width_Eta).append("\n");
    if (this->Cut_IM_Width_Eta_Prime != nullValue)
        outputData.append("Cut-IM-Width-Eta-Prime: ").append(this->Cut_IM_Width_Eta_Prime).append("\n\n");


    outputData.append(TextFields[3]);

    if (this->SortRawNParticlesTotal != nullValue &&
        this->SortRawNParticlesCB != nullValue &&
        this->SortRawNParticlesTAPS != nullValue)
    {
        outputData.append("SortRaw-NParticles: ").append(this->SortRawNParticlesTotal).append("\t\t");
        outputData.append(this->SortRawNParticlesCB).append("\t\t");
        outputData.append(this->SortRawNParticlesTAPS).append("\n");
    }

    if (this->SortRawCBEnergySum != nullValue)
        outputData.append("SortRaw-CBEnergySum: ").append(this->SortRawCBEnergySum).append("\n");
    if (this->SortNParticles != nullValue)
        outputData.append("Sort-NParticles: ").append(this->SortNParticles).append("\n");

    outputData.append(TextFields[4]);
    if (this->TimeCutMinApparatus1 != nullValue)
        outputData.append("TimeCutMin1: ").append(this->TimeCutMinApparatus1).append("\n");
    if (this->TimeCutMaxApparatus1 != nullValue)
        outputData.append("TimeCutMax1: ").append(this->TimeCutMaxApparatus1).append("\n");
    if (this->TimeCutMinApparatus2 != nullValue)
        outputData.append("TimeCutMin2: ").append(this->TimeCutMinApparatus2).append("\n");
    if (this->TimeCutMaxApparatus2 != nullValue)
        outputData.append("TimeCutMax2: ").append(this->TimeCutMaxApparatus2).append("\n");

    outfile.write (outputData.c_str(),outputData.size());
    outfile.close();

}

void ConfigFile::loadGoATConfigFile(const char *config_file)
{
    ConfigFile::loadGoATConfigFile(std::string(config_file));
}

void ConfigFile::writeGoATConfigFile(const char *filename)
{
    ConfigFile::writeGoATConfigFile(std::string(filename));
}

std::string ConfigFile::ReadConfig(const std::string& key_in, const Int_t instance, const Char_t* configname)
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

std::string ConfigFile::ReadConfig(const std::string& key_in, std::string configname)
{
    return ConfigFile::ReadConfig(key_in, 0, (Char_t*)(configname.c_str()));
}


void ConfigFile::setParticleReconstruction(int val)
{
    this->ParticleReconstruction = std::to_string(val);
}

void ConfigFile::setChargedParticleReconstruction(int val)
{
    this->ChargedParticleReconstruction = std::to_string(val);
}


void ConfigFile::setMesonReconstruction(int val)
{
    this->MesonReconstruction = std::to_string(val);
}

void ConfigFile::setCut_dE_E_CB_Proton(std::string str)
{
    std::string temp = str;
    temp.append(" Proton");
    this->Cut_dE_E_CB_Proton = temp;
}

void ConfigFile::setCut_dE_E_CB_Pion(std::string str)
{
    std::string temp = str;
    temp.append(" Pion");
    this->Cut_dE_E_CB_Pion = temp;
}

void ConfigFile::setCut_dE_E_CB_Electron(std::string str)
{
    std::string temp = str;
    temp.append(" Electron");
    this->Cut_dE_E_CB_Electron = temp;
}

void ConfigFile::setCut_dE_E_Taps_Proton(std::string str)
{
    std::string temp = str;
    temp.append(" CutProton");
    this->Cut_dE_E_TAPS_Proton = temp;
}

void ConfigFile::setCut_dE_E_Taps_Pion(std::string str)
{
    std::string temp = str;
    temp.append(" CutPiplus");
    this->Cut_dE_E_TAPS_Pion = temp;
}

void ConfigFile::setCut_dE_E_Taps_Electron(std::string str)
{
    std::string temp = str;
    temp.append(" CutElectron");
    this->Cut_dE_E_TAPS_Electron = temp;
}

void ConfigFile::setSortRawNParticlesTotal(std::string sign, std::string str)
{
    this->SortRawNParticlesTotal = str + sign;
}

void ConfigFile::setSortRawNParticlesCB(std::string sign, std::string str)
{
    this->SortRawNParticlesCB = str + sign ;
}

void ConfigFile::setSortRawNParticlesTAPS(std::string sign, std::string str)
{
    this->SortRawNParticlesTAPS = str + sign;
}

void ConfigFile::setSortRawEnergyCB(std::string str)
{
    this->SortRawCBEnergySum = str + "+";
}

void ConfigFile::setCut_IM_Width_Pi0(std::string str)
{
    this->Cut_IM_Width_Pi0 = str;
}

void ConfigFile::setCut_IM_Width_Eta(std::string str)
{
    this->Cut_IM_Width_Eta = str;
}

void ConfigFile::setCut_IM_Width_Eta_Prime(std::string str)
{
    this->Cut_IM_Width_Eta_Prime = str;
}

void ConfigFile::setTimeCutMinApparatus1(std::string s)
{
    this->TimeCutMinApparatus1 = s;
}

void ConfigFile::setTimeCutMaxApparatus1(std::string s)
{
    this->TimeCutMaxApparatus1 = s;
}

void ConfigFile::setTimeCutMinApparatus2(std::string s)
{
    this->TimeCutMinApparatus2 = s;
}

void ConfigFile::setTimeCutMaxApparatus2(std::string s)
{
    this->TimeCutMaxApparatus2 = s;
}

std::string ConfigFile::getParticleReconstruction()
{
    return this->ParticleReconstruction;
}

std::string ConfigFile::getChargedParticleReconstruction()
{
    return this->ChargedParticleReconstruction;
}

std::string ConfigFile::getMesonReconstruction()
{
    return this->MesonReconstruction;
}

std::string ConfigFile::getCut_dE_E_CB_Proton()
{
    if (this->Cut_dE_E_CB_Proton == nullValue)
        return "No file selected.";
    return this->Cut_dE_E_CB_Proton;
}

std::string ConfigFile::getCut_dE_E_CB_Pion()
{
    if (this->Cut_dE_E_CB_Pion == nullValue)
        return "No file selected.";
    return this->Cut_dE_E_CB_Pion;
}

std::string ConfigFile::getCut_dE_E_CB_Electron()
{
    if (this->Cut_dE_E_CB_Electron == nullValue)
        return "No file selected.";
    return this->Cut_dE_E_CB_Electron;
}

std::string ConfigFile::getCut_dE_E_Taps_Proton()
{
    if (this->Cut_dE_E_TAPS_Proton == nullValue)
        return "No file selected.";
    return this->Cut_dE_E_TAPS_Proton;
}

std::string ConfigFile::getCut_dE_E_Taps_Pion()
{
    if (this->Cut_dE_E_TAPS_Pion == nullValue)
        return "No file selected.";
    return this->Cut_dE_E_TAPS_Pion;
}

std::string ConfigFile::getCut_dE_E_Taps_Electron()
{
    if (this->Cut_dE_E_TAPS_Electron == nullValue)
        return "No file selected.";
    return this->Cut_dE_E_TAPS_Electron;
}

std::string ConfigFile::getSortRawNParticlesTotal()
{
    return this->SortRawNParticlesTotal;
}

std::string ConfigFile::getSortRawNParticlesCB()
{
    return this->SortRawNParticlesCB;
}

std::string ConfigFile::getSortRawNParticlesTAPS()
{
    return this->SortRawNParticlesTAPS;
}

std::string ConfigFile::getSortRawEnergyCB()
{
    return this->SortRawCBEnergySum;
}

std::string ConfigFile::getCut_IM_Width_Pi0()
{
    return this->Cut_IM_Width_Pi0;
}

std::string ConfigFile::getCut_IM_Width_Eta()
{
    return this->Cut_IM_Width_Eta;
}

std::string ConfigFile::getCut_IM_Width_Eta_Prime()
{
    return this->Cut_IM_Width_Eta_Prime;
}

std::string ConfigFile::getTimeCutMinApparatus1()
{
    return this->TimeCutMinApparatus1;
}

std::string ConfigFile::getTimeCutMaxApparatus1()
{
    return this->TimeCutMaxApparatus1;
}

std::string ConfigFile::getTimeCutMinApparatus2()
{
    return this->TimeCutMinApparatus2;
}

std::string ConfigFile::getTimeCutMaxApparatus2()
{
    return this->TimeCutMaxApparatus2;
}
