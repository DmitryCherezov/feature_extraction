/*
 *
 * \author Hailing Zhou<hailing.zhou@deakin.edu.au> from Deakin University, CISR
 * \date Mon Nov 10, 2014
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 */
//#include "stdafx.h"
#include "Ensemble3dExtension.h"
#include <dirent.h>
#include "Ensemble.h"
#include "DICOMin.h"
#include <vtkImageData.h>
#include <vtkNIFTIImageWriter.h>
#include <vtkNIFTIImageReader.h>

CEnsemble3dExt::CEnsemble3dExt() {

    aslice = outmask = NULL;
    slices_name.clear();


 }

CEnsemble3dExt::~CEnsemble3dExt(){


}

void CEnsemble3dExt::_free_temporal(){

    int s = _temporal.size();
    for (int j = 0; j < s; j++ ){

        Mat* slice;

        slice = _temporal.back();

        _temporal.pop_back();

        delete slice;

    }

}

void CEnsemble3dExt::LoadSlices(const char* in_folder_name)
{






	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir (in_folder_name)) != NULL)
	{
		while ((ent = readdir (dir)) != NULL)
		{
            string name = ent->d_name;
			if(name.length() > 3)
			{
				////printf ("%s\n", ent->d_name);
				string path = in_folder_name;
				string imgname = ent->d_name;
				string s = path + imgname;
				slices_name.push_back(s);
			}
		}
		closedir (dir);
	}
	else
	{
		perror ("");
		return;
	}
    DICOMin reader(in_folder_name,slices_name.size());
	_dcm = reader;

}

bool CEnsemble3dExt::UpdateSeed(int z, int fb)
{
	float diff_threshold = 15;
	IplImage *cur = NULL, *next = NULL, *maskcore = NULL;
	vector<int> xcoords, ycoords;
	bool has_valid_seed = false;
	int x, y, cur_count = 0,  next_id = 0, next_count = 0, val;
	float imean = 0, min_diff = 10000, diff;
	IplConvKernel* eErode = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT,NULL);

	cur = _dcm.getSllice(z);
	if(fb == FORWARD && z+1<slices_name.size())
		next = _dcm.getSllice(z+1);
	else if(fb == BACKWARD && z-1>=0)
		next = _dcm.getSllice(z-1);
	else
		return has_valid_seed;

	maskcore = cvCreateImage(cvGetSize(outmask),8,1);
	cvZero(maskcore);
	//cvErode(outmask, maskcore, eErode, 1);
	cvCopy(outmask, maskcore);
	for(y=0; y<cur->height; y++)
		for(x=0; x<cur->width; x++)
			if((int)CV_IMAGE_ELEM(maskcore,uchar,y,x)==255)
			{
				cur_count ++;
				imean += CV_IMAGE_ELEM(cur,uchar,y,x);
			}
	if(cur_count > 5)
		imean= imean / (float)cur_count;
	else
		return false; //The curent outmask is too small.

	val = CV_IMAGE_ELEM(next,uchar,seedy,seedx);
	if(abs(val-imean) < diff_threshold)
	{
		return true;
	}

	for(y=0; y<next->height; y++)
		for(x=0; x<next->width; x++)
			if((int)CV_IMAGE_ELEM(maskcore,uchar,y,x)==255)
			{
				val = CV_IMAGE_ELEM(next,uchar,y,x);
				diff = abs(val-imean);
				if( diff < diff_threshold)
				{
					if(diff<min_diff)
					{
						min_diff = diff;
						next_id = next_count;
					}
					next_count ++;
					xcoords.push_back(x);
					ycoords.push_back(y);
					if(!has_valid_seed)
						has_valid_seed = true;
				}
			}


	//cout<<has_valid_seed<<','<<xcoords.size()<<endl;
	if(has_valid_seed)
	{
		seedx = xcoords[next_id];
		seedy = ycoords[next_id];
		cvLine(next,cvPoint(seedx,seedy),cvPoint(seedx,seedy), cvScalarAll(128), 10);
		//cvShowImage("updated seed", next);
	}

	cvReleaseImage(&maskcore);

	return has_valid_seed;
}

