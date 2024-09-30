import ROOT as R
import sys


def returnCanvas(fileName, energy):

    dataFrame = R.RDataFrame("tree", fileName)
    nEvents = dataFrame.Count()

    canvas = R.TCanvas("c", "", 1600, 900)
    canvas.Divide(3, 3)

    for i in range(1, 10):
        pad = canvas.cd(i)
        pad.SetLeftMargin(0.1)
        pad.SetRightMargin(0.02)
        # pad.SetBottomMargin(0.05)

    for i in range(3):
        dataFrame = dataFrame.Define(f"eInt{i}", f"eInt[volumeID=={i}]")
        dataFrame = dataFrame.Define(f"typeInt{i}", f"typeInt[volumeID=={i}]")
        dataFrame = dataFrame.Define(f"nInt{i}", f"eInt{i}.size()")

        canvas.cd(i + 1)
        histo = dataFrame.Histo1D(
            ("", f"n Interactions detector {i}", 10, 0, 10), f"nInt{i}"
        )
        histo.SetStats(0)
        histo.DrawCopy()

        canvas.cd(i + 4)
        histo = dataFrame.Histo1D(
            ("", f"interaction type detector {i}", 2, 1, 3), f"typeInt{i}"
        )
        histo.SetStats(0)
        histo.GetYaxis().SetRangeUser(
            0, histo.GetBinContent(histo.GetMaximumBin()) * 1.05
        )
        histo.DrawCopy()

        canvas.cd(i + 7)
        histo = dataFrame.Histo1D(
            ("", f"energy detector {i};keV", int(energy * 0.5 + 10), 0, energy + 20),
            f"eInt{i}",
        )
        histo.SetStats(0)
        histo.DrawCopy()

    return canvas


if __name__ == "__main__":

    energy = 440
    xo1 = 0.2
    xo2 = 0.2
    xo3 = 3.0

    fileName = f"results/{energy}_{xo1:.2f}_{xo2:.2f}_{xo3:.2f}.root"
    print(fileName)
    canvas = returnCanvas(fileName, energy)

    canvas.Update()

    input("enter to exit")
