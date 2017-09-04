#ifndef _UTIL2D_H_
#define _UTIL2D_H_

#include "Head.h"

class Util2D {
public:

	/*
	* euclidean distance
	*/
	static double ED(double x1, double y1, double x2, double y2) {
		double a = x1 - x2;
		double b = y1 - y2;
		double s = sqrt(a * a + b * b);
		return s;
	}
};

#endif
