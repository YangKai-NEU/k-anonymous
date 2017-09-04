#ifndef _ANONYMOUS_SERVER_H
#define _ANONYMOUS_SERVER_H

#include "Head.h"
#include "Hilbert.h"
#include "Traverse.h"
#include "DataServer.h"
#include "Log.h"
#include "Util2D.h"
#include "Timer.h"

struct POI{
public:
	//edge id, loc
	vector<pair<int,int>/**/> edges;
};

class AnonymousServer {
public:

	double avg_distance;
	double avg_entropy;
	double avg_variance;
	
	double total_query_time;
	double total_dividing_road_network_time;
	
	AnonymousServer(){
		this->total_query_time=0;
		this->total_dividing_road_network_time=0;
	}

	/*
	* query interface
	*/
	vector<POIL> query(QL& location) {
		this->avg_distance=0;
		switch(QUERYTYPE) {
		case HILBERT_CURVE_TRAVERSE:
			return QueryWithHilbertCurve(location);
			break;
		case DEPTH_FIRST_TRAVERSE:
		case BREADTH_FIRST_TRAVERSE:
			return QueryWithTraverse(location);
			break;
		case RANDOM_ORDER_TRAVERSE:
			return QueryWithRandom(location);
			break;
		default:
			return QueryWithHilbertCurve(location);
		}
	}
	
	/*
	* the ratio of a point in one edge
	*/
	

