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
// $Id: TIIGRDetectorConstruction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file TIIGRDetectorConstruction.cc
/// \brief Implementation of the TIIGRDetectorConstruction class

#include "TIIGRDetectorConstruction.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4Box.hh>
#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4PVParameterised.hh>
#include <G4PVPlacement.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <G4String.hh>
#include <G4Tubs.hh>
#include <G4ios.hh>
#include <geomdefs.hh>

TIIGRDetectorConstruction::TIIGRDetectorConstruction()
    : G4VUserDetectorConstruction()
{
    DefineCommands();
}

TIIGRDetectorConstruction::~TIIGRDetectorConstruction()
{
    delete fMessenger;
}

G4VPhysicalVolume* TIIGRDetectorConstruction::Construct()
{
    G4NistManager* nist = G4NistManager::Instance();

    auto aluminumElement = nist->FindOrBuildElement("Al");
    auto yttriumElement = nist->FindOrBuildElement("Y");
    auto oxygenElement = nist->FindOrBuildElement("O");
    auto lanthanumElement = nist->FindOrBuildElement("La");
    auto bromideElement = nist->FindOrBuildElement("Br");

    G4double densityYAG = 4.55 * CLHEP::g / CLHEP::cm3;
    auto     YAGMaterial = new G4Material("YAGScint", densityYAG, 3);
    YAGMaterial->AddElement(yttriumElement, 3);
    YAGMaterial->AddElement(aluminumElement, 5);
    YAGMaterial->AddElement(oxygenElement, 12);

    G4double densityLaBr = 5.06 * CLHEP::g / CLHEP::cm3;
    auto     LaBrMaterial = new G4Material("LaBr", densityLaBr, 2);
    LaBrMaterial->AddElement(lanthanumElement, 1);
    LaBrMaterial->AddElement(bromideElement, 3);

    auto BGOMaterial = nist->FindOrBuildMaterial("G4_BGO");

    const auto YAGRadLen = YAGMaterial->GetRadlen();
    const auto LaBrRadLen = LaBrMaterial->GetRadlen();
    const auto BGORadLen = BGOMaterial->GetRadlen();

    G4cout << "YAG X0: " << YAGRadLen / CLHEP::mm << " mm" << G4endl;
    G4cout << "LaBr X0: " << LaBrRadLen / CLHEP::mm << " mm" << G4endl;
    G4cout << "BGO X0: " << BGORadLen / CLHEP::mm << " mm" << G4endl;

    // G4cout << "YAG density: " << YAGMaterial->GetDensity() / (CLHEP::g / CLHEP::cm3) << " g/cm3" << G4endl;
    // G4cout << "LaBr density: " << LaBrMaterial->GetDensity() / (CLHEP::g / CLHEP::cm3) << " g/cm3" << G4endl;
    // G4cout << "BGO density: " << BGOMaterial->GetDensity() / (CLHEP::g / CLHEP::cm3) << " g/cm3" << G4endl;

    const auto detectorSpacing = 10 * CLHEP::cm;

    const auto camera0SensitiveThickness = fCamera0X0 * YAGRadLen;
    const auto camera1SensitiveThickness = fCamera1X0 * LaBrRadLen;
    const auto camera2SensitiveThickness = fCamera2X0 * BGORadLen;

    G4cout << "camera0SensitiveThickness: " << camera0SensitiveThickness / CLHEP::mm << " mm" << G4endl;
    G4cout << "camera1SensitiveThickness: " << camera1SensitiveThickness / CLHEP::mm << " mm" << G4endl;
    G4cout << "camera2SensitiveThickness: " << camera2SensitiveThickness / CLHEP::mm << " mm" << G4endl;

    const auto camera0TotalThickness = camera0SensitiveThickness + 2 * fShellThickness;
    const auto camera1TotalThickness = camera1SensitiveThickness + 2 * fShellThickness;
    const auto camera2TotalThickness = camera2SensitiveThickness + 2 * fShellThickness;

    const auto detector0InnerRadius = 30 * CLHEP::cm;
    const auto detector1InnerRadius =
        detector0InnerRadius + 0.5 * (camera0TotalThickness - camera1TotalThickness) + detectorSpacing;
    const auto detector2InnerRadius =
        detector1InnerRadius + 0.5 * (camera1TotalThickness - camera2TotalThickness) + detectorSpacing;

    auto det0description = DetectorDescription{detector0InnerRadius, camera0SensitiveThickness, YAGMaterial};
    auto det1description = DetectorDescription{detector1InnerRadius, camera1SensitiveThickness, LaBrMaterial};
    auto det2description = DetectorDescription{detector2InnerRadius, camera2SensitiveThickness, BGOMaterial};

    detectorDescriptionVec.push_back(det0description);
    detectorDescriptionVec.push_back(det1description);
    detectorDescriptionVec.push_back(det2description);

    // Option to switch on/off checking of volumes overlaps
    G4bool checkOverlaps = true;

    G4Material* worldMaterial = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* waterMaterial = nist->FindOrBuildMaterial("G4_WATER");
    G4Material* aluminumMaterial = nist->FindOrBuildMaterial("G4_Al");

    G4double world_sizeX = 3 * CLHEP::m;
    G4double world_sizeY = 3 * CLHEP::m;
    G4double world_sizeZ = 3 * CLHEP::m;

    auto solidWorld = new G4Box("World", // its name
                                0.5 * world_sizeX, 0.5 * world_sizeY,
                                0.5 * world_sizeZ); // its size

    auto logicWorld = new G4LogicalVolume(solidWorld,    // its solid
                                          worldMaterial, // its material
                                          "World");      // its name

    auto physWorld = new G4PVPlacement(nullptr,    // no rotation
                                       {},         // at (0,0,0)
                                       logicWorld, // its logical volume
                                       "World",    // its name
                                       nullptr,    // its mother  volume
                                       false,      // no boolean operation
                                       0,          // copy number
                                       false);     // checkOverlaps);        //overlaps checking

    // Compton camera volume
    const G4double cameraZlength = 150. * CLHEP::cm;
    const G4double startAngle = 0. * CLHEP::deg;
    const G4double spanningAngle = 360. * CLHEP::deg;

    for (auto iDetector = 0U; iDetector < detectorDescriptionVec.size(); ++iDetector)
    {
        const auto& detectorDescription = detectorDescriptionVec[iDetector];

        const auto innerRadius = detectorDescription.innerRadius;
        const auto innerRadiusDetector = detectorDescription.innerRadius + fShellThickness;
        const auto detectorThickness = detectorDescription.thickness;
        const auto outerRadiusDetector = innerRadiusDetector + detectorThickness;
        const auto outerRadius = outerRadiusDetector + fShellThickness;

        auto solidCamera = new G4Tubs("camera", innerRadiusDetector, outerRadiusDetector, 0.5 * cameraZlength,
                                      startAngle, spanningAngle);
        auto logicCamera = new G4LogicalVolume(solidCamera, detectorDescription.material, "camera");

        new G4PVPlacement(nullptr, {}, logicCamera, "camera", logicWorld, 0, iDetector, checkOverlaps);

        // aluminum shell
        auto solidInnerShell =
            new G4Tubs("innerShell", innerRadius, innerRadiusDetector, 0.5 * cameraZlength, startAngle, spanningAngle);
        auto solidOuterShell =
            new G4Tubs("outerShell", outerRadiusDetector, outerRadius, 0.5 * cameraZlength, startAngle, spanningAngle);

        auto logicInnerShell = new G4LogicalVolume(solidInnerShell, aluminumMaterial, "innerShell");
        auto logicOuterShell = new G4LogicalVolume(solidOuterShell, aluminumMaterial, "outerShell");

        new G4PVPlacement(nullptr, {}, logicInnerShell, "innerShell", logicWorld, 0, iDetector, checkOverlaps);
        new G4PVPlacement(nullptr, {}, logicOuterShell, "outerShell", logicWorld, 0, iDetector, checkOverlaps);
    }

    // place water block if human flag is true
    G4bool humanFlag = false;
    if (humanFlag)
    {
        // Lets approximate a human with a box
        const G4double humanXsize = 50 * CLHEP::cm;
        const G4double humanYsize = 15 * CLHEP::cm;
        const G4double humanZsize = 175 * CLHEP::cm;

        G4Box* solidHuman = new G4Box("Human", 0.5 * humanXsize, 0.5 * humanYsize, 0.5 * humanZsize);

        G4LogicalVolume* logicHuman = new G4LogicalVolume(solidHuman, waterMaterial, "Human");
        new G4PVPlacement(nullptr, {}, logicHuman, "Human", logicWorld, 0, 0, checkOverlaps);
    }

    return physWorld;
}

