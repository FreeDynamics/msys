import numpy as np
import msys
import pathlib


def test_assign_bo():
    prmtop = f"{str(pathlib.Path(__file__).parent)}/rna.ol3.prmtop"
    mol = msys.LoadPrmTop(prmtop)
    msys.AssignBondOrderAndFormalCharge(mol)
    print("succeed")


if __name__ == "__main__":
    test_assign_bo()