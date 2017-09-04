#ifndef _TRAVERSE_H_
#define _TRAVERSE_H_

#include "Head.h"
#include "Graphic.h"
#include "Log.h"

class Traverse {
public:
	vector<pair<int, int>/**/> traverse_edges;

	void print() {
		int eSize = this->traverse_edges.size();
		for (int i = 0; i < eSize; i++) {
			cout <<"("<< this->traverse_edges[i].first << "," << this->traverse_edges[i].second << ")\t";
#ifdef _LOG_
			gLog->WriteChar('(');
			gLog->WriteInt(this->traverse_edges[i].first);
			gLog->WriteChar(',');
			gLog->WriteInt(this->traverse_edges[i].second);
			gLog->WriteString(")\t");
#endif
		}
		cout << endl;
#ifdef _LOG_
		gLog->WriteChar('\n');
#endif
	}
	
	/*
	* sort the edges randomly
	*/
	void RandomTraverse(){
		vector<int> tedges;
		int i;
		int c=0;
		int eSize=gGraphic->edges.size();
		for(i=0;i<eSize;i++){
			tedges.push_back(i);
		}
		while(true){
			int eesize=tedges.size();
			if(eesize==0){
				break;
			}
			clock_t st = clock();
			srand((unsigned)st);
			long a=(long)rand()<<16;
			st = clock();
			srand((unsigned)st);
			long b=rand();
			int s = (a | b) % eesize;
			this->traverse_edges.push_back(make_pair(tedges[s],c++));
			tedges.erase(tedges.begin()+s);
		}
#ifdef _DEBUG_
		cout << "construct the random order successfully..." << endl;
#ifdef _LOG_
		gLog->WriteString("construct the random order successfully...\n");
#endif
#endif
	}

	/*
	* depth first traverse order
	*/
	void DepthFirstTraverse() {
		int i;
		int edgeSize = gGraphic->edges.size();
		clock_t st = clock();
		srand((unsigned)st);
		long a=(long)rand()<<16;
		st = clock();
		srand((unsigned)st);
		long b=rand();
		int s = (a | b) % edgeSize;
		bool *visited = new bool[edgeSize];
		memset(visited, 0, edgeSize);
		bool *puted = new bool[edgeSize];
		memset(puted, 0, edgeSize);
		stack<pair<int, bool>/**/> mqueue;
		mqueue.push(make_pair(s, true));
		visited[s] = true;
		int count = 0;
		while (!mqueue.empty()) {
			pair<int, bool> top = mqueue.top();
			if (!puted[top.first]) {
				traverse_edges.push_back(make_pair(top.first, count++));
				puted[top.first] = true;
			}
			Node* tNode = top.second ? gGraphic->nodes[gGraphic->edges[top.first]->end] : gGraphic->nodes[gGraphic->edges[top.first]->start];
			int eSize = tNode->edges.size();
			for (i = 0; i < eSize; i++) {
				Edge* tEdge = gGraphic->edges[tNode->edges[i]];
				if (!visited[tEdge->id]) {
					if (tEdge->end == tNode->id) {
						mqueue.push(make_pair(tEdge->id, false));
						visited[tEdge->id] = true;
					}
					else {
						mqueue.push(make_pair(tEdge->id, true));
						visited[tEdge->id] = true;
					}
					break;
				}
			}
			if (i == eSize) {
				mqueue.pop();
			}
		}
#ifdef _DEBUG_
		cout << "construct the depth first order successfully..." << endl;
#ifdef _LOG_
		gLog->WriteString("construct the depth first order successfully...\n");
#endif
#endif
	}

	/*
	* breadth first traverse order
	*/
	void BreadthFirstTraverse() {
		int i;
		int edgeSize = gGraphic->edges.size();
		clock_t st = clock();
		srand((unsigned)st);
		long a=(long)rand()<<16;
		st = clock();
		srand((unsigned)st);
		long b=rand();
		int s = (a | b) % edgeSize;
		bool *visited = new bool[edgeSize];
		memset(visited, 0, edgeSize);
		queue<pair<int, bool>/**/> mqueue;
		mqueue.push(make_pair(s, true));
		visited[s] = true;
		int count = 0;
		while (!mqueue.empty()) {
			pair<int, bool> top = mqueue.front();
			mqueue.pop();
			traverse_edges.push_back(make_pair(top.first, count++));
			Node* tNode = top.second ? gGraphic->nodes[gGraphic->edges[top.first]->end] : gGraphic->nodes[gGraphic->edges[top.first]->start];
			int eSize = tNode->edges.size();
			for (i = 0; i < eSize; i++) {
				Edge* tEdge = gGraphic->edges[tNode->edges[i]];
				if (!visited[tEdge->id]) {
					if (tEdge->end == tNode->id) {
						mqueue.push(make_pair(tEdge->id, false));
					}
					else {
						mqueue.push(make_pair(tEdge->id, true));
					}
					visited[tEdge->id] = true;
				}
			}
		}
#ifdef _DEBUG_
		cout << "construct the breadth first order successfully..." << endl;
#ifdef _LOG_
		gLog->WriteString("construct the breadth first order successfully...\n");
#endif
#endif
	}
};


Traverse *gTraverse = new Traverse();

#endif
