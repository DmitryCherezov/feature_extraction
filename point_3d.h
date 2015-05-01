#pragma once
class point_3d
{
public:
	point_3d();
	~point_3d();
	int x();
	int y();
	int z();
	void set(int x, int y, int z);
	point_3d &operator=(point_3d &);
private:
	int cx, cy, cz;
};


