#ifndef _HEAD_H_
#define _HEAD_H_

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <stack>
#include <algorithm>
#include <ctime>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
using namespace std;

//#define _DEBUG_
#define _LOG_
//#define _DISTANCE_

// the depth of the grid, that is (2^GN)*(2^GN)
static int GN;

//the number of the POI
static int POIN;

//the frequency in the road network which is given randomly
static int GFREQUENCY;

//the threshold of the query frequency
static double THRESHOLD;

static int K;
static int KNN;

//the number of the users who take the queries
static int USERNUM;

//the query type specified
static int QUERYTYPE;

static string LOG_FILE;

//average distance
static bool DISTANCE;

//entropy
static bool ENTROPY;

//variance
static bool VARIANCE;

const int HILBERT_CURVE_TRAVERSE = 0;
const int DEPTH_FIRST_TRAVERSE=1;
const int BREADTH_FIRST_TRAVERSE = 2;
const int RANDOM_ORDER_TRAVERSE = 3;

#endif
