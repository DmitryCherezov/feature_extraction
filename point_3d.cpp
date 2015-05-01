//#include "stdafx.h"
#include "point_3d.h"


point_3d::point_3d()
{
	cx = 0;
	cy = 0;
	cz = 0;
}


point_3d::~point_3d()
{

}


void point_3d::set(int x, int y, int z){

	cx = x;
	cy = y;
	cz = z;

}

int point_3d::x(){

	return cx;

}

int point_3d::y(){

	return cy;
}

int point_3d::z(){

	return cz;
}

point_3d &point_3d::operator=(point_3d &other) {
	point_3d result;
	result.set(other.x(), other.y(), other.z());
	return result;
}
