/*
 *
 * \author Hailing Zhou<hailing.zhou@deakin.edu.au> from Deakin University, CISR
 * \date Mon Nov 10, 2014
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 */

#ifndef EXT3D_H
#define EXT3D_H

//#include "Ensemble.h"

#define FORWARD 1
#define BACKWARD -1

//#include "dirent.h"
//#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv/cv.h"
#include "DICOMin.h"

using namespace std;
using namespace cv;

class CEnsemble3dExt
{
public:
	CEnsemble3dExt();
	~CEnsemble3dExt();

	void LoadSlices(const char* in_folder_name);
	void InputSeed(int x, int y, int z) {seedx = x; seedy = y; seedz = z;}
	void Ensemble3Dvolume(const char* out_folder_name);

private:
	IplImage* aslice;
	IplImage* outmask;
	vector<string> slices_name;
	int seedx;
	int seedy;
	int seedz;
    DICOMin _dcm;
	bool UpdateSeed(int z, int fb);
	void SaveResult(const char* folder_name, int z);
	vector<Mat*> _temporal;
	void _init_temporal();
	void _free_temporal();
	void _saveToNiffty(vector<Mat*> _data, const char* _name );
};

#endif
