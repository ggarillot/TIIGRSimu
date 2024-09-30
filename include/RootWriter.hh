#pragma once

#include <G4AnalysisManager.hh>
#include <G4ThreeVector.hh>

class G4ParticleDefinition;
class G4Step;

class RootWriter
{
  public:
    RootWriter();
    virtual ~RootWriter() = default;

    void openRootFile(const G4String& name = "test.root");
    void closeRootFile();

    void setEventNumber(const G4int eventNumber);

    void setInitialGamma(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& mom, const G4double energy);

    void setEdep(const G4double edep0, const G4double edep1, const G4double edep2);

    void addInteraction(const G4int          trackID,
                        const G4ThreeVector& position,
                        const G4double       energy,
                        const G4double       time,
                        const G4int          type,
                        const G4int          numInt,
                        const G4int          volumeID);

    void fillTree();

  protected:
    void createHistograms();

  protected:
    G4AnalysisManager* analysisManager = nullptr;

    G4int id_tree{};

    G4int id_eventID{};

    G4int id_initPosX{};
    G4int id_initPosY{};
    G4int id_initPosZ{};
    G4int id_initMomX{};
    G4int id_initMomY{};
    G4int id_initMomZ{};
    G4int id_initEnergy{};

    G4int id_nInts{};

    G4int id_edepDet0{};
    G4int id_edepDet1{};
    G4int id_edepDet2{};

    std::vector<int>    idIntVec{};
    std::vector<double> xIntVec{};
    std::vector<double> yIntVec{};
    std::vector<double> zIntVec{};
    std::vector<double> eIntVec{};
    std::vector<double> tIntVec{};
    std::vector<int>    typeIntVec{};
    std::vector<int>    volumeIDVec{};
    std::vector<int>    numIntVec{};
};