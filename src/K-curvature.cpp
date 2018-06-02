#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

class image
{
public:
	int numRows;
	int numCols;
	int minVal;
	int maxVal;
	int** img;

	image(int row, int col, int min, int max)
	{
		numRows = row;
		numCols = col;
		minVal = min;
		maxVal = max;
		img = new int*[numRows];
		for(int i = 0; i < numRows; i++)
		{
			img[i] = new int[numCols];
		}

		for(int i = 0; i < row; i++)
		{
			for(int j = 0; j < col; j++)
			{
				img[i][j] = 0;
			}
		}
	}

	~image()
	{
		for(int i = 0; i < numRows; ++i)
		{
			delete[] img[i];
		}
		delete[]img;
	}

	void plotPt2Img(int x, int y, int corner)
	{
		img[x][y] = corner;
	}

	void prettyPrint(ofstream &outFile)
	{
		for(int i = 0; i < numRows; i++)
		{
			for(int j = 0; j < numCols; j++)
			{
				if(img[i][j] == 0)
				{
					outFile << " ";
				}
				else
				{
					outFile << img[i][j];
				}
			}
			outFile << endl;
		}
	}

};

class boundaryPt
{
public:
	friend class kCurvature;
	int x;
	int y;
	double curvature;
	int localMax;
	int corner;

	boundaryPt()
	{
		x = 0;
		y = 0;
		curvature = 0.0;
		localMax = 0;
		corner = 0;
	}

	boundaryPt(int row, int col, double curv, int max, int corn)
	{
		x = row;
		y = col;
		curvature = curv;
		localMax = max;
		corner = corn;
	}

	void printPt(ofstream &outFile)
	{
		outFile << x << " " << y << " " << curvature << endl;
	}
};

class kCurvature
{
public:
	int K;
	int numPts;
	boundaryPt* PtAry;
	int beginIndex;
	int Q;
	int P;
	int R;

	kCurvature()
	{
		numPts = 0;
		K = 0;
		beginIndex = 0;
		Q = 0;
		P = K;
		R = 2 * K;

		PtAry = new boundaryPt[numPts];
	}

	~kCurvature()
	{
		delete[] PtAry;
	}

	void countPts(ifstream &inFile)
	{
		int x = 0, y = 0;
		while(!inFile.eof())
		{
			inFile >> x >> y;
			numPts++;
		}
		inFile.close();
	}

	void storePt(int x, int y, int index)
	{
		PtAry[index].x = x;
		PtAry[index].y = y;
	}

	double computeCurvature(int Q, int P, int R)
	{
		double slop1 = ((double)PtAry[Q].y - (double)PtAry[P].y) / (((double)PtAry[Q].x - (double)PtAry[P].x) + 0.0001);
		// ( (c1-c2) / (r1 - r2) + 0.0001 )
		double slop2 =((double)PtAry[P].y-(double)PtAry[R].y)/(((double)PtAry[P].x-(double)PtAry[R].x)+0.0001);
		//  ( (c2 - c3) / ( r2 - r3 )  +0.0001 )
		double result = slop1 - slop2;
		return abs(result);
	}

	//a point, P(i) is a local maxima if p(i) is >= the curvatures of  its 1X5 neighbors:
	//p(i-2), p(i-1), p(i+1), p(i+2). The method  returns 1 if yes, returns 0 if not.

	int computeLocalMaxima(int i)
	{
		if(PtAry[i].curvature >= PtAry[(i - 2 + numPts) % numPts].curvature
           && PtAry[i].curvature >= PtAry[(i - 1 + numPts) % numPts].curvature
           && PtAry[i].curvature >= PtAry[(i + 1 + numPts) % numPts].curvature
           && PtAry[i].curvature >= PtAry[(i + 2 + numPts) % numPts].curvature)
		{
			return 1;
		}
		return 0;
	}

	void setCorner(int i)
	{
		if(PtAry[i].localMax == 1 && (PtAry[(i - 2 + numPts) % numPts].localMax == 0) && (PtAry[(i + 2 + numPts) % numPts].localMax == 0) )
		{
			PtAry[i].corner = 8;
		}
		else
		{
			PtAry[i].corner = 1;
		}
	}