	/*
	* query with hilvert curve
	*/
	vector<POIL> QueryWithHilbertCurve(QL& location) {
		int i,j;
		double fre = gGraphic->edges[location.edge]->query_frequency;
		int hSize=gHilbert->hilbert_POIs.size();
		int nSize=gGraphic->nodes.size();
		POI *POIs=new POI[hSize];
		int *POIindex=new int[nSize];
		
		//the satisfied POIs
		vector<int> locs;
		
		Timer *whotimer = new Timer();
		whotimer->SC();
		
		for(i=0;i<hSize;i++){
			Node *tNode=gGraphic->nodes[gHilbert->hilbert_POIs[i].first];
			if(tNode->is_poi){
				int vSize=tNode->voronoi_edges.size();
				bool visited=false;
				for(j=0;j<vSize;j++){
					Edge *tEdge=gGraphic->edges[tNode->voronoi_edges[j].first];
					double absfre=tEdge->query_frequency-fre;
					absfre=absfre>=0?absfre:-absfre;
					if(absfre<=THRESHOLD){
						Edge *ttEdge=gGraphic->edges[tNode->voronoi_edges[j].first];
						POIs[i].edges.push_back(tNode->voronoi_edges[j]);
						if(!visited){
							locs.push_back(i);
							POIindex[tNode->id]=i;
							visited=true;
						}
					}
				}
			}
		}
		
		whotimer->EC();
		this->total_dividing_road_network_time+=whotimer->DC();
		
		int maxRange=0;
		
		for(i=0;i<hSize;i++){
			int t=POIs[i].edges.size();
			maxRange=maxRange>t?maxRange:t;
		}
		
		int lSize=locs.size();
		
		vector<QL> qNodes;
		QL clientQL(location.edge, location.ratio);
		int order;
		int poiorder;
		Edge *eEdge=gGraphic->edges[location.edge];
		if(eEdge->is_divided){
			if(location.ratio<=eEdge->poi_nodes.second){
				poiorder=eEdge->poi_nodes.first.first;
			}else{
				poiorder=eEdge->poi_nodes.first.second;
			}
		}else{
			poiorder=eEdge->poi_nodes.first.first;
		}
		int eeSize=POIs[POIindex[poiorder]].edges.size();
		for(i=0;i<eeSize;i++){
			if(POIs[POIindex[poiorder]].edges[i].first==location.edge){
				order=i;
				break;
			}
		}
		
		
#ifdef _DEBUG_
		cout << "..." << endl;
		cout << "the " << K << " query locations are shown as follows :" << endl;
#ifdef _LOG_
		gLog->WriteString("...\nthe ");
		gLog->WriteInt(K);
		gLog->WriteString(" query locations are shown as follows :\n");
#endif
		clientQL.print();
#endif
		qNodes.push_back(clientQL);
		if (lSize < K) {
			for (i = 0; i < lSize; i++) {
				double r;
				clock_t st = clock();
				srand((unsigned)st);
				//check whether the edge contains historical locations
				int pSize=POIs[locs[i]].edges.size();
				int porder=pSize*order/maxRange;
				if (gGraphic->cache_map.find(POIs[locs[i]].edges[porder].first) != gGraphic->cache_map.end()) {
					r = gGraphic->cache_map[POIs[locs[i]].edges[porder].first];
				}
				else {
					r = rand()*1.0 / RAND_MAX;
				}
				QL tQL(POIs[locs[i]].edges[porder].first, r);
#ifdef _DEBUG_
				tQL.print();
#endif
				qNodes.push_back(tQL);
			}
		}else {
			int interval = lSize / (K - 1);
			for (i = 0; i < lSize; i += interval) {
				double r;
				clock_t st = clock();
				srand((unsigned)st);
				//check whether the edge contains historical locations
				int pSize=POIs[locs[i]].edges.size();
				int porder=pSize*order/maxRange;
				if (gGraphic->cache_map.find(POIs[locs[i]].edges[porder].first) != gGraphic->cache_map.end()) {
					r = gGraphic->cache_map[POIs[locs[i]].edges[porder].first];
				}
				else {
					r = rand()*1.0 / RAND_MAX;
				}
				QL tQL(POIs[locs[i]].edges[porder].first, r);
#ifdef _DEBUG_
				tQL.print();
#endif
				qNodes.push_back(tQL);
			}
		}
#ifdef _DEBUG_
		cout << "..." << endl;
#ifdef _LOG_
		gLog->WriteString("...\n");
#endif
#endif
		int qSize = qNodes.size();
		int pos = rand() % qSize;

		//mix the true location and the henerated locations, put the true location to a random postion
		int e;
		e=qNodes[0].edge;
		qNodes[0].edge = qNodes[pos].edge;
		qNodes[pos].edge= e;
		
		double r;

		r=qNodes[0].ratio ;
		qNodes[0].ratio = qNodes[pos].ratio;
		qNodes[pos].ratio =r;
		
		if(DISTANCE){
			double distance_sum=0;
			for(i=0;i<qSize;i++){
				distance_sum+=gGraphic->NetworkDistance(qNodes[i].edge,qNodes[i].ratio,qNodes[pos].edge,qNodes[pos].ratio);
			}
			this->avg_distance=distance_sum/qSize;
		}
		
		if(ENTROPY){
			double entropy_sum=0;
			double sum=0;
			for(i=0;i<qSize;i++){
				sum+=gGraphic->edges[qNodes[i].edge]->query_frequency;
			}
			for(i=0;i<qSize;i++){
				double r=gGraphic->edges[qNodes[i].edge]->query_frequency/sum;
				entropy_sum+=(-r*log(r)/log(2));
			}
			this->avg_entropy=entropy_sum;
		}
		
		if(VARIANCE){
			double variance_sum=0;
			double sum=0;
			for(i=0;i<qSize;i++){
				sum+=gGraphic->edges[qNodes[i].edge]->query_frequency;
			}
			sum/=qSize;
			for(i=0;i<qSize;i++){
				variance_sum+=pow(gGraphic->edges[qNodes[i].edge]->query_frequency-sum,2);
			}
			this->avg_variance=variance_sum/qSize;
		}
		Timer *querytimer = new Timer();
		querytimer->SC();
		vector<vector<POIL>/**/> results = gDataServer->query(qNodes);
		querytimer->EC();
		this->total_query_time+=querytimer->DC();
		return results[pos];
	}

