#pragma once
#ifndef _NORMALBOX_H_
#define _NORMALBOX_H_

#include <string>
#include <vector>

class NormalBox
{
public:

	NormalBox(BoundingBox* b, float r0, float g0, float b0, float r1, float g1, float b1, float r2, float g2, float b2): b(b), r0(r0), r1(r1), r2(r2), g0(g0), g1(g1), g2(g2), b0(b0), b1(b1), b2(b2){
	}

	virtual ~NormalBox(){}

	float getR0() const { return r0; }
	float getR1() const { return r1; }
	float getR2() const { return r2; }
	float getG0() const { return g0; }
	float getG1() const { return g1; }
	float getG2() const { return g2; }
	float getB0() const { return b0; }
	float getB1() const { return b1; }
	float getB2() const { return b2; }
	BoundingBox* getB() { return b; }

private:
	BoundingBox* b;
	float r0;
	float r1;
	float r2;
	float g0;
	float g1;
	float g2;
	float b0;
	float b1;
	float b2;
};

#endif
