#include "RootWriter.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4AnalysisManager.hh>
#include <G4RunManager.hh>
#include <G4ios.hh>

RootWriter::RootWriter()
{
    analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(0);

    const auto runManager = G4RunManager::GetRunManager();
    const auto type = runManager->GetRunManagerType();

    if (type != G4RunManager::sequentialRM)
        analysisManager->SetNtupleMerging(true);
}

void RootWriter::openRootFile(const G4String& name)
{
    analysisManager->OpenFile(name);
    createHistograms();
}

void RootWriter::closeRootFile()
{
    analysisManager->Write();
    analysisManager->CloseFile();
}

void RootWriter::createHistograms()
{
    id_tree = analysisManager->CreateNtuple("tree", "tree");

    id_eventID = analysisManager->CreateNtupleIColumn(id_tree, "eventID");

    id_initPosX = analysisManager->CreateNtupleDColumn(id_tree, "initPosX");
    id_initPosY = analysisManager->CreateNtupleDColumn(id_tree, "initPosY");
    id_initPosZ = analysisManager->CreateNtupleDColumn(id_tree, "initPosZ");
    id_initMomX = analysisManager->CreateNtupleDColumn(id_tree, "initMomX");
    id_initMomY = analysisManager->CreateNtupleDColumn(id_tree, "initMomY");
    id_initMomZ = analysisManager->CreateNtupleDColumn(id_tree, "initMomZ");
    id_initEnergy = analysisManager->CreateNtupleDColumn(id_tree, "initEnergy");

    id_nInts = analysisManager->CreateNtupleIColumn(id_tree, "nInts");

    analysisManager->CreateNtupleIColumn(id_tree, "idInt", idIntVec);
    analysisManager->CreateNtupleDColumn(id_tree, "xInt", xIntVec);
    analysisManager->CreateNtupleDColumn(id_tree, "yInt", yIntVec);
    analysisManager->CreateNtupleDColumn(id_tree, "zInt", zIntVec);
    analysisManager->CreateNtupleDColumn(id_tree, "eInt", eIntVec);
    analysisManager->CreateNtupleDColumn(id_tree, "tInt", tIntVec);
    analysisManager->CreateNtupleIColumn(id_tree, "typeInt", typeIntVec);
    analysisManager->CreateNtupleIColumn(id_tree, "numInt", numIntVec);
    analysisManager->CreateNtupleIColumn(id_tree, "volumeID", volumeIDVec);

    id_edepDet0 = analysisManager->CreateNtupleDColumn(id_tree, "edep0");
    id_edepDet1 = analysisManager->CreateNtupleDColumn(id_tree, "edep1");
    id_edepDet2 = analysisManager->CreateNtupleDColumn(id_tree, "edep2");

    analysisManager->FinishNtuple(id_tree);
}

void RootWriter::setEventNumber(const G4int eventNumber)
{
    analysisManager->FillNtupleIColumn(id_tree, id_eventID, eventNumber);
}

void RootWriter::setInitialGamma(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& mom, const G4double energy)
{
    analysisManager->FillNtupleDColumn(id_tree, id_initPosX, pos.x() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(id_tree, id_initPosY, pos.y() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(id_tree, id_initPosZ, pos.z() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(id_tree, id_initMomX, mom.x());
    analysisManager->FillNtupleDColumn(id_tree, id_initMomY, mom.y());
    analysisManager->FillNtupleDColumn(id_tree, id_initMomZ, mom.z());
    analysisManager->FillNtupleDColumn(id_tree, id_initEnergy, energy / CLHEP::keV);
}

void RootWriter::setEdep(const G4double edep0, const G4double edep1, const G4double edep2)
{
    analysisManager->FillNtupleDColumn(id_tree, id_edepDet0, edep0 / CLHEP::keV);
    analysisManager->FillNtupleDColumn(id_tree, id_edepDet1, edep1 / CLHEP::keV);
    analysisManager->FillNtupleDColumn(id_tree, id_edepDet2, edep2 / CLHEP::keV);
}

void RootWriter::addInteraction(const G4int          trackID,
                                const G4ThreeVector& position,
                                const G4double       energy,
                                const G4double       time,
                                const G4int          type,
                                const G4int          numInt,
                                const G4int          volumeID)
{
    idIntVec.push_back(trackID);
    xIntVec.push_back(position.x() / CLHEP::mm);
    yIntVec.push_back(position.y() / CLHEP::mm);
    zIntVec.push_back(position.z() / CLHEP::mm);
    eIntVec.push_back(energy / CLHEP::keV);
    tIntVec.push_back(time / CLHEP::ns);
    typeIntVec.push_back(type);
    numIntVec.push_back(numInt);
    volumeIDVec.push_back(volumeID);
}

void RootWriter::fillTree()
{
    analysisManager->FillNtupleIColumn(id_tree, id_nInts, xIntVec.size());

    analysisManager->AddNtupleRow(id_tree);

    idIntVec.clear();
    xIntVec.clear();
    yIntVec.clear();
    zIntVec.clear();
    eIntVec.clear();
    tIntVec.clear();
    typeIntVec.clear();
    numIntVec.clear();
    volumeIDVec.clear();
}