void TIIGRDetectorConstruction::DefineCommands()
{
    // Define detector construction command directory using generic messenger class
    fMessenger = new G4GenericMessenger(this, "/TIIGR/det/", "Detector geometry control");

    // Thrench heigh TrD can be now controlled
    auto& shellThicknessCmd =
        fMessenger->DeclarePropertyWithUnit("ShellThickness", "cm", fShellThickness, "Shell thickness");
    shellThicknessCmd.SetParameterName("ShellThickness", true);
    shellThicknessCmd.SetRange("ShellThickness >= 0.");
    shellThicknessCmd.SetDefaultValue(".1");

    // Control thickness of detectors
    auto& det0ThicknessCmd = fMessenger->DeclareProperty("det0", fCamera0X0, "Detector 0 thickness in X0");
    det0ThicknessCmd.SetParameterName("det1", true);
    det0ThicknessCmd.SetRange("det1>0.");
    det0ThicknessCmd.SetDefaultValue("1");

    auto& det1ThicknessCmd = fMessenger->DeclareProperty("det1", fCamera1X0, "Detector 1 thickness in X0");
    det1ThicknessCmd.SetParameterName("det1", true);
    det1ThicknessCmd.SetRange("det1>0.");
    det1ThicknessCmd.SetDefaultValue("1");

    auto& det2ThicknessCmd = fMessenger->DeclareProperty("det2", fCamera2X0, "Detector 2 thickness in X0");
    det2ThicknessCmd.SetParameterName("det2", true);
    det2ThicknessCmd.SetRange("det2>0.");
    det2ThicknessCmd.SetDefaultValue("1");
}
