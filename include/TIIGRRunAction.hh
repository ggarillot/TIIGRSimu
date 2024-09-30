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
// $Id: TIIGRRunAction.hh 99560 2016-09-27 07:03:29Z gcosmo $
//
/// \file TIIGRRunAction.hh
/// \brief Definition of the TIIGRRunAction class

#include <G4UserRunAction.hh>

#include "RootWriter.hh"
#include "TIIGREventAction.hh"

class G4Run;
class TIIGREventAction;
class G4GenericMessenger;

class TIIGRRunAction : public G4UserRunAction
{
  public:
    TIIGRRunAction();
    virtual ~TIIGRRunAction();

    // virtual G4Run* GenerateRun();
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    RootWriter* getRootWriter() const { return rootWriter.get(); }

    void DefineCommands();

  protected:
    G4GenericMessenger* fMessenger = nullptr;

    G4String outputFileName{"output.root"};

    std::unique_ptr<RootWriter> rootWriter = nullptr;

    std::chrono::steady_clock::time_point fBeginTime{};

    std::thread fPrintingThread{};
};
