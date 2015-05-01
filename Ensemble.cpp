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
#include "Ensemble.h"
#include "RegionGrow.h"
#include <iostream>
using namespace std;

class CRegionGrow;
CEnsemble::CEnsemble(IplImage* src, CvPoint p, IplImage* mask)
{
	seed = p;
	in = src;
	outmask = mask;
}

void CEnsemble::getIntersection(IplImage* results[CORE_SEEDS_NUM], IplImage* out)
{
	int x, y, i;
	IplImage* sum = cvCreateImage(cvGetSize(out),32,1);
	cvZero(sum);
	for(i=0; i<CORE_SEEDS_NUM ; i++)
		cvAdd(results[i],sum, sum);
	cvZero(out);
	float sum255 = 255*CORE_SEEDS_NUM;
	for(y=0; y<sum->height; y++)
	{
		for(x=0; x<sum->width; x++)
		{
			if(CV_IMAGE_ELEM(sum,float,y,x)==sum255)
				CV_IMAGE_ELEM(out,uchar,y,x)=255;
		}
	}

}
void CEnsemble::merge2image(IplImage* src, IplImage*out)
{
	int x, y;
	for(y=0; y<out->height; y++)
	{
		for(x=0; x<out->width; x++)
		{
			if((int)CV_IMAGE_ELEM(src,uchar,y,x)==255)
				CV_IMAGE_ELEM(out,uchar,y,x)=255;
		}
	}
}
bool CEnsemble::GetCentre(IplImage* mask, int& centrex, int& centrey)
{
	int x, y,c = 0;
	int sumx = 0, sumy = 0;
	for(y=0; y<mask->height; y++)
	{
		for(x=0; x<mask->width; x++)
		{
			if((int)CV_IMAGE_ELEM(mask,uchar,y,x)==255)
			{
				sumx += x;
				sumy += y;
				c++;
			}
		}
	}

	if(c>0)
	{
		centrex = sumx/c;
		centrey = sumy/c;
		return true;
	}
	else
	{
		centrex=-100;
		centrey=-100;
		return false;
	}

	////for Debug
	//cvDrawLine(mask,cvPoint(centrex,centrey), cvPoint(centrex,centrey), cvScalarAll(128), 5);
	//cvNamedWindow("centre");
	//cvShowImage("centre",mask);
}
void CEnsemble::checkResult(IplImage* result, int corex, int corey)
{
/*
	CvMemStorage * storage = cvCreateMemStorage(0);
    CvSeq *contours = 0;
	IplImage* temp = cvCloneImage(result);
	int cx, cy;
	GetCentre(temp, cx, cy);
	if(cx>0 && cy>0) //result is not empty
	{
		float dist = sqrt((double)((corex-cx)*(corex-cx)+(corey-cy)*(corey-cy)));
		//cvFindContours( temp, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		//for( ; contours != 0; contours = contours->h_next )
		//	if(contours->total<10)  // remove noise
		//		cvZero(result);
		//if(dist>20)
		//	cvZero(result);
	}
	cvReleaseImage(&temp);
*/
}

