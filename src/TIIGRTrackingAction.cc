#include "TIIGRTrackingAction.hh"

#include "ParticleMemory.hh"
#include "RootWriter.hh"
#include "TrackInformation.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ParticleDefinition.hh>
#include <G4ProcessType.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>
#include <G4ios.hh>

TIIGRTrackingAction::TIIGRTrackingAction(RootWriter* rootWriter)
    : rootWriter(rootWriter)
{
}

void TIIGRTrackingAction::PreUserTrackingAction(const G4Track* track)
{
    const auto trackID = track->GetTrackID();
    const auto parentID = track->GetParentID();
    const auto particleDefinition = track->GetParticleDefinition();
    const auto initialEnergy = track->GetKineticEnergy();

    const auto initialPosition = track->GetPosition();
    const auto initialTime = track->GetGlobalTime();

    const auto creatorProcess = track->GetCreatorProcess();

    auto particleMemory = ParticleMemory{};
    particleMemory.initialPosition = initialPosition;
    particleMemory.initialEnergy = initialEnergy;
    particleMemory.particleDefinition = particleDefinition;
    particleMemory.initialTime = initialTime;

    if (creatorProcess)
        particleMemory.creatorProcessName = creatorProcess->GetProcessName();

    childParentRelationMap.insert({trackID, parentID});
    parentChildrenRelationMap.insert({trackID, {}});

    if (parentID > 0)
        parentChildrenRelationMap.at(parentID).insert(trackID);

    particleMemoryMap.insert({trackID, particleMemory});

    const auto parentParticleDefinition = trackID == 1 ? nullptr : particleMemoryMap.at(parentID).particleDefinition;

    auto trackInfo = new TrackInformation(initialPosition, parentParticleDefinition, initialEnergy);

    const auto volumeName = track->GetVolume()->GetName();
    if (volumeName == "Body")
        trackInfo->doComeFromBody = true;

    track->SetUserInformation(trackInfo);
}

void TIIGRTrackingAction::PostUserTrackingAction(const G4Track* track)
{
    const auto trackID = track->GetTrackID();

    particleMemoryMap.at(trackID).finalEnergy = track->GetKineticEnergy();
    particleMemoryMap.at(trackID).finalPosition = track->GetPosition();
    particleMemoryMap.at(trackID).finalTime = track->GetGlobalTime();
}

void TIIGRTrackingAction::printParticleMemory() const
{
    // for (const auto& [particleID, particleMemory] : particleMemoryMap)
    // {
    //     // if (particleMemory.initialPosition.z() < 0 && particleID != 1)
    //     // continue;
    //     G4cout << particleID << " : " << particleMemory.particleDefinition->GetPDGEncoding() << " - "
    //            << particleMemory.particleDefinition->GetParticleName() << " : "
    //            << particleMemory.initialEnergy / CLHEP::keV << " keV, " << particleMemory.initialPosition / CLHEP::mm
    //            << " mm " << particleMemory.initialTime / CLHEP::ns << " ns -> "
    //            << particleMemory.finalEnergy / CLHEP::keV << " keV, " << particleMemory.finalPosition / CLHEP::mm
    //            << " mm " << particleMemory.finalTime / CLHEP::ns
    //            << " ns ->  parent : " << childParentRelationMap.at(particleID) << "   "
    //            << particleMemory.creatorProcessName << "\n";
    // }
    // G4cout << G4endl;
}

void TIIGRTrackingAction::reset()
{
    // for (const auto& [trackID, particleMemory] : particleMemoryMap)
    // {
    //     const auto& particleDef = particleMemory.particleDefinition;
    //     if (particleDef->GetPDGEncoding() == -11)
    //     {
    //         const auto& childSet = parentChildrenRelationMap.at(trackID);
    //         auto        parentID = childParentRelationMap.at(trackID);

    //         std::vector<G4int> parentVec{};

    //         while (parentID != 0)
    //         {
    //             const auto& parentPDGID = particleMemoryMap.at(parentID).particleDefinition->GetPDGEncoding();
    //             parentVec.push_back(parentPDGID);
    //             parentID = childParentRelationMap.at(parentID);
    //         }

    //         for (auto it = parentVec.rbegin(); it != parentVec.rend(); it++)
    //             G4cout << *it << " -> ";

    //         for (const auto& childID : childSet)
    //         {
    //             const auto& childParticleMemory = particleMemoryMap.at(childID);
    //             const auto& childPDGEncoding = childParticleMemory.particleDefinition->GetPDGEncoding();
    //             G4cout << childPDGEncoding << " - " << childParticleMemory.initialEnergy / CLHEP::MeV << ",";
    //         }
    //         G4cout << G4endl;
    //     }
    // }

    childParentRelationMap.clear();
    parentChildrenRelationMap.clear();
    particleMemoryMap.clear();
    printParticleMemoryMap = false;
}
