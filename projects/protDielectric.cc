//*******************************************************************************************************
//*******************************************************************************************************
//*************************************                      ********************************************
//*************************************     protDielectric   ********************************************
//*************************************                      ********************************************
//*******************************************************************************************************
//******** -sidechain and backbone optimization with a burial-based scaling of electrostatics- **********
//*******************************************************************************************************

/////// Just specify a infile and outfile, it will optimize to a generally effective minimum.

//--Program setup----------------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <time.h>
#include <sstream>
#include "ensemble.h"
#include "PDBInterface.h"

int main (int argc, char* argv[])
{
	//--Running parameters
    if (argc !=2)
	{
        cout << "protDielectric <inFile.pdb>" << endl;
		exit(1);
	}
	enum aminoAcid {A,R,N,D,Dh,C,Cx,Cf,Q,E,Eh,Hd,He,Hp,I,L,K,M,F,P,O,S,T,W,Y,V,G,dA,dR,dN,dD,dDh,dC,dCx,dCf,dQ,dE,dEh,dHd,dHe,dHp,dI,dL,dK,dM,dF,dP,dO,dS,dT,dW,dY,dV,Csf,Sf4,Hca,Eoc,Oec,Saf,Hem,Cyn,Tp};
	string aminoAcidString[] = {"A","R","N","D","Dh","C","Cx","Cf","Q","E","Eh","Hd","He","Hp","I","L","K","M","F","P","O","S","T","W","Y","V","G","dA","dR","dN","dD","dDh","dC","dCx","dCf","dQ","dE","dEh","dHd","dHe","dHp","dI","dL","dK","dM","dF","dP","dO","dS","dT","dW","dY","dV","Csf","Sf4","Hca","Eoc","Oec","Saf","Hem","Cyn","Tp"};
	string infile = argv[1];
	PDBInterface* thePDB = new PDBInterface(infile);
	ensemble* theEnsemble = thePDB->getEnsemblePointer();
	molecule* pMol = theEnsemble->getMoleculePointer(0);
	protein* _prot = static_cast<protein*>(pMol);
	string outFile;

    _prot->protEnergy();
    cout << "chain" << " position" << " residue" << " dielectric" << " solvationEnergy" << endl;
    for (UInt i = 0; i < _prot->getNumChains(); i++)
    {
        for (UInt j = 0; j < _prot->getNumResidues(i); j++)
        {
            UInt restype = _prot->getTypeFromResNum(i,j);
            cout << i << " " << j+1 << " " << aminoAcidString[restype] << " " << _prot->getDielectric(i, j) << " " << _prot->getSolvationEnergy(i, j) << endl;
        }
    }

//--Print final energy and write a pdb file--------------------------------------------------------------
   outFile = infile;
   pdbWriter(_prot, outFile);
	return 0;
}

