#include <iostream>
#include "RegionGrow.h"
#include "Ensemble3dExtension.h"
//#define TESTING

using namespace std;

int main(int argc, char *argv[])
{

    string in_dir,out_dir;
    int x,y,z;
    #ifdef TESTING

    in_dir = "/home/dmitry/USF/research/data/data/moffitt/L0007/";
    x = 172;
    y = 253;
    z = 96;
    out_dir = "/home/dmitry/Desktop/test/test_seg.nii.gz";


    #else


    if(argc != 6 )
    {
        cout<<"5 parameters are required for input, like:"<<endl;
        cout<<"in_folder_name, seed_x, seed_y, seed_z, out_file_name"<<endl;
        cout<<".//segmentation ..//QIN-LUNG-01-0007// 172 257 22 ..//QIN-LUNG-01-0007-out.nii.gz"<<endl;
        return 1;
    }

    in_dir = argv[0];
    x = atoi( argv[2] );
    y = atoi( argv[3] );
    z = atoi( argv[4] );
    out_dir = argv[5];


    #endif // TESTING

    CEnsemble3dExt ensemble3de;
    ensemble3de.LoadSlices(in_dir.c_str());
//It is the index of the slicer that the seed is specified. The index starts from 0.
    ensemble3de.InputSeed(x,y,z);
    ensemble3de.Ensemble3Dvolume(out_dir.c_str());
    return 0;
}
