#ifndef _EXP_H_
#define _EXP_H_

#include "Head.h"
#include "Log.h"

class Exp {
public:

	string node_path;
	string edge_path;

	/*
	* deal with the parameters, do exp
	*/
	void DealParameter(int argc, char* argv[]) {
		if (argc >= 15) {
			node_path = argv[1];
			edge_path = argv[2];
			GN = atoi(argv[3]);
			GFREQUENCY = atoi(argv[4]);
			THRESHOLD = atoi(argv[5]);
			POIN = atoi(argv[6]);
			K = atoi(argv[7]);
			USERNUM = atoi(argv[8]);
			QUERYTYPE = atoi(argv[9]);
			LOG_FILE = argv[10];
			DISTANCE=atoi(argv[11]);
			ENTROPY=atoi(argv[12]);
			VARIANCE=atoi(argv[13]);
			KNN=atoi(argv[14]);
		}
		else {
			node_path = "/home/yangkai/k-anonymous/roadnetwork/WA_Nodes.txt";
			edge_path = "/home/yangkai/k-anonymous/roadnetwork/WA_Edges.txt";
			GN = 13;
			GFREQUENCY = 200000;
			THRESHOLD = 0;
			POIN = 1000;
			K = 20;
			USERNUM = 10000;
			QUERYTYPE = RANDOM_ORDER_TRAVERSE;
			time_t t = std::time(0);
			struct tm * now = std::localtime(&t);
			char logname[1000];
			sprintf(
				logname,
				"/home/yangkai/k-anonymous/log/%04d-%02d-%02d %02d-%02d-%02d.txt", 
				now->tm_year + 1900, 
				now->tm_mon+1, 
				now->tm_mday,
				now->tm_hour,
				now->tm_min,
				now->tm_sec);
			LOG_FILE = logname;
			DISTANCE=true;
			ENTROPY=true;
			VARIANCE=true;
			KNN=1;
		}
		
#ifdef _DEBUG_
		cout << "deal with the parameters successfully..." << endl;
#endif // _DEBUG_

	}

};

Exp *gExp = new Exp();

#endif
