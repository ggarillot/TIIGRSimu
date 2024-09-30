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
// $Id: TIIGRDetectorConstruction.hh 69565 2013-05-08 12:35:31Z gcosmo $
//
/// \file TIIGRDetectorConstruction.hh
/// \brief Definition of the TIIGRDetectorConstruction class

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4Material.hh>

#include <G4Types.hh>
#include <G4VUserDetectorConstruction.hh>

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;

struct DetectorDescription
{
    G4double    innerRadius{};
    G4double    thickness{};
    G4Material* material{};
};

class TIIGRDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    TIIGRDetectorConstruction();
    virtual ~TIIGRDetectorConstruction();

    virtual G4VPhysicalVolume* Construct() override;

  protected:
    void DefineCommands();

  protected:
    G4GenericMessenger* fMessenger = nullptr;

    std::vector<DetectorDescription> detectorDescriptionVec{};

    G4double fShellThickness = 0.1 * CLHEP::cm;

    G4double fCamera0X0 = 1;
    G4double fCamera1X0 = 1;
    G4double fCamera2X0 = 1;
};