bool CEnsemble::findCoreRegion(IplImage* initial, IplImage* out, int& centrex, int& centrey)
{
	IplConvKernel* eErode = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT,NULL);
	IplConvKernel* eDilate = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT,NULL);

	int adaptiveLower,adaptiveUpper, i;
	IplImage* results[CORE_SEEDS_NUM];
	for(i=0; i<CORE_SEEDS_NUM; i++)
	{
		results[i] = cvCreateImage(cvGetSize(initial),8,1);
		cvZero(results[i]);
	}

	cvErode(initial, initial, eErode, 1);
	cvDilate(initial, initial, eDilate, 1);
	bool is_sucess = GetCentre(initial, centrex, centrey);
	adaptiveLower = CV_IMAGE_ELEM(in,uchar,centrey, centrex) - 10;
	adaptiveUpper = CV_IMAGE_ELEM(in,uchar,centrey, centrex) + 10;
	if(is_sucess) //results[i] is not empty
		ClickandGrow(cvPoint( centrex, centrey), results[0], adaptiveLower,adaptiveUpper, 10, 30);
	else
		return false;

	////for Debug
	//cout<<centrex<<','<<centrey<<endl;
	//cvNamedWindow("results[0]");
	//cvShowImage("results[0]", results[0]);

	for(i=1; i<CORE_SEEDS_NUM; i++)
	{
		is_sucess = GetCentre(results[i-1], centrex, centrey);

		adaptiveLower = CV_IMAGE_ELEM(in,uchar,centrey, centrex) - 10;
		adaptiveUpper = CV_IMAGE_ELEM(in,uchar,centrey, centrex) + 10;

		if(is_sucess) //results[i] is not empty
			ClickandGrow(cvPoint(centrex, centrey), results[i], adaptiveLower,adaptiveUpper, 8, 15);

		////for Debug
		//cout<<centrex<<','<<centrey<<endl;
		//cvNamedWindow("results[i]");
		//cvShowImage("results[i]", results[i]);
		//cvWaitKey(0);
	}
	cvZero(out);
	getIntersection(results, out);
	cvErode(out, out, eErode, 1);
	//cvDilate(out, out, eDilate, 1);

	for(i=0; i<CORE_SEEDS_NUM; i++)
		cvReleaseImage(&results[i]);

	return true;
}

bool CEnsemble::find6PrimarySeeds(IplImage* core, int seeds[PRIMARY_SEEDS_NUM][2])
{
	CvMemStorage * storage = cvCreateMemStorage(0);
    CvSeq *contours = 0, *corecontour = 0;
	int centrex, centrey, i;
	bool find_contour = false;

	bool is_sucess = GetCentre(core, centrex, centrey);
	if(!is_sucess)
		return false;

	IplImage* temp = cvCloneImage(core);
	seeds[0][0] = centrex;
	seeds[0][1] = centrey;
	IplConvKernel* eDilate = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT,NULL);
	cvDilate(temp, temp, eDilate, 1);
	cvFindContours( temp, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	for( ; contours != 0; contours = contours->h_next )
		if(contours->total>5)  // remove noise
		{
			corecontour = &(*contours);
			find_contour = true;
		}
	if(!find_contour)
		return false;

	for(i=1; i<6; i++)
	{
		seeds[i][0] = seeds[0][0];
		seeds[i][1] = seeds[0][1];
	}
	for(i=0; i<corecontour->total; i++)
	{
		CvPoint* r = CV_GET_SEQ_ELEM( CvPoint, corecontour, i );
		if(abs(r->y-centrey)<2 && r->x>centrex)
		{
			seeds[1][0] = r->x + 1;
			seeds[1][1] = r->y;
		}
		if(abs(r->x-centrex)<2 && r->y<centrey)
		{
			seeds[2][0] = r->x;
			seeds[2][1] = r->y - 1;
		}
		if(abs(r->y-centrey)<2 && r->x<centrex)
		{
			seeds[3][0] = r->x - 1;
			seeds[3][1] = r->y;
		}
		if(abs(r->x-centrex)<2 && r->y>centrey)
		{
			seeds[4][0] = r->x;
			seeds[4][1] = r->y + 1;
		}
		if(i==5)  //the random seed
		{
			seeds[5][0] = r->x;
			seeds[5][1] = r->y;
		}
		//cvDrawLine(core,cvPoint(r->x,r->y), cvPoint(r->x,r->y), cvScalarAll(128), 1);
	}
	cvReleaseImage(&temp);


	////for Debug
	for(i=0; i<PRIMARY_SEEDS_NUM; i++)
		cvDrawLine(core,cvPoint(seeds[i][0],seeds[i][1]), cvPoint(seeds[i][0],seeds[i][1]), cvScalarAll(128), 5);

	return true;
}

void CEnsemble::ClickandGrow(CvPoint seed, IplImage* out, float lower, float upper, float diff_t, float dist_t)
{
	CRegionGrow region_grow(in, seed, out, lower, upper, diff_t, dist_t);
	region_grow.doRegionGrow();

	//cout<<seed.x<<','<<seed.y<<','<<lower<<','<<upper<<endl;
	//region_grow.releaseMem();
	//cvShowImage("initialResult", out);
}