	/*
	* query with traverse
	*/
	vector<POIL> QueryWithTraverse(QL& location) {
		int i,j;
		double fre = gGraphic->edges[location.edge]->query_frequency;
		vector<int> locations;
		int hSize = gTraverse->traverse_edges.size();
		for (i = 0; i < hSize; i++) {
			Edge* tEdge = gGraphic->edges[gTraverse->traverse_edges[i].first];
			double absfre=tEdge->query_frequency-fre;
			absfre=absfre>=0?absfre:-absfre;
			if (absfre <= THRESHOLD) {
				locations.push_back(tEdge->id);
			}
		}
		vector<QL> qNodes;
		QL clientQL(location.edge, location.ratio);
#ifdef _DEBUG_
		cout << "..." << endl;
		cout << "the " << K << " query locations are shown as follows :" << endl;
#ifdef _LOG_
		gLog->WriteString("...\nthe ");
		gLog->WriteInt(K);
		gLog->WriteString(" query locations are shown as follows :\n");
#endif
		clientQL.print();
#endif
		qNodes.push_back(clientQL);
		int lSize = locations.size();
		if (lSize < K) {
			for (i = 0; i < lSize; i++) {
				double r;
				clock_t st = clock();
				srand((unsigned)st);
				//check whether the edge contains historical locations
				if (gGraphic->cache_map.find(locations[i]) != gGraphic->cache_map.end()) {
					r = gGraphic->cache_map[locations[i]];
				}
				else {
					r = rand()*1.0 / RAND_MAX;
				}
				QL tQL(locations[i], r);
#ifdef _DEBUG_
				tQL.print();
#endif
				qNodes.push_back(tQL);
			}
		}
		else {
			int interval = lSize / (K - 1);
			for (i = 0; i < lSize; i += interval) {
				double r;
				clock_t st = clock();
				srand((unsigned)st);
				//check whether the edge contains historical locations
				if (gGraphic->cache_map.find(locations[i]) != gGraphic->cache_map.end()) {
					r = gGraphic->cache_map[locations[i]];
				}
				else {
					r = rand()*1.0 / RAND_MAX;
				}
				QL tQL(locations[i], r);
#ifdef _DEBUG_
				tQL.print();
#endif
				qNodes.push_back(tQL);
			}
		}
#ifdef _DEBUG_
		cout << "..." << endl;
#ifdef _LOG_
		gLog->WriteString("...\n");
#endif
#endif
		int qSize = qNodes.size();
		int pos = rand() % qSize;

		//mix the true location and the henerated locations, put the true location to a random postion
		int e;
		e=qNodes[0].edge;
		qNodes[0].edge = qNodes[pos].edge;
		qNodes[pos].edge= e;
		
		double r;

		r=qNodes[0].ratio ;
		qNodes[0].ratio = qNodes[pos].ratio;
		qNodes[pos].ratio =r;
		
		if(DISTANCE){
			double distance_sum=0;
			for(i=0;i<qSize;i++){
				distance_sum+=gGraphic->NetworkDistance(qNodes[i].edge,qNodes[i].ratio,qNodes[pos].edge,qNodes[pos].ratio);
			}
			this->avg_distance=distance_sum/qSize;
		}
		if(ENTROPY){
			double entropy_sum=0;
			double sum=0;
			for(i=0;i<qSize;i++){
				sum+=gGraphic->edges[qNodes[i].edge]->query_frequency;
			}
			for(i=0;i<qSize;i++){
				double r=gGraphic->edges[qNodes[i].edge]->query_frequency/sum;
				entropy_sum+=(-r*log(r)/log(2));
			}
			this->avg_entropy=entropy_sum;
		}
		
		if(VARIANCE){
			double variance_sum=0;
			double sum=0;
			for(i=0;i<qSize;i++){
				sum+=gGraphic->edges[qNodes[i].edge]->query_frequency;
			}
			sum/=qSize;
			for(i=0;i<qSize;i++){
				variance_sum+=pow(gGraphic->edges[qNodes[i].edge]->query_frequency-sum,2);
			}
			this->avg_variance=variance_sum/qSize;
		}
		Timer *querytimer = new Timer();
		querytimer->SC();
		vector<vector<POIL>/**/> results = gDataServer->query(qNodes);
		querytimer->EC();
		this->total_query_time+=querytimer->DC();
		return results[pos];
	}
	
