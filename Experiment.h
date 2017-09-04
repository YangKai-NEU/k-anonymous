#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include "Head.h"
#include "Hilbert.h"
#include "AnonymousServer.h"
#include "Log.h"

class Experiment {
public:
	double avg_distance;
	double avg_entropy;
	double avg_variance;
	/*
	* query
	*/ 
	void query() {
		int i;
		avg_distance=0;
		avg_entropy=0;
		avg_variance=0;
		for (i = 0; i < USERNUM; i++) {
			int edgeSize = gGraphic->edges.size();

			clock_t st = clock();
			srand((unsigned)st);
			long a=(long)rand()<<16;
			st = clock();
			srand((unsigned)st);
			long b=rand();
			int edgeID = (a | b) % edgeSize;

			QL location(edgeID, rand() * 1.0 / RAND_MAX);
			vector<POIL> result;
			result = gAnonymousServer->query(location);
			if(DISTANCE){
				avg_distance += gAnonymousServer->avg_distance;
			}
			if(ENTROPY){
				avg_entropy+=gAnonymousServer->avg_entropy;
			}
			
			if(VARIANCE){
				avg_variance+=gAnonymousServer->avg_variance;
			}
#ifdef _DEBUG_H_
			cout << "..." << endl;
#ifdef _LOG_
			gLog->WriteString("...\n");
#endif
			result.print();
			cout << "..." << endl;
#ifdef _LOG_
			gLog->WriteString("...\n");
#endif
#endif
			
		}
		if(DISTANCE){
			avg_distance/=(USERNUM-1);
		}
		
		if(ENTROPY){
			avg_entropy/=(USERNUM-1);
		}
		
		if(VARIANCE){
			avg_variance/=(USERNUM-1);
		}
		
		
#ifdef _DEBUG_
		cout << "query finished successfully..." << endl;
#ifdef _LOG_
		gLog->WriteString("query finished successfully...\n");
#endif
#endif // _DEBUG_
	}

};

Experiment *gExperiment=new Experiment();

#endif