	void printPtAry(ofstream &outFile)
	{
		for(int i = 0; i <= numPts - 1; i++)
		{
			outFile << i << " " << PtAry[i].x << " " << PtAry[i].y << endl;
		}
	}
};

int main(int argc, char **argv)
{
	/*-----------STEP 0----------*/
	ifstream inFile;
	ofstream outFile1;
	ofstream outFile2;
	ofstream outFile3;
	if (!inFile)
	{
		cout << "ERROR to open file";
		exit(1);
	}
	inFile.open(argv[1]);
	outFile1.open(argv[2]);
	outFile2.open(argv[3]);
	outFile3.open(argv[4]);

	int row, col, min, max;
	inFile >> row >> col >> min >> max;
	int label;
	inFile >> label;
	kCurvature *kcurv = new kCurvature();
	kcurv->countPts(inFile);
	outFile1 << row << " " << col << " " << min << " " << max << endl;
	outFile1 << label << endl;
	outFile1 << kcurv->numPts << endl;
	inFile.open(argv[1]);
	int K;
	cout << "Input K (length of neighborhood to be used in the K-curvature computation): "<< endl;
	cin >> K;
	kcurv->K = K;
	int index = 0;

	/*-----------STEP 1, 2, 3----------*/
	int x = 0, y = 0;
	inFile >> row >> col >> min >> max;
	inFile >> label;
	while(!inFile.eof())
	{
		inFile >> x >> y;
		kcurv->storePt(x, y, index);
		index++;
	}
	kcurv->printPtAry(outFile3);
	outFile3 << "------------------------------" << endl;

	/*-----------STEP 4----------*/
	kcurv->Q = 0;
	kcurv->P = kcurv->K;
	kcurv->R = 2 * kcurv->K;


	/*-----------STEP 5, 6, 7----------*/
	while(kcurv->P != (kcurv->K - 1))
	{
		index = kcurv->P;

		kcurv->PtAry[index].curvature = kcurv->computeCurvature(kcurv->Q, kcurv->P, kcurv->R);
		outFile3 << kcurv->Q << " " << kcurv->P << " " << kcurv->R << " " << index << " " << kcurv->PtAry[index].x << " " << kcurv->PtAry[index].y << " " << kcurv->PtAry[index].curvature << endl;
		kcurv->Q = (kcurv->Q + 1) % kcurv->numPts;
		kcurv->P = (kcurv->P + 1) % kcurv->numPts;
		kcurv->R = (kcurv->R + 1) % kcurv->numPts;
	}
	outFile3 << "------------------------------" << endl;

	/*-----------STEP 8----------*/

	for(int i = 0; i < kcurv->numPts; i++)
	{
		outFile3 << kcurv->PtAry[i].x << " " << kcurv->PtAry[i].y << " " << kcurv->PtAry[i].curvature << endl;
	}
	outFile3 << "------------------------------" << endl;

	/*-----------STEP 9, 10----------*/
	for(int i = 0; i < kcurv->numPts; i++)
	{
		kcurv->PtAry[i].localMax = kcurv->computeLocalMaxima(i);

	}

	for(int i = 0; i < kcurv->numPts; i++)
	{
		kcurv->setCorner(i);
	}

	/*-----------STEP 11----------*/
	for(int i = 0; i < kcurv->numPts; i++)
	{
		outFile1 << kcurv->PtAry[i].x << " " << kcurv->PtAry[i].y << " " << kcurv->PtAry[i].corner << endl;
	}


	/*-----------STEP 12----------*/
	image img(row, col, min, max);

	/*-----------STEP 13----------*/

	for(int i = 0; i < kcurv->numPts; i++)
	{
		img.plotPt2Img(kcurv->PtAry[i].x, kcurv->PtAry[i].y, kcurv->PtAry[i].corner);
	}


	/*-----------STEP 14----------*/
	img.prettyPrint(outFile2);


	/*-----------STEP 15----------*/
	outFile1.close();
	outFile2.close();
	outFile3.close();
	kcurv->~kCurvature();

	return 0;
}
