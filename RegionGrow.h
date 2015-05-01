#ifndef REGIONGROW_H
#define REGIONGROW_H
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv/cv.h"

class CRegionGrow
{
public:
	IplImage* in;
	CvPoint seed;
	IplImage* out;

	float intensity_lower_threshold;
	float intensity_upper_threshold;
	float diff_threshold;

	CRegionGrow(IplImage* src, CvPoint p, IplImage* mask,
		float vmin = 100, float vmax = 200, float difft= 4, float dist = 60);
	~CRegionGrow();

	void setSeed(CvPoint p) {seed = p;}
	void setThrehold( float vmin, float vmax) {intensity_lower_threshold = vmin, intensity_upper_threshold = vmax;}
	void getInput(IplImage* src) { in = src;}

	void doRegionGrow();
	//void Initialization(IplImage* src, CvPoint p, IplImage* mask, float vmin, float vmax, float difft= 10, float dist = 60);
	//void releaseMem();


private:
	int count;
	float total[3];
	float mean[3];
	bool ** visit;

	int ydim, xdim, yseed, xseed;
	int region_size_threhold, i, j;
	int rgb[3];
	float diff;
	float** dist_map;
	float dist_threshold;

	void seedGrow(int x, int y);
	void calDistMap(int x, int y);
	bool checkPixel(int x, int y);
};


#endif
