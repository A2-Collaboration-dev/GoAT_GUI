#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <TROOT.h>



class ConfigFile
{

private:
    std::string configFile;

    std::string ParticleReconstruction;
    std::string ChargedParticleReconstruction;
    std::string MesonReconstruction;

    std::string Cut_dE_E_CB_Proton;
    std::string Cut_dE_E_CB_Pion;
    std::string Cut_dE_E_CB_Electron;
    std::string Cut_dE_E_TAPS_Proton;
    std::string Cut_dE_E_TAPS_Pion;
    std::string Cut_dE_E_TAPS_Electron;

    /* Meson reconstruction */
    std::string Cut_IM_Width_Pi0;
    std::string Cut_IM_Width_Eta;
    std::string Cut_IM_Width_Eta_Prime;

    /* Sorting preferenecs */
    bool SortRawNParticles;

    std::string SortRawNParticlesTotal;
    std::string SortRawNParticlesCB;
    std::string SortRawNParticlesTAPS;
    std::string SortRawCBEnergySum;


    /* Sort on reconstructed Particles */
    std::string SortNParticles;




public:
    static const std::string nullValue;

    ConfigFile();
    ConfigFile(std::string file);

    static std::string ReadConfig(const std::string& key_in, const Int_t instance, const Char_t* configname);
    static std::string ReadConfig(const std::string& key_in, std::string configname);

    void setParticleReconstruction(int val);
    void setChargedParticleReconstruction(int val);
    void setMesonReconstruction(int val);

    void setCut_dE_E_CB_Proton(std::string str);
    void setCut_dE_E_CB_Pion(std::string str);
    void setCut_dE_E_CB_Electron(std::string str);

    void setCut_dE_E_Taps_Proton(std::string str);
    void setCut_dE_E_Taps_Pion(std::string str);
    void setCut_dE_E_Taps_Electron(std::string str);

    /* Pre sorting */
    void setSortRawNParticlesTotal(std::string sign, std::string str);
    void setSortRawNParticlesCB(std::string sign, std::string str);
    void setSortRawNParticlesTAPS(std::string sign, std::string str);
    void setSortRawEnergyCB(std::string str);

    std::string getParticleReconstruction();
    std::string getChargedParticleReconstruction();
    std::string getMesonReconstruction();

    std::string getCut_dE_E_CB_Proton();
    std::string getCut_dE_E_CB_Pion();
    std::string getCut_dE_E_CB_Electron();

    std::string getCut_dE_E_Taps_Proton();
    std::string getCut_dE_E_Taps_Pion();
    std::string getCut_dE_E_Taps_Electron();

    std::string getSortRawNParticlesTotal();
    std::string getSortRawNParticlesCB();
    std::string getSortRawNParticlesTAPS();
    std::string getSortRawEnergyCB();

    void loadGoATConfigFile(const char* config_file);
    void loadGoATConfigFile(const std::string config_file);

    void writeGoATConfigFile(const char* outfile);
    void writeGoATConfigFile(const std::string outfile);
};



#endif // CONFIGFILE_H