void CEnsemble3dExt::SaveResult(const char* folder_name, int z)
{
	string path = folder_name;
	char dirc[100];
	int n = sprintf(dirc,"000%03d.png", z );
    int x,y;
	string dir = path + dirc;
	uchar pixel;

	//printf ("%s\n", dir.c_str());
	//cvSaveImage(dir.c_str(), outmask);

	for(x=0; x<512; x++){
        for(y=0;y<512; y++){


            Mat* slice = _temporal[z];
//            cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
//            cv::imshow( "Display window", *slice );                   // Show our image inside it.
//
//            cv::waitKey(0);
            pixel = CV_IMAGE_ELEM(outmask,uchar,y,x);

            slice->at<uchar>(cvPoint(x,y)) = pixel;

        }

	}
}

void CEnsemble3dExt::Ensemble3Dvolume(const char* folder_name)
{
    _init_temporal();
	int k = 0, max_nodule_size = 35;
	int start = seedz, z = seedz;
	int org_x = seedx, org_y = seedy, org_z = seedz;
	for(z=start; z>=0; z--)
	{
		//printf ("%s\n", slices_name[z].c_str());
		cout<<z<<":"<<seedx<<','<<seedy<<endl;
		aslice = _dcm.getSllice(z);
//		cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
//        cv::imshow( "Display window", cv::Mat(aslice) );                   // Show our image inside it.
//
//        cv::waitKey(0);
		outmask = cvCreateImage(cvGetSize(aslice),8,1);
		cvZero(outmask);

		//cvShowImage("in", aslice);
		//cvWaitKey(0);

		CvPoint seed = cvPoint(seedx,seedy);
		//cvSmooth(aslice, aslice);
		CEnsemble ensemble(aslice, seed, outmask);
		ensemble.doEnsembleSegmentation();
		SaveResult(folder_name, z);

		if(!UpdateSeed(z, BACKWARD))
			break;

		k ++;
		if(k > max_nodule_size)
			break;
		cvReleaseImage(&outmask);
	}

	seedx = org_x;
	seedy = org_y;
	seedz = org_z;
	k = 0;
	for(z=start+1; z<slices_name.size(); z++)
	{
		////printf ("%s\n", slices_name[i].c_str());
		cout<<z<<":"<<seedx<<','<<seedy<<endl;

		aslice = _dcm.getSllice(z);
		outmask = cvCreateImage(cvGetSize(aslice),8,1);
		cvZero(outmask);

		CvPoint seed = cvPoint(seedx,seedy);
		//cvSmooth(aslice, aslice);
		CEnsemble ensemble(aslice, seed, outmask);
		ensemble.doEnsembleSegmentation();

		SaveResult(folder_name, z);
		if(!UpdateSeed(z, FORWARD))
			break;

		k ++;
		if(k > max_nodule_size)
			break;

		//cvWaitKey(0);
		cvReleaseImage(&outmask);
	}

	_saveToNiffty(_temporal, folder_name );

}


void CEnsemble3dExt::_init_temporal(){

    _free_temporal();

    int numSlices = _dcm.getNumOfSlices();
    for( int j = 0; j<numSlices; j++ ){

        Mat* slice = new Mat(512,512,CV_8UC1,Scalar(0));
//        cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
//        cv::imshow( "Display window", *slice );                   // Show our image inside it.
//
//        cv::waitKey(0);

        _temporal.push_back(slice);

    }


}


void CEnsemble3dExt::_saveToNiffty(vector<Mat*> _data, const char* _name ){

    int x,y,z;
    Mat* slice;
    uchar val;
    vtkSmartPointer<vtkNIFTIImageWriter> writer =  vtkSmartPointer<vtkNIFTIImageWriter>::New();
    writer->SetFileName(_name);
    writer->SetNIFTIVersion(2);

    vtkSmartPointer<vtkImageData> scalar = vtkSmartPointer<vtkImageData>::New();

    scalar->SetDimensions(512,512,_data.size());

    scalar->AllocateScalars(VTK_DOUBLE,1);

    for(z = _data.size()-1; z >= 0; z-- ){

        slice = _data[z];
        for(y = 0; y < 512; y++){

            for (x = 0; x < 512; x++ ){

                val = slice->at<uchar>(cvPoint(x,y));
                scalar->SetScalarComponentFromDouble (x, y, z, 0, (double)val);


            }
        }
    }
    writer->SetInputData(scalar);
    //scalar->SetDataExtent( 0, 1023, 0, 511, 0, 0 );;
    writer->Write();
}




