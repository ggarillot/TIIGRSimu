import ROOT as R
import sys


def returnCanvas(fileName, energy):

    dataFrame = R.RDataFrame("tree", fileName)

    # last interaction is a photo
    dataFrame = dataFrame.Filter("!typeInt.empty()")
    dataFrame = dataFrame.Filter("typeInt.back()==2")

    # 2 or 3 interactions
    dataFrame = dataFrame.Filter("numInt.size()==2 || numInt.size()==3")
    dataFrame = dataFrame.Filter("volumeID[1] > volumeID[0]")

    # 2 interactions
    dataFrame2Int = dataFrame.Filter("typeInt.size()==2")

    # 3 interactions
    dataFrame3Int = dataFrame.Filter("typeInt.size()==3")
    dataFrame3Int = dataFrame3Int.Filter("volumeID[2] > volumeID[1]")

    canvas2Int = R.TCanvas("c2", "2 interactions", 1600, 900)
    canvas2Int.Divide(3, 3)

    for i in range(1, 10):
        pad = canvas2Int.cd(i)
        pad.SetLeftMargin(0.1)
        pad.SetRightMargin(0.02)
        # pad.SetBottomMargin(0.05)

    canvas3Int = R.TCanvas("c3", "3 interactions", 1600, 900)
    canvas3Int.Divide(3, 3)

    for i in range(1, 10):
        pad = canvas3Int.cd(i)
        pad.SetLeftMargin(0.1)
        pad.SetRightMargin(0.02)
        # pad.SetBottomMargin(0.05)

    for i in range(3):
        dataFrame2Int = dataFrame2Int.Define(f"eInt{i}", f"eInt[volumeID=={i}]")
        dataFrame2Int = dataFrame2Int.Define(f"typeInt{i}", f"typeInt[volumeID=={i}]")
        dataFrame2Int = dataFrame2Int.Define(f"nInt{i}", f"eInt{i}.size()")

        dataFrame3Int = dataFrame3Int.Define(f"eInt{i}", f"eInt[volumeID=={i}]")
        dataFrame3Int = dataFrame3Int.Define(f"typeInt{i}", f"typeInt[volumeID=={i}]")
        dataFrame3Int = dataFrame3Int.Define(f"nInt{i}", f"eInt{i}.size()")

        canvas2Int.cd(i + 1)
        histo = dataFrame2Int.Histo1D(
            ("", f"n Interactions detector {i}", 10, 0, 10), f"nInt{i}"
        )
        histo.SetStats(0)
        histo.DrawCopy()

        canvas3Int.cd(i + 1)
        histo = dataFrame3Int.Histo1D(
            ("", f"n Interactions detector {i}", 10, 0, 10), f"nInt{i}"
        )
        histo.SetStats(0)
        histo.DrawCopy()

        canvas2Int.cd(i + 4)
        histo = dataFrame2Int.Histo1D(
            ("", f"interaction type detector {i}", 2, 1, 3), f"typeInt{i}"
        )
        histo.SetStats(0)
        histo.GetYaxis().SetRangeUser(
            0, histo.GetBinContent(histo.GetMaximumBin()) * 1.05
        )
        histo.DrawCopy()
        canvas3Int.cd(i + 4)
        histo = dataFrame3Int.Histo1D(
            ("", f"interaction type detector {i}", 2, 1, 3), f"typeInt{i}"
        )
        histo.SetStats(0)
        histo.GetYaxis().SetRangeUser(
            0, histo.GetBinContent(histo.GetMaximumBin()) * 1.05
        )
        histo.DrawCopy()

        canvas2Int.cd(i + 7)
        histo = dataFrame2Int.Histo1D(
            ("", f"energy detector {i};keV", int(energy * 0.5 + 10), 0, energy + 20),
            f"eInt{i}",
        )
        histo.SetStats(0)
        histo.DrawCopy()

        canvas3Int.cd(i + 7)
        histo = dataFrame3Int.Histo1D(
            ("", f"energy detector {i};keV", int(energy * 0.5 + 10), 0, energy + 20),
            f"eInt{i}",
        )
        histo.SetStats(0)
        histo.DrawCopy()

    return canvas2Int, canvas3Int


if __name__ == "__main__":

    energy = 440
    xo1 = 0.2
    xo2 = 0.2
    xo3 = 3.0

    fileName = f"results/{energy}_{xo1:.2f}_{xo2:.2f}_{xo3:.2f}.root"
    print(fileName)
    canvas2, canvas3 = returnCanvas(fileName, energy)

    canvas2.Update()
    canvas3.Update()

    input("enter to exit")
