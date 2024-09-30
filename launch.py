import os


def launch(xo1, xo2, xo3, energy):

    fileContent = f"""
/TIIGR/det/det0 {xo1:.2f}
/TIIGR/det/det1 {xo2:.2f}
/TIIGR/det/det2 {xo3:.2f}

/run/numberOfThreads 8

/output/fileName {energy}_{xo1:.2f}_{xo2:.2f}_{xo3:.2f}.root

/run/initialize
/gun/energy {energy} keV

/run/beamOn  100000   
"""

    file = open("run.mac", "w")
    file.write(fileContent)
    file.close()

    os.system("./bin/TIIGR run.mac")

    os.system(f"mkdir -p ./results")
    os.system(f"mv {energy}_{xo1:.2f}_{xo2:.2f}_{xo3:.2f}.root ./results")


if __name__ == "__main__":

    energy = 440
    xo3 = 3.0

    for xo1 in [0.05 * i for i in range(2, 21)]:
        for xo2 in [0.05 * i for i in range(2, 21)]:
            print(f"{xo1} {xo2} {xo3}")
            launch(xo1, xo2, xo3, energy)
