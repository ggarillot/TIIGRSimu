import ROOT as R
import sys


def returnRatio(fileName):

    dataFrame = R.RDataFrame("tree", fileName)
    nEvents = dataFrame.Count()

    # last interaction is a photo
    dataFrame = dataFrame.Filter("!typeInt.empty()")
    dataFrame = dataFrame.Filter("typeInt.back()==2")
    nEvents1 = dataFrame.Count()

    # 2 or 3 interactions
    dataFrame = dataFrame.Filter("numInt.size()==2 || numInt.size()==3")
    nEvents2 = dataFrame.Count()

    # 2 interactions
    dataFrame2Int = dataFrame.Filter("typeInt.size()==2")
    nEvents3 = dataFrame2Int.Count()

    dataFrame2Int = dataFrame2Int.Filter("volumeID[1] > volumeID[0]")
    nEvents4 = dataFrame2Int.Count()

    # 3 interactions
    dataFrame3Int = dataFrame.Filter("typeInt.size()==3")
    nEvents5 = dataFrame3Int.Count()

    dataFrame3Int = dataFrame3Int.Filter(
        "volumeID[2] > volumeID[1] && volumeID[1] > volumeID[0]"
    )
    nEvents6 = dataFrame3Int.Count()

    return (nEvents.GetValue(), nEvents4.GetValue(), nEvents6.GetValue())


if __name__ == "__main__":

    energy = 218
    xo3 = 3.0

    histo2D = R.TH2D(
        "histo", ";X0 detector 1;X0 detector 2", 20, 0.1, 1.1, 20, 0.1, 1.1
    )
    histo2D.SetContour(99)
    histo2D.SetStats(0)

    for xo1 in [0.05 * i for i in range(2, 21)]:
        for xo2 in [0.05 * i for i in range(2, 21)]:
            fileName = f"results/{energy}_{xo1:.2f}_{xo2:.2f}_{xo3:.2f}.root"
            print(fileName)
            nEvents, nEvents2, nEvents3 = returnRatio(fileName)

            ratio = 1.0 * (nEvents2 + nEvents3) / nEvents

            histo2D.Fill(xo1, xo2, ratio)

    histo2D.Draw("col text")

    input("enter to exit")
