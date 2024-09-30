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
// $Id: TIIGREventAction.cc 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file TIIGREventAction.cc
/// \brief Implementation of the TIIGREventAction class

#include "TIIGREventAction.hh"
#include "RootWriter.hh"
#include "TIIGRSteppingAction.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4Event.hh>
#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>

#include <G4UserRunAction.hh>
#include <G4ios.hh>

std::atomic<G4int> TIIGREventAction::fNEventsProcessed = 0;

TIIGREventAction::TIIGREventAction(TIIGRTrackingAction* trackingAction, RootWriter* rootWriter)
    : G4UserEventAction()
    , fTrackingAction(trackingAction)
    , fRootWriter(rootWriter)
{
    DefineCommands();
}

TIIGREventAction::~TIIGREventAction() {}

void TIIGREventAction::BeginOfEventAction(const G4Event* event)
{
    // G4cout << "event begin" << G4endl;
    fRootWriter->setEventNumber(event->GetEventID());
}

void TIIGREventAction::EndOfEventAction(const G4Event* event)
{
    fRootWriter->fillTree();

    // if (trackingAction->doPrintParticleMemoryMap())
    fTrackingAction->printParticleMemory();

    fTrackingAction->reset();

    // TODO
    fRootWriter->setEdep(edep0, edep1, edep2);

    // G4cout << edep0 / CLHEP::keV << " keV " << edep1 / CLHEP::keV << " keV " << edep2 / CLHEP::keV << " keV" <<
    // G4endl;
    edep0 = 0;
    edep1 = 0;
    edep2 = 0;

    fNEventsProcessed++;

    // G4cout << "event end" << G4endl;
}

void TIIGREventAction::addEdep(G4double edep, G4int detID)
{
    if (detID == 0)
        edep0 += edep;
    else if (detID == 1)
        edep1 += edep;
    else if (detID == 2)
        edep2 += edep;
}

void TIIGREventAction::DefineCommands()
{
    // Define detector construction command directory using generic messenger class
    fMessenger = new G4GenericMessenger(this, "/TIIGR/output/", "TIIGR control");
}
