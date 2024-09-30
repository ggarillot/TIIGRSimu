#pragma once

#include <G4LorentzVector.hh>
#include <G4ParticleDefinition.hh>
#include <G4String.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>

struct ParticleMemory
{
    G4ThreeVector initialPosition{};
    G4ThreeVector finalPosition{};

    G4double initialTime{};
    G4double finalTime{};

    G4double initialEnergy{};
    G4double finalEnergy{};

    G4String creatorProcessName{};

    const G4ParticleDefinition* particleDefinition{};
};