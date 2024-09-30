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
// $Id: TIIGRSteppingAction.cc 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file TIIGRSteppingAction.cc
/// \brief Implementation of the TIIGRSteppingAction class

#include "TIIGRSteppingAction.hh"
#include "RootWriter.hh"
#include "TIIGREventAction.hh"
#include "TrackInformation.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4RunManager.hh>
#include <G4Step.hh>
#include <G4StepPoint.hh>
#include <G4ThreeVector.hh>
#include <G4Track.hh>
#include <G4Types.hh>
#include <G4VProcess.hh>
#include <G4ios.hh>

TIIGRSteppingAction::TIIGRSteppingAction(TIIGREventAction* eventAction, RootWriter* rootWriter)
    : G4UserSteppingAction()
    , fEventAction(eventAction)
    , fRootWriter(rootWriter)
{
}

void TIIGRSteppingAction::UserSteppingAction(const G4Step* step)
{
    const auto curTrack = step->GetTrack();
    const auto trackID = curTrack->GetTrackID();

    const auto preStepPoint = step->GetPreStepPoint();
    const auto postStepPoint = step->GetPostStepPoint();

    const auto volume = postStepPoint->GetTouchableHandle()->GetVolume();

    if (!volume) // particle exiting world
        return;

    // Searching for initial Gamma:

    if (volume->GetName() != "camera")
        return;

    // TODO
    const auto eDep = step->GetTotalEnergyDeposit();
    const auto detID = volume->GetCopyNo();
    fEventAction->addEdep(eDep, detID);

    if (curTrack->GetDefinition()->GetParticleName() == "gamma" && curTrack->GetTrackID() == 1)
    {
        const auto processName = postStepPoint->GetProcessDefinedStep()->GetProcessName();

        const auto volumeID = postStepPoint->GetTouchableHandle()->GetCopyNumber();
        const auto tDetHitTime = postStepPoint->GetGlobalTime();

        // const auto betaEnergy = step->GetDeltaEnergy() * (-1);

        const auto betaEnergy = preStepPoint->GetTotalEnergy() - postStepPoint->GetTotalEnergy();

        const G4ThreeVector position = postStepPoint->GetPosition();

        if (betaEnergy > 0)
        {
            G4int interactionType = 0;

            if (processName == "phot")
                interactionType = 2;
            else if (processName == "compt")
                interactionType = 1;

            if (interactionType > 0)
            {
                // const auto secondaryVec = step->GetSecondaryInCurrentStep();
                // for (const auto secondary : *secondaryVec)
                // {
                //     G4cout << "       " << secondary->GetTrackID() << ": "
                //            << secondary->GetParticleDefinition()->GetParticleName() << " "
                //            << secondary->GetKineticEnergy() / CLHEP::keV << " keV    "
                //            << secondary->GetPosition() / CLHEP::mm << " mm  " << secondary->GetGlobalTime() /
                //            CLHEP::ps
                //            << " ps  "
                //            << "\n";
                // }
                // G4cout << G4endl;

                auto& interactionNumber =
                    dynamic_cast<TrackInformation*>(curTrack->GetUserInformation())->numberOfInteractions;

                fRootWriter->addInteraction(trackID, position, betaEnergy, tDetHitTime, interactionType,
                                            interactionNumber, volumeID);

                interactionNumber++;
            }
            // else
            // {
            //     G4cout << processName << "  " << betaEnergy / CLHEP::keV << " keV" << G4endl;
            // }
        }

        //  G4cout<< "We found a gamma"<< G4endl;
        //  G4cout<< "Energy(poststep) is "<< step->GetPostStepPoint()->GetKineticEnergy()<< G4endl;
        //  G4cout<< "Energy(prestep) is "<< step->GetPreStepPoint()->GetKineticEnergy()<< G4endl;
        //  G4cout<< "dE "<< step->GetDeltaEnergy()<<G4endl;
        //  G4cout<< "Process name is "<<
        //  step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()<<G4endl; G4cout<< "and trackID is
        //  "<<  step->GetTrack()->GetTrackID() << G4endl;
    }
}
