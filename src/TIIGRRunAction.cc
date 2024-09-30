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
// $Id: TIIGRRunAction.cc 99560 2016-09-27 07:03:29Z gcosmo $
//
/// \file TIIGRRunAction.cc
/// \brief Implementation of the TIIGRRunAction class

#include "TIIGRRunAction.hh"
#include "TIIGREventAction.hh"

#include <chrono>

#include <G4GenericMessenger.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>

TIIGRRunAction::TIIGRRunAction()
    : G4UserRunAction()
{
    // add new units for dose
    const G4double milligray = 1.e-3 * CLHEP::gray;
    const G4double microgray = 1.e-6 * CLHEP::gray;
    const G4double nanogray = 1.e-9 * CLHEP::gray;
    const G4double picogray = 1.e-12 * CLHEP::gray;

    new G4UnitDefinition("milligray", "milliGy", "Dose", milligray);
    new G4UnitDefinition("microgray", "microGy", "Dose", microgray);
    new G4UnitDefinition("nanogray", "nanoGy", "Dose", nanogray);
    new G4UnitDefinition("picogray", "picoGy", "Dose", picogray);

    DefineCommands();

    rootWriter = std::make_unique<RootWriter>();
}

TIIGRRunAction::~TIIGRRunAction()
{
    delete fMessenger;
}

void TIIGRRunAction::BeginOfRunAction(const G4Run*)
{
    using namespace std::chrono_literals;

    if (IsMaster())
        TIIGREventAction::resetNEventsProcessed();

    // // inform the runManager to save random number seed
    // G4RunManager::GetRunManager()->SetRandomNumberStore(false);
    // // Normal randomization
    // G4RunManager::GetRunManager()->SetRandomNumberStore(true);
    // G4RunManager::GetRunManager()->SetRandomNumberStoreDir("random/");
    // // automatic (time-based) random seeds for each run
    // G4cout << "*******************" << G4endl;
    // G4cout << "*** AUTOSEED ON ***" << G4endl;
    // G4cout << "*******************" << G4endl;
    // long   seeds[2];
    // time_t systime = time(NULL);
    // seeds[0] = (long)systime;
    // seeds[1] = (long)(systime * G4UniformRand());
    // G4Random::setTheSeeds(seeds);
    // G4Random::showEngineStatus();

    rootWriter->openRootFile(outputFileName);

    if (IsMaster())
    {
        fBeginTime = std::chrono::steady_clock::now();

        const auto runManager = G4RunManager::GetRunManager();
        const auto nThreads = runManager->GetNumberOfThreads();
        const auto nEventsToBeProcessed = runManager->GetNumberOfEventsToBeProcessed();

        const auto threadLoop = [=](std::chrono::duration<double> interval) -> void
        {
            auto refTime = fBeginTime;
            G4cout << nEventsToBeProcessed << " events on " << nThreads << " threads" << G4endl;

            G4int nEventsProcessed = 0;
            G4int nEventsLastCheck = nEventsProcessed;

            while (nEventsProcessed < nEventsToBeProcessed)
            {
                const auto                          now = std::chrono::steady_clock::now();
                const std::chrono::duration<double> deltaTime = now - refTime;
                const std::chrono::duration<double> totalTime = now - fBeginTime;
                refTime = now;

                nEventsProcessed = TIIGREventAction::getNEventsProcessed();

                const auto nEventsDelta = nEventsProcessed - nEventsLastCheck;
                nEventsLastCheck = nEventsProcessed;

                const auto nEventsPerSec = nEventsDelta / deltaTime.count();
                const auto timeRemaining = 1s * (nEventsToBeProcessed - nEventsProcessed) / nEventsPerSec;

                G4cout << nEventsProcessed << "/" << nEventsToBeProcessed << " events \t total time "
                       << totalTime.count() << " s "
                       << "\t" << nEventsPerSec << " events/s \t time remaining : " << timeRemaining.count() << " s"
                       << G4endl;

                interval = 1s * std::min(interval.count(), (1.1 * timeRemaining).count());
                std::this_thread::sleep_for(interval);
            }
        };

        fPrintingThread = std::thread(threadLoop, 2s);
        return;
    }
}

void TIIGRRunAction::EndOfRunAction(const G4Run* run)
{
    rootWriter->closeRootFile();

    if (IsMaster())
    {
        fPrintingThread.join();
        const auto                          now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> totalTime = now - fBeginTime;

        const auto nEventsProcessed = TIIGREventAction::getNEventsProcessed();
        G4cout << nEventsProcessed << " events processed in " << totalTime.count()
               << " s : " << nEventsProcessed / totalTime.count() << " events/s" << G4endl;
    }
}

void TIIGRRunAction::DefineCommands()
{
    // Define detector construction command directory using generic messenger class
    fMessenger = new G4GenericMessenger(this, "/output/", "output commands");

    // Control thickness of detectors
    auto& outputNameCmd = fMessenger->DeclareProperty("fileName", outputFileName, "output root file name");
    outputNameCmd.SetParameterName("fileName", true);
    outputNameCmd.SetDefaultValue("output.root");
}
