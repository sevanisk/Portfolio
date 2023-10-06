#pragma once
#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include <string>
#include <vector>

class BoundingBox
{
public:
	BoundingBox(int xmin, int xmax, int ymin, int ymax);

	BoundingBox(int x0, int y0, float z0, int x1, int y1, float z1, int x2, int y2, float z2): x0(x0), x1(x1), x2(x2), y0(y0), y1(y1), y2(y2), z0(z0), z1(z1), z2(z2){
		xmin = findMin(x0, x1, x2);
		xmax = findMax(x0, x1, x2);
		ymin = findMin(y0, y1, y2);
		ymax = findMax(y0, y1, y2);
	}

	virtual ~BoundingBox(){}
	int getMinX() const { return xmin; }
	int getMaxX() const { return xmax; }
	int getMinY() const { return ymin; }
	int getMaxY() const { return ymax; }

	int getX0() const { return x0; }
	int getX1() const { return x1; }
	int getX2() const { return x2; }
	int getY0() const { return y0; }
	int getY1() const { return y1; }
	int getY2() const { return y2; }
	float getZ0() const { return z0; }
	float getZ1() const { return z1; }
	float getZ2() const { return z2; }
	int findMin(int a, int b, int c){
		if(a <= b && a <= c){
			return a;
		}
		else if(b <= a && b <= c){
			return b;
		}
		else{
			return c;
		}
	}

	int findMax(int a, int b, int c){
		if(a >= b && a >= c){
			return a;
		}
		else if(b >= a && b >= c){
			return b;
		}
		else{
			return c;
		}
	}

	bool in_box(int x, int y){
		if(y < ymin || y > ymax){
			return false;
		}
		if(x < xmin || x > xmax){
			return false;
		}
		return true;
	}

private:
	int xmin;
	int xmax;
	int ymin;
	int ymax;
	int x0;
	int x1;
	int x2;
	int y0;
	int y1;
	int y2;
	float z0; 
	float z1;
	float z2;
};

#endif
