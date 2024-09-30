//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: TIIGRPrimaryGeneratorAction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file TIIGRPrimaryGeneratorAction.cc
/// \brief Implementation of the TIIGRPrimaryGeneratorAction class

#include "TIIGRPrimaryGeneratorAction.hh"

#include "RootWriter.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ParticleDefinition.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4RunManager.hh>
#include <G4ThreeVector.hh>
#include <Randomize.hh>

TIIGRPrimaryGeneratorAction::TIIGRPrimaryGeneratorAction(RootWriter* rootWriter)
    : G4VUserPrimaryGeneratorAction()
    , fRootWriter(rootWriter)
{
    fParticleGun = new G4ParticleGun(1);

    // default particle kinematic
    auto particleTable = G4ParticleTable::GetParticleTable();

    auto particle = particleTable->FindParticle("gamma");

    fParticleGun->SetParticleDefinition(particle);

    const auto particlePosition = G4ThreeVector{};
    fParticleGun->SetParticlePosition(particlePosition);

    const auto particleEnergy = 218 * CLHEP::keV;
    fParticleGun->SetParticleEnergy(particleEnergy);
}

TIIGRPrimaryGeneratorAction::~TIIGRPrimaryGeneratorAction()
{
    delete fParticleGun;
}

void TIIGRPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    G4double cosTheta = 2 * G4UniformRand() - 1;
    G4double phi = 2 * CLHEP::pi * G4UniformRand();
    G4double sinTheta = std::sqrt(1. - cosTheta * cosTheta);

    auto initMomentum = G4ThreeVector{sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta};

    fParticleGun->SetParticleMomentumDirection(initMomentum);

    fRootWriter->setInitialGamma(fParticleGun->GetParticlePosition(), initMomentum, fParticleGun->GetParticleEnergy());

    fParticleGun->GeneratePrimaryVertex(anEvent);
}
