#include "Head.h"
#include "Exp.h"
#include "Util2D.h"
#include "Graphic.h"
#include "Hilbert.h"
#include "Experiment.h"
#include "Timer.h"
#include "Traverse.h"
#include "Log.h"

int main(int argc, char* argv[]) {
	
	Timer *whotimer = new Timer();
	whotimer->SC();

	//deal with the parameters which user specified
	gExp->DealParameter(argc, argv);

#ifdef _LOG_
	gLog->OpenFile();
#endif

	/*
	* Init the Hilbert Curve and the Voronoi Diagram
	*/
	
	//construct a graphic from the road network files
	gGraphic->CGFF(gExp->node_path, gExp->edge_path);

	//generate historical query frequency for every edge in graphic randomwly
	gGraphic->CF();

	//generate some POIs in road network randomwly
	gGraphic->MPOI();

	//construct voronoi diagram
	Timer *constructTime = new Timer();
	constructTime->SC();
	gGraphic->MNVD();
	constructTime->EC();
	cout<<constructTime->DC()<<" millisecond are used in constructing voronoi diagram..."<<endl;

	switch (QUERYTYPE) {
	case HILBERT_CURVE_TRAVERSE:
		//update the range of the grid, i.e. left, right, top and bottom
		gHilbert->IRWN();

		//construct the hilbert curve
		gHilbert->MPHC();

		break;
	case DEPTH_FIRST_TRAVERSE:
		//depth first traverse of the road network
		gTraverse->DepthFirstTraverse();

		break;
	case BREADTH_FIRST_TRAVERSE:
		//breadth first traverse of the road network
		gTraverse->BreadthFirstTraverse();

		break;
	case RANDOM_ORDER_TRAVERSE:
		//traverse edges randomly
		gTraverse->RandomTraverse();
		break;
	default:
		//update the range of the grid, i.e. left, right, top and bottom
		gHilbert->IRWN();

		//construct the hilbert curve
		gHilbert->MPHC();

		break;
	}

	/*
	* Do experiments with various parameters
	*/

	Timer *querytimer = new Timer();
	querytimer->SC();

	//queries in experiment
	gExperiment->query();

	querytimer->EC();
	whotimer->EC();
	double queryduration = querytimer->DC();
	double wholeduration = whotimer->DC();

	//show the all kinds of time used in this process

	cout << "..." << endl;
	cout << "frequency="<<GFREQUENCY<<"\tPOIs="<<POIN<<"\tK="<<K<<"\tusers="<<USERNUM<<"\tquery type="<<QUERYTYPE<<endl;
	if(DISTANCE){
		cout << "avgdistance="<<gExperiment->avg_distance<<endl;
#ifdef _LOG_
		gLog->WriteString("...\n");
		gLog->WriteString("avgdistance=");
		gLog->WriteDouble(gExperiment->avg_distance);	
		gLog->WriteString("\n...\n");
#endif
	}
	if(ENTROPY){
		cout << "avgentropy="<<gExperiment->avg_entropy<<endl;
#ifdef _LOG_
		gLog->WriteString("...\n");
		gLog->WriteString("avgentropy=");
		gLog->WriteDouble(gExperiment->avg_entropy);	
		gLog->WriteString("\n...\n");
#endif
	}
	if(VARIANCE){
		cout << "avgvariance="<<gExperiment->avg_variance<<endl;
#ifdef _LOG_
		gLog->WriteString("...\n");
		gLog->WriteString("avgvariance=");
		gLog->WriteDouble(gExperiment->avg_variance);	
		gLog->WriteString("\n...\n");
#endif
	}

	cout << wholeduration << " millisecond are used in whole process" << endl;
	cout << wholeduration-queryduration << " millisecond are used in preduce process" << endl;
#ifdef _DEBUG_
	cout << "..." << endl;
	cout << queryduration << " millisecond are used in query process" << endl;
	cout << "..." << endl;
#endif
	cout << queryduration / USERNUM / K << " millisecond are used in everage query process" << endl;
	cout << (queryduration-gAnonymousServer->total_query_time) / USERNUM / K << " millisecond are used in everage anonymous process" << endl;
	cout << (queryduration-gAnonymousServer->total_dividing_road_network_time) / USERNUM << " millisecond are used in dividing road network process" << endl;
	cout << "..." << endl;
	

#ifdef _LOG_
	gLog->WriteString("...\n");
	gLog->WriteDouble(wholeduration);
	gLog->WriteString(" millisecond are used in whole process\n");
	gLog->WriteString("...\n");
	gLog->WriteString("...\n");
	gLog->WriteDouble(wholeduration-queryduration);
	gLog->WriteString(" millisecond are used in preduce process\n");
	gLog->WriteString("...\n");
	gLog->WriteString("...\n");
	gLog->WriteDouble(queryduration);
	gLog->WriteString(" millisecond are used in query process");
	gLog->WriteString("...\n");
	gLog->WriteString("...\n");
	gLog->WriteDouble(queryduration / USERNUM / K);
	gLog->WriteString(" millisecond are used in everage query process");
	gLog->WriteString("...\n");
#endif

	return 0;
}