void CEnsemble::doEnsembleSegmentation()
{
	////Step 1
	int inSeedx = seed.x, inSeedy = seed.y;
	int centrex, centrey;
	float adaptiveLower, adaptiveUpper;
	IplImage* initialResult = cvCreateImage(cvGetSize(in), 8, 1);
	adaptiveLower = CV_IMAGE_ELEM(in,uchar,inSeedy, inSeedx) - 5;
	adaptiveUpper = CV_IMAGE_ELEM(in,uchar,inSeedy, inSeedx) + 5;
	ClickandGrow(cvPoint(inSeedx, inSeedy), initialResult, adaptiveLower, adaptiveUpper, 10, 25); //10,25
//	cvNamedWindow("initialResult");
//	cvShowImage("initialResult",initialResult);
//	cvWaitKey(0);

	////Step 2
	IplImage* core = cvCreateImage(cvGetSize(in),8,1);
	bool core_sucess = findCoreRegion(initialResult, core, centrex, centrey);
	if(!core_sucess)
	{
		cout<<"The core region is empty!!!"<<endl;
		cout<<"Bad seed,Try another seed please!"<<endl;
		cvCopy(initialResult,outmask);
		return;
	}
	//cvNamedWindow("core");
	//cvShowImage("core",core);

	////Step 3
	int seeds[PRIMARY_SEEDS_NUM ][2];
	bool pseed_success = find6PrimarySeeds(core, seeds);
	if(!pseed_success)
	{
		cout<<"No primary seeds are found!!!"<<endl;
		cout<<"Bad seed,Try another seed please!"<<endl;
		//cvCopy(initialResult,outmask);
		return;
	}
	//cvNamedWindow("6seeds");
	//cvShowImage("6seeds",core);

	////Step 4
	IplConvKernel* eErode = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT,NULL);
	IplConvKernel* eDilate = cvCreateStructuringElementEx(5,5,1,1,CV_SHAPE_RECT,NULL);
	IplImage* temp = cvCreateImage(cvGetSize(in),8,1);
	IplImage* finalResult = cvCreateImage(cvGetSize(in),8,1);
	cvZero(finalResult);
	merge2image(core, finalResult);
	for(int i=0; i<PRIMARY_SEEDS_NUM ; i++)
	{
		cvZero(temp);
		adaptiveLower = CV_IMAGE_ELEM(in,uchar, seeds[i][1], seeds[i][0]) - 10;
		adaptiveUpper = CV_IMAGE_ELEM(in,uchar, seeds[i][1], seeds[i][0]) + 10;
		ClickandGrow(cvPoint(seeds[i][0], seeds[i][1]), temp, adaptiveLower, adaptiveUpper, 6, 30);
		checkResult(temp, centrex, centrey);
		cvErode(temp, temp, eErode, 1);
		cvDilate(temp, temp, eDilate, 1);
		merge2image(temp, finalResult);

		//cvNamedWindow("temp");
		//cvShowImage("temp",temp);
		//cvWaitKey(0);
	}
	cvCopy(finalResult,outmask);

	IplImage* red_mask = cvCreateImage(cvGetSize(in),8,3);
	//cvSet(red_mask, cvScalar(255,0,0),finalResult);
	//////cvCvtColor(in, gray, CV_RGB2GRAY);
	//cvAddWeighted(in, 0.5, red_mask, 0.5, 0, red_mask);
	//cvLine(red_mask, cvPoint(seed.x,seed.y), cvPoint(seed.x,seed.y), cvScalar(0,0,255), 5);
	//cvNamedWindow("finalResult");
	//cvShowImage("finalResult",red_mask);
	//
	//cvWaitKey(0);

	cvReleaseImage(&initialResult);
	cvReleaseImage(&core);
	cvReleaseImage(&temp);
	cvReleaseImage(&finalResult);
	cvReleaseImage(&red_mask);

}