	/*
	* query with random
	*/
	vector<POIL>/**/ QueryWithRandom(QL& location) {
		int i,j;
		double fre = gGraphic->edges[location.edge]->query_frequency;
		vector<int> locations;
		int hSize = gTraverse->traverse_edges.size();
		for (i = 0; i < hSize; i++) {
			locations.push_back(gTraverse->traverse_edges[i].first);
		}
		vector<QL> qNodes;
		QL clientQL(location.edge, location.ratio);
#ifdef _DEBUG_
		cout << "..." << endl;
		cout << "the " << K << " query locations are shown as follows :" << endl;
#ifdef _LOG_
		gLog->WriteString("...\nthe ");
		gLog->WriteInt(K);
		gLog->WriteString(" query locations are shown as follows :\n");
#endif
		clientQL.print();
#endif
		qNodes.push_back(clientQL);
		int lSize = locations.size();
		if (lSize < K) {
			for (i = 0; i < lSize; i++) {
				double r;
				clock_t st = clock();
				srand((unsigned)st);
				//check whether the edge contains historical locations
				if (gGraphic->cache_map.find(locations[i]) != gGraphic->cache_map.end()) {
					r = gGraphic->cache_map[locations[i]];
				}
				else {
					r = rand()*1.0 / RAND_MAX;
				}
				QL tQL(locations[i], r);
#ifdef _DEBUG_
				tQL.print();
#endif
				qNodes.push_back(tQL);
			}
		}
		else {
			int interval = lSize / (K - 1);
			for (i = 0; i < lSize; i += interval) {
				double r;
				clock_t st = clock();
				srand((unsigned)st);
				//check whether the edge contains historical locations
				if (gGraphic->cache_map.find(locations[i]) != gGraphic->cache_map.end()) {
					r = gGraphic->cache_map[locations[i]];
				}
				else {
					r = rand()*1.0 / RAND_MAX;
				}
				QL tQL(locations[i], r);
#ifdef _DEBUG_
				tQL.print();
#endif
				qNodes.push_back(tQL);
			}
		}
#ifdef _DEBUG_
		cout << "..." << endl;
#ifdef _LOG_
		gLog->WriteString("...\n");
#endif
#endif
		int qSize = qNodes.size();
		int pos = rand() % qSize;

		//mix the true location and the henerated locations, put the true location to a random postion
		int e;
		e=qNodes[0].edge;
		qNodes[0].edge = qNodes[pos].edge;
		qNodes[pos].edge= e;
		
		double r;

		r=qNodes[0].ratio ;
		qNodes[0].ratio = qNodes[pos].ratio;
		qNodes[pos].ratio =r;
		
		if(DISTANCE){
			double distance_sum=0;
			for(i=0;i<qSize;i++){
				distance_sum+=gGraphic->NetworkDistance(qNodes[i].edge,qNodes[i].ratio,qNodes[pos].edge,qNodes[pos].ratio);
			}
			this->avg_distance=distance_sum/qSize;
		}
		if(ENTROPY){
			double entropy_sum=0;
			double sum=0;
			for(i=0;i<qSize;i++){
				sum+=gGraphic->edges[qNodes[i].edge]->query_frequency;
				//cout<<gGraphic->edges[qNodes[i].edge]->query_frequency<<endl;
			}
			for(i=0;i<qSize;i++){
				double r=gGraphic->edges[qNodes[i].edge]->query_frequency/sum;
				entropy_sum+=(-r*log(r)/log(2));
			}
			this->avg_entropy=entropy_sum;
		}
		
		if(VARIANCE){
			double variance_sum=0;
			double sum=0;
			for(i=0;i<qSize;i++){
				sum+=gGraphic->edges[qNodes[i].edge]->query_frequency;
			}
			sum/=qSize;
			for(i=0;i<qSize;i++){
				variance_sum+=pow(gGraphic->edges[qNodes[i].edge]->query_frequency-sum,2);
			}
			this->avg_variance=variance_sum/qSize;
		}
		Timer *querytimer = new Timer();
		querytimer->SC();
		vector<vector<POIL>/**/> results = gDataServer->query(qNodes);
		querytimer->EC();
		this->total_query_time+=querytimer->DC();
		return results[pos];
	}
};

AnonymousServer* gAnonymousServer = new AnonymousServer();

#endif
