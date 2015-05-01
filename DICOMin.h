#pragma once
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <vtkSmartPointer.h>
#include <vtkDICOMImageReader.h>
#include <vtkStringArray.h>
#include <boost/filesystem.hpp>
#include <boost/lambda/bind.hpp>
#include <dcmtk/config/cfunix.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcistrmf.h>


class DICOMin
{
public:
    DICOMin();
	DICOMin(std::string dName,int numSlices); //construct object with DICOM reader and BMP reader
	~DICOMin();
	int getNumOfSlices(); //return number of slices in DICOM image
	vtkSmartPointer<vtkDICOMImageReader> getVTKreader();
	DcmFileFormat & getHeaderInfo(); //return reference to the DICOM header reader object

	IplImage* getSllice(int idx);
private:
	void calculateNumOfPixels(); //calculates number of tumor pixels

	//Fields
	DcmFileFormat header; //Dicom header file reader
	cv::Mat aReader_bmp; //bitmask
	vtkSmartPointer<vtkDICOMImageReader> reader; //DICOM image
	int numOfSlices; //number of DICOM image slices
	int numberOfPixels; //number of tumor pixels
	int extractIDX(const char* path);
	int _min_val;
	int _max_val;
	void find_vals_for_normalization();
};
