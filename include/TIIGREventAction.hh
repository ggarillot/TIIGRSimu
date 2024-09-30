#pragma once

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
// $Id: TIIGREventAction.hh 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file TIIGREventAction.hh
/// \brief Definition of the TIIGREventAction class

#include "G4UserEventAction.hh"
#include "RootWriter.hh"

#include "TIIGRTrackingAction.hh"

#include <atomic>

class TIIGRRunAction;
class TIIGRTrackingAction;
class TIIGRSteppingAction;

class G4Event;
class G4GenericMessenger;

/// Event action class
///
class TIIGREventAction : public G4UserEventAction
{
  public:
    TIIGREventAction(TIIGRTrackingAction* trackingAction, RootWriter* rootWriter);
    virtual ~TIIGREventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    static G4int getNEventsProcessed() { return fNEventsProcessed; }
    static void  resetNEventsProcessed() { fNEventsProcessed = 0; }

    void addEdep(G4double edep, G4int detID);

  protected:
    void                DefineCommands();     // adding the method for defining macro commands
    G4GenericMessenger* fMessenger = nullptr; // adding messenger

    TIIGRTrackingAction* fTrackingAction = nullptr;
    RootWriter*          fRootWriter = nullptr;

    // TODO
    G4double edep0{};
    G4double edep1{};
    G4double edep2{};

    static std::atomic<G4int> fNEventsProcessed;
};
