#include "DICOMin.h"
#include <fstream>
#include <boost/lambda/bind.hpp>
#include <vtkImageData.h>
#include <math.h>

using namespace cv;
using namespace boost::filesystem;
using namespace boost::lambda;
DICOMin::DICOMin(){

}
DICOMin::DICOMin(std::string dName,int numSlices)
{
	if(!exists(dName))
	{
		cout<<"DICOM directory does not exist\n";
		exit(-1);
	}


	//Initialize number of pixels to non calculated value
	this->numberOfPixels = -1;
	//Read bitmap
//	if(is_directory(bName)) //if I got directory, then read series of mask files
//	{
//		//Count files in the directory
//		int n = std::count_if(
//        directory_iterator(bName),
//        directory_iterator(),
//        bind( static_cast<bool(*)(const path&)>(is_regular_file),
//          bind( &directory_entry::path, _1 ) ) );
//		aReader_bmp = Mat(n*512, 512, int(0));
//		directory_iterator end_itr;
//		//int row = 0;
//		for ( directory_iterator itr(bName); itr != end_itr; ++itr )
//		{
//            int idx = n - 1 - extractIDX(itr->path().string().c_str());
//			Mat temp = imread(itr->path().string(), 0);
//			for(int y = 0; y < 512; y++)
//			{
//				for(int x = 0; x < 512; x++)
//				{
//
//					if(temp.at<uchar>(y, x) != 0)
//						aReader_bmp.at<uchar>(idx*512+y, x) = 255;
//					else
//						aReader_bmp.at<uchar>(idx*512+y, x) = 0;
//				}
//				//row++;
//			}
//		}
//		//imwrite("Generated.bmp", aReader_bmp);
//	}
//	else //read mask as a single file
//	{
//		aReader_bmp = imread(bName, 0);
//		if(aReader_bmp.data)
//		{
//			cout<<"Bitmap:\nwidth: "<<aReader_bmp.cols<<" height: "<<aReader_bmp.rows<<endl;
//		}
//		else
//		{
//			cout<<"\nError in reading Bitmap\n";
//			exit(-1);
//		}
//	}
//	this->numOfSlices = aReader_bmp.rows/512;

	//Read DICOM image
	reader = vtkSmartPointer<vtkDICOMImageReader>::New(); //create new dicom image reader
	reader->SetDirectoryName(dName.c_str()); //add directory name
	try
	{
		reader->Update(); //read dicom series
	}
	catch(Exception e)
	{
		cout<<"VTK error while reading dicom series\n";
	}



	//Extract DICOM header info
	OFCondition status;
    boost::filesystem::directory_iterator itr(dName); //for reading filename from the directory
	//Load dicom header info
	status = header.loadFile((itr->path().parent_path().string() + "/" + itr->path().filename().string()).c_str());
	if(!status.good())
	{
		cout<<"Couldn't read DICOM header info\nDcmMetaInfo returned bad while reading dicom file\n";
	}
    numOfSlices = numSlices;
	find_vals_for_normalization();

}
int DICOMin::extractIDX(const char* path){
    int result = 0;
    string str = path;
    string name="";
    int len = str.length();
    int n_begin = 0;
    int j;

    for(j=len-1;j>=0;j--){
        if(str[j]=='/'){
            n_begin = j+1;
            break;
        }
    }
    for(j=n_begin;j<len-1;j++){

        if(str[j]=='.'){
            break;
        }
        name = name+str.at(j);

    }

    result = atoi(name.c_str());

    return result;

}

DICOMin::~DICOMin()
{
}

void DICOMin::calculateNumOfPixels()
{
	this->numberOfPixels = 0;
	//Go through the bitmask and count all white pixels
	for(int y = 0; y < aReader_bmp.rows; y++)
	{
		for(int x = 0; x < aReader_bmp.cols; x++)
		{
			if(aReader_bmp.at<uchar>(y, x) == 255) //if this is a white pixel, increment counter
				this->numberOfPixels++;
		}
	}
}

void DICOMin::find_vals_for_normalization(){

    vtkImageData * scalar = getVTKreader()->GetOutput();
    int x,y,z,tmp_val;
//    _max_val = -10e6;
//    _min_val = 10e6;
//
//    for(z=0;z<numOfSlices;z++){
//        for(y=0;y<512;y++){
//            for(x=0;x<512;x++){
//
//                tmp_val =(int)scalar->GetScalarComponentAsDouble(x, 511 - y % 512, y /512, 0);
//
//                if(_max_val<tmp_val){
//                    _max_val = tmp_val;
//                }
//                if(_min_val>tmp_val){
//                    _min_val = tmp_val;
//                }
//
//            }
//
//
//        }
//
//    }

   double vrange[2];
   getVTKreader()->GetOutput()->GetScalarRange(vrange);
   _min_val = vrange[0];
   _max_val = vrange[1];
}

int DICOMin::getNumOfSlices()
{
	return this->numOfSlices;
}

vtkSmartPointer<vtkDICOMImageReader> DICOMin::getVTKreader()
{
	return this->reader;
}

DcmFileFormat & DICOMin::getHeaderInfo()
{
	return this->header;
}


IplImage* DICOMin::getSllice(int idx){
    IplImage* result = cvCreateImage(cvSize(512, 512), IPL_DEPTH_8U, 1);
    Mat tmp(cvSize(512, 512), CV_8UC1);
    vtkImageData * scalar = getVTKreader()->GetOutput();
    int x, y, tmp_val;
    uchar val;

    for (x=0;x<512;x++){
        for(y=0;y<512;y++){

            tmp_val = (int)scalar->GetScalarComponentAsDouble(x, y, numOfSlices - 1 - idx, 0);

            val = round(255*((double)tmp_val-_min_val)/((double)(_max_val-_min_val)));

            result->imageData[512*(511-y)+x] =val;
            //tmp.at<uchar>(cvPoint(x,511-y)) = val;

        }
    }
//    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
//    cv::imshow( "Display window", tmp );                   // Show our image inside it.
//
//    cv::waitKey(0);
    return result;
}
