//*******************************************************************************************************
//*******************************************************************************************************
//***********************************                          ******************************************
//***********************************         protAlign        ******************************************
//***********************************                          ******************************************
//*******************************************************************************************************
//******  -Calculates best fit and RMSD of two pdbs and aligns the second to the first-  ****************
//*******************************************************************************************************


#include "PDBInterface.h"
#include "ensemble.h"

int main (int argc, char* argv[])
{
	if (argc !=3)
	{   cout << "protAlign <inFile1.pdb> <inFile2.pdb>" << endl;
		exit(1); }

	string infile1 = argv[1];
	PDBInterface* thePDB1 = new PDBInterface(infile1);
	ensemble* theEnsemble1 = thePDB1->getEnsemblePointer();
	molecule* pMol1 = theEnsemble1->getMoleculePointer(0);
	protein* _prot1 = static_cast<protein*>(pMol1);
	
	string infile2 = argv[2];
	PDBInterface* thePDB2 = new PDBInterface(infile2);
	ensemble* theEnsemble2 = thePDB2->getEnsemblePointer();
	molecule* pMol2 = theEnsemble2->getMoleculePointer(0);
	protein* _prot2 = static_cast<protein*>(pMol2);

    vector<dblVec> coord1;
    vector<dblVec> coord2;
    vector<dblVec> coord1part;
    vector<dblVec> coord2part;
    atomIterator theIter1(_prot1);
    atomIterator theIter2(_prot2);
    atom* pAtom;
    bool first = true;
    
    // Load backbone atoms into vector for fit and alignment
    for (;!(theIter1.last());theIter1++)
    {
       pAtom = theIter1.getAtomPointer(); 
       if (pAtom->getName() == "CA"){
            coord1.push_back(pAtom->getCoords());
       }
    }
    for (;!(theIter2.last());theIter2++)
    {
       pAtom = theIter2.getAtomPointer(); 
       if (pAtom->getName() == "CA"){
            coord2.push_back(pAtom->getCoords());
       }
    }
    int diff = 0;
    if(coord1.size() != coord2.size()){
		if (coord2.size() < coord1.size()){ diff = coord1.size()-coord2.size(); first = true;}
		else{diff = coord2.size()-coord1.size(); first = false;}
    }
    int maxsize, newsize, remove;
    if (first){maxsize = coord2.size();}else{maxsize = coord1.size();}
	double rotmat[9]; double centroid1[3]; double centroid2[3]; double rmsd = 0; int ierr = 0;
	int list1[maxsize]; int list2[maxsize]; int trials = 1; double bestRMSD= 999999.0; double bestRotMat[9];
	double newCoord1[maxsize*3]; double newCoord2[maxsize*3]; double newCoord3[maxsize*3];
	
	if (diff != 0){trials = 1;}
	for (int h = 0; h < trials; h++)
	{
		coord1part = coord1; coord2part = coord2;
		for (int i = 0; i < diff; i++){
			if (first){
				newsize = coord1part.size();
				remove = rand() % newsize;
				coord1part.erase(coord1part.begin() + remove);
			}
			else{
				newsize = coord2part.size();
				remove = rand() % newsize;
				coord2part.erase(coord2part.begin() + remove);
			}
		}
		for (int i=0; i<maxsize; i++)
		{	
			for (int j=0; j<3; j++)
			{
				if(first){
					newCoord1[ (i*3) + j] = coord1part[i][j];
					newCoord2[ (i*3) + j] = coord2part[i][j];
				}
				else{
					newCoord1[ (i*3) + j] = coord2part[i][j];
					newCoord2[ (i*3) + j] = coord1part[i][j];
				}
			}
			list1[i] = i+1;
			list2[i] = i+1;
		}
		
		// Calculate best fit of backbone atoms, rotation matrix and rmsd using fortran algorithm based on Machlachlan
		bestfit_(newCoord1, &maxsize, newCoord2, &maxsize, &maxsize, newCoord3, list1, list2, &rmsd, &ierr, rotmat, centroid1, centroid2);
		if (rmsd < bestRMSD){
			bestRMSD = rmsd;
			for (int j = 0; j < 9; j++){bestRotMat[j]=rotmat[j];}
		}
	}
	
	// Load rotation vector into rotation matrix
	dblMat rotMat(3,3,3);
    for (UInt i=0; i<3; i++)
    {	for (UInt j=0; j<3; j++)
		{
			rotMat[i][j] = bestRotMat[i*3 + j];
		}
    }
    
    // Transform second or smaller pdb onto first using rotation matrix and centroid translation
    
    if (!first){
		for (UInt i = 0; i < _prot2->getNumChains(); i++)
		{
			_prot2->translateChain(i,centroid1[0]-centroid2[0],centroid2[1]-centroid2[1],centroid1[2]-centroid2[2]);
			_prot2->transform(i,rotMat);
		}
		pdbWriter(_prot2,infile2);
	}
	else{
		for (UInt i = 0; i < _prot1->getNumChains(); i++)
		{
			_prot1->translateChain(i,centroid2[0]-centroid1[0],centroid2[1]-centroid1[1],centroid2[2]-centroid1[2]);
			_prot1->transform(i,rotMat);
		}
		pdbWriter(_prot1,infile1);
	}
    cout << "RMSD = " << bestRMSD << endl;
	return 0;
}
