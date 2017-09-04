#ifndef _DATA_SERVER_H_
#define _DATA_SERVER_H_

#include "Head.h"
#include "Graphic.h"
#include "Log.h"

struct QL {
public:
	int edge;
	double ratio;

	QL(int e, double r) {
		this->edge = e;
		this->ratio = r;
	}

	void print() {
		Edge* tEdge = gGraphic->edges[edge];
		Node* start = gGraphic->nodes[tEdge->start];
		Node* end = gGraphic->nodes[tEdge->end];
		double x, y;
		x = start->lat + (end->lat - start->lat)*ratio;
		y = start->lng + (end->lng - start->lng)*ratio;
		cout << "query location : (" << x << "," << y << ")" << endl;
#ifdef _LOG_
		gLog->WriteString("query location : (");
		gLog->WriteDouble(x);
		gLog->WriteChar(',');
		gLog->WriteDouble(y);
		gLog->WriteString(")\n");
#endif
	}
};

struct POIL {
public:
	int id;
	double x, y;

	POIL() {}

	POIL(int id, double x, double y) {
		this->id = id;
		this->x = x;
		this->y = y;
	}

	void print() {
		cout << "the query result is shown as follows :" << endl;
		cout << "\tID=" << id << ", " << "x=" << x << ", " << "y=" << y << endl;
#ifdef _LOG_
		gLog->WriteString("the query result is shown as follows :\n");
		gLog->WriteString("\tID=");
		gLog->WriteInt(id);
		gLog->WriteString(", x=");
		gLog->WriteDouble(x);
		gLog->WriteString(", y=");
		gLog->WriteDouble(y);
		gLog->WriteChar('\n');
#endif
	}
};

class DataServer {
public:

	/*
	* query interface, get nearest POIs of the query points by voronoi diagram
	*/
	vector<vector<POIL>/**/> query(vector<QL> locations) {
		vector<vector<POIL>/**/> result;
		int lsize = locations.size();
		int nSize=gGraphic->nodes.size();
		int eSize=gGraphic->edges.size();
		for (int i = 0; i < lsize; i++) {
			queue<int> mqueue;
			vector<POIL> tresult;
			mqueue.push(locations[i].edge);
			vector<bool> puted(nSize,false);
			vector<bool> visited(eSize,false);
			visited[locations[i].edge]=true;
			while(!mqueue.empty()){
				int edge = mqueue.front();
				mqueue.pop();
				Edge* tEdge = gGraphic->edges[edge];
				
				if(gGraphic->nodes[tEdge->start]->is_poi){
					Node *t = gGraphic->nodes[tEdge->start];
					POIL tPOIL(t->id, t->lat, t->lng);
					if(!puted[t->id]){
						tresult.push_back(tPOIL);
						puted[t->id]=true;
						if(tresult.size()>=KNN){
							break;
						}
					}
				}
				if(gGraphic->nodes[tEdge->end]->is_poi){
					Node *t = gGraphic->nodes[tEdge->end];
					POIL tPOIL(t->id, t->lat, t->lng);
					if(!puted[t->id]){
						tresult.push_back(tPOIL);
						puted[t->id]=true;
						if(tresult.size()>=KNN){
							break;
						}
					}
				}
				
				Node* tStart=gGraphic->nodes[tEdge->start];
				for(int j=0;j<tStart->edges.size();j++){
					if(!visited[tStart->edges[j]]){
						mqueue.push(tStart->edges[j]);
						visited[tStart->edges[j]]=true;
					}
				}
				
				Node* tEnd=gGraphic->nodes[tEdge->end];
				for(int j=0;j<tEnd->edges.size();j++){
					if(!visited[tEnd->edges[j]]){
						mqueue.push(tEnd->edges[j]);
						visited[tEnd->edges[j]]=true;
					}
				}
			}
			result.push_back(tresult);
		}
		return result;
	}
};

DataServer *gDataServer = new DataServer();

#endif
