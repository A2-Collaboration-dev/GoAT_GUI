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
    std::string SortNParticlesSign;

    std::string pi0Sign;
    std::string pi0Number;
    std::string pi0AMin;
    std::string pi0AMax;


    /* Cuts */
    std::string TimeCutMinApparatus1;
    std::string TimeCutMaxApparatus1;
    std::string TimeCutMinApparatus2;
    std::string TimeCutMaxApparatus2;




public:
    static const std::string nullValue;
    static const std::vector<std::string> TextFields;

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

    /* Meson reconstruction */
    void setCut_IM_Width_Pi0(std::string str);
    void setCut_IM_Width_Eta(std::string str);
    void setCut_IM_Width_Eta_Prime(std::string str);

    /* Post Reconstruction */
    void setSortNParticles(std::string, std::string);
    void setSortNParticlesSign(std::string);

    void setPSpi0Sign(std::string);
    void setPSpi0Number(std::string, std::string);
    void setPSpi0AMin(std::string);
    void setPSpi0AMax(std::string);

    /* Cuts */
    void setTimeCutMinApparatus1(std::string);
    void setTimeCutMaxApparatus1(std::string);
    void setTimeCutMinApparatus2(std::string);
    void setTimeCutMaxApparatus2(std::string);

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

    /* Meson reconstruction */
    std::string getCut_IM_Width_Pi0();
    std::string getCut_IM_Width_Eta();
    std::string getCut_IM_Width_Eta_Prime();

    /* Cuts */
    std::string getTimeCutMinApparatus1();
    std::string getTimeCutMaxApparatus1();
    std::string getTimeCutMinApparatus2();
    std::string getTimeCutMaxApparatus2();

    /* Post Reconstruction */

    std::string getSortNParticles();
    std::string getSortNParticlesSign();

    std::string getPSpi0Sign();
    std::string getPSpi0Number();
    std::string getPSpi0AMin();
    std::string getPSpi0AMax();

    void loadGoATConfigFile(const char* config_file);
    void loadGoATConfigFile(const std::string config_file);

    void writeGoATConfigFile(const char* outfile);
    void writeGoATConfigFile(const std::string outfile);
};



#endif // CONFIGFILE_H
