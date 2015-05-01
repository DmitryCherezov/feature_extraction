#ifndef ENSEMBLE_H
#define ENSEMBLE_H

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv/cv.h"

const int PRIMARY_SEEDS_NUM = 6;
const int CORE_SEEDS_NUM = 10;


/*
 *
 * \author Hailing Zhou<hailing.zhou@deakin.edu.au> from Deakin University, CISR
 * \date Wed Aug 20, 2014
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 */

//#include "Global.h"

class CEnsemble
{
public:
	IplImage* in;
	CvPoint seed;
	IplImage* outmask;

	CEnsemble(IplImage* src, CvPoint p, IplImage* mask) ;
	~CEnsemble() {};

	void doEnsembleSegmentation();

private:
	void getIntersection(IplImage* results[CORE_SEEDS_NUM], IplImage* out);
	void merge2image(IplImage* src, IplImage*out);
	bool GetCentre(IplImage* mask, int& centrex, int& centrey);
	void checkResult(IplImage* result, int corex, int corey);
	bool findCoreRegion(IplImage* initial, IplImage* out, int& centrex, int& centrey);
	bool find6PrimarySeeds(IplImage* core, int seeds[PRIMARY_SEEDS_NUM][2]);
	void ClickandGrow(CvPoint seed, IplImage* out, float lower = 100, float upper = 200, float difft= 10, float dist = 60);

};

#endif
