/*
 *
 * \author Hailing Zhou<hailing.zhou@deakin.edu.au> from Deakin University, CISR
 * \date Wed Aug 20, 2014
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 */
//#include "stdafx.h"
#include "RegionGrow.h"
#include <iostream>

using namespace std;

//float** dist_map;  //////It may cause the stack memory problem if I put dist_map in the class definition
//float dist_threshold;  //////It may cause the stack memory problem if I put dist_map in the class definition
CRegionGrow::CRegionGrow(IplImage* src, CvPoint p, IplImage* mask, float vmin, float vmax, float diff_t, float dist_t)
{
	intensity_lower_threshold = vmin;
	intensity_upper_threshold = vmax;
	diff_threshold = diff_t;
	dist_threshold = dist_t;

	seed = p;
	in = src;
	out = mask;

	if(in)
	{
		ydim = in->height, xdim = in->width, yseed = seed.y, xseed = seed.x;
		region_size_threhold = (ydim*xdim)/10; //depend on the size of area of interest.

		visit = new bool*[ydim];
		dist_map = new float*[ydim];
		for(i=0; i<ydim; i++)
		{
			visit[i] = new bool[xdim];
			dist_map[i] = new float[xdim];
		}
		for(i=0; i<ydim; i++)
			for(j=0; j<xdim; j++)
			{
				visit[i][j] = 0;
				dist_map[i][j] = 0;
			}

		for(i=0; i<3; i++)
		{
			total[i] = 0;
			mean[i] = 0;
		}
	}
	else
		cout<<"Error in input images!"<<endl;

	if(out)
		cvZero(out);
	else
		cout<<"Error in the out image!"<<endl;
}

CRegionGrow::~CRegionGrow()
{
	for(int i=0; i<ydim; i++)
	{
		delete[] visit[i];
		delete[] dist_map[i];
	}
	delete[] visit;
	delete[] dist_map;
}

//void CRegionGrow::Initialization(IplImage* src, CvPoint p, IplImage* mask, float vmin, float vmax, float diff_t, float dist_t)
//{
//
//}

//void CRegionGrow::releaseMem()
//{
//	for(int i=0; i<ydim; i++)
//	{
//		delete[] visit[i];
//		delete[] dist_map[i];
//	}
//	delete[] visit;
//	delete[] dist_map;
//}

void CRegionGrow::calDistMap(int x, int y)
{
	for(j=0; j<ydim; j++)
		for(i=0; i<xdim; i++)
			dist_map[j][i] = sqrt((double)(j-y)*(j-y)+(i-x)*(i-x));
}

void CRegionGrow::doRegionGrow()
{
	int x, y;

	count = 0;
	for (y = yseed - 2; y <= yseed + 2; y++)
		for (x = xseed - 2; x <= xseed + 2; x++)
			if ((x > 0) && (y > 0) && (x < xdim) && (y <ydim))
			{
				count ++;
				for(i=0; i < 3; i++)
				{
					rgb[i] = CV_IMAGE_ELEM(in,uchar,y,x);
					total[i] += rgb[i];
					//CV_IMAGE_ELEM(out,uchar,y,x) = 255;
				}
			}
	for(i=0; i<3; i++)
		mean[i] = total[i] / (float)count;

	count = 0;
	for(i=0; i<3; i++)
		total[i] = 0;
	calDistMap(xseed, yseed);
	seedGrow(xseed, yseed);

	//cvShowImage("out",out);
	//cvShowImage("in",in);
	//cvWaitKey(0);
}

bool CRegionGrow::checkPixel(int x, int y)
{
	for(i=0; i<3; i++)
		rgb[i] = CV_IMAGE_ELEM(in,uchar,y,x);


	////////The criteria 1 is the pixel value in the range
	//if(rgb[0]>intensity_lower_threshold && rgb[0]<intensity_upper_threshold
	//	&& rgb[1]>intensity_lower_threshold && rgb[1]<intensity_upper_threshold
	//	&& rgb[2]>intensity_lower_threshold && rgb[2]<intensity_upper_threshold)
	//	return true;
	//else
	//	return false;

	//////The criteria 2 is the pixel value different is small
	diff = 0;
	for(i=0; i<3; i++)
	{
		diff += (rgb[i]-mean[i])*(rgb[i]-mean[i]);
	}
	diff = sqrt(double(diff)/3.0);


	if(diff<diff_threshold && dist_map[y][x]<dist_threshold)
		return true;
	else
		return false;
}

void CRegionGrow::seedGrow(int x, int y)
{
	if(visit[y][x] == 0 && count<region_size_threhold)
	{


		if(checkPixel(x,y)==true)
		{
            visit[y][x] = 1;

			CV_IMAGE_ELEM(out,uchar,y,x) = 255;

			count ++;
			for(i=0; i<3; i++)
				total[i] += rgb[i];
			for(i=0; i<3; i++)
				mean[i] = total[i] / (float)count;

             if (x > 2)
				 seedGrow(x - 1, y);
            if (y > 2)
				 seedGrow(x, y - 1);
             if (x < xdim - 2)
				 seedGrow(x + 1, y);
             if (y<ydim-2)
				 seedGrow(x, y + 1);
		}

	}
}
