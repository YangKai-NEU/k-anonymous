#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "Head.h"
#include "Log.h"
#include "Util2D.h"

struct Compare {
	bool operator()(const pair<int, double> a, const pair<int, double> b) {
		return a.second > b.second;
	}
};

struct Compare2 {
	bool operator()(const pair<int, pair<double, bool>/**/> a, const pair<int, pair<double, bool>/**/> b) {
		return a.second.first > b.second.first;
	}
};

struct Node {
public:
	int id;
	double lat;
	double lng;
	bool is_poi;
	vector<int> edges;
	vector<pair<int,int>/**/> voronoi_edges;
	
	Node(int id, double lat, double lng) {
		this->id = id;
		this->lat = lat;
		this->lng = lng;
		this->is_poi = false;
	}
	
	void print() {
		cout << "node " << id << " : (" << lat << "," << lng << ")" << endl;
#ifdef _LOG_
		gLog->WriteString("node ");
		gLog->WriteInt(id);
		gLog->WriteString(" : (");
		gLog->WriteDouble(lat);
		gLog->WriteChar(',');
		gLog->WriteDouble(lng);
		gLog->WriteString(")\n");
#endif
	}
};

struct Edge {
public:
	int id;
	int start;
	int end;

	//latitude, longitude of the center point in this edge
	pair<double, double> center;

	//the query frequency in this edge
	double query_frequency;		

	pair<pair<int, int>, double> poi_nodes;
	bool is_divided;
	double length;

	Edge(int id, int start, int end, double len) {
		this->id = id;
		this->start = start;
		this->end = end;
		this->length = len;
		this->query_frequency = 0;
		this->is_divided = false;
		this->poi_nodes = make_pair(make_pair(-1, -1), -1);
	}
	
	void print() {
		cout << "edge " << id << " : " << start << "->" << end << " ,length=" << length << endl;
#ifdef _LOG_
		gLog->WriteString("edge ");
		gLog->WriteInt(id);
		gLog->WriteString(" : ");
		gLog->WriteInt(start);
		gLog->WriteString("->");
		gLog->WriteInt(end);
		gLog->WriteString(" ,length=");
		gLog->WriteDouble(length);
		gLog->WriteChar('\n');
#endif
	}
};

class Graphic {
public:
	vector<Node*> nodes;
	vector<Edge*> edges;
	map<int, double> cache_map;

	/*
	* Load the data of graphic from file, construct graphic from file
	*/
	void CGFF(string nodesFilePath, string edgesFilePath) {

		int id;
		double lat, lng;
		int start, end;
		int i;

		FILE* nodeFile = fopen(nodesFilePath.c_str(), "r");
		FILE* edgeFile = fopen(edgesFilePath.c_str(), "r");

		// loading nodes information from file
		while ((fscanf(nodeFile, "%d %lf %lf\n", &id, &lat, &lng)) != EOF) {
			nodes.push_back(new Node(id, lat, lng));
		}

		// loading edges information from file
		while ((fscanf(edgeFile, "%d %d %d\n", &id, &start, &end)) != EOF) {
			edges.push_back(new Edge(id, start, end, Util2D::ED(nodes[start]->lat, nodes[start]->lng, nodes[end]->lat, nodes[end]->lng)));
			nodes[start]->edges.push_back(id);
			nodes[end]->edges.push_back(id);
		}
		fclose(nodeFile);
		fclose(edgeFile);

		// calculate the center point of edges in road network
		for (i = 0; i < edges.size(); i++) {
			edges[i]->center.first = (nodes[edges[i]->start]->lat + nodes[edges[i]->end]->lat) / 2;
			edges[i]->center.second = (nodes[edges[i]->start]->lng + nodes[edges[i]->end]->lng) / 2;
		}

#ifdef _DEBUG_
		cout << "construct the road network successfully..." << endl;
#ifdef _LOG_
		gLog->WriteString("construct the road network successfully...\n");
#endif
#endif // _DEBUG_

	}

	/*
	* frequency of the road network, construct frequency randomly
	*/
	void CF() {
		int i;
		int edgeSize = edges.size();
		for (i = 0; i < GFREQUENCY; i++) {
			clock_t st = clock();
			srand((unsigned)st);
			long a=(long)rand()<<16;
			st = clock();
			srand((unsigned)st);
			long b=rand();
			int e = (a | b) % edgeSize;
			edges[e]->query_frequency++;
			st = clock();
			srand((unsigned)st);
			double r = rand()*1.0 / RAND_MAX;
			cache_map[e] = r;
		}
#ifdef _DEBUG_
		int maxF = -1;
		int a[10000] = { 0 };
		for (i = 0; i < edgeSize; i++) {
			a[edges[i]->query_frequency]++;
		}
		for (i = 0; i < edgeSize; i++) {
			maxF = maxF > edges[i]->query_frequency ? maxF : edges[i]->query_frequency;
		}
		i = 0;
		while (a[i] == 0) {
			i++;
		}
		cout << "..." << endl << "frequency of edges is shown as follows :" << endl;
#ifdef _LOG_
		gLog->WriteString("...\nfrequency of edges is shown as follows :\n");
#endif
		for (; i <= maxF; i++) {
			cout << i << ": " << a[i] << "\t";
#ifdef _LOG_
			gLog->WriteInt(i);
			gLog->WriteString(": ");
			gLog->WriteInt(a[i]);
			gLog->WriteChar('\t');
#endif
		}
		cout << endl << "..." << endl;
		cout << "generate the frequency for every edge successfully..." << endl;
#ifdef _LOG_
		gLog->WriteString("\n...\n");
		gLog->WriteString("generate the frequency for every edge successfully...\n");
#endif
#endif
	}

	/*
	* make POIs randomly
	*/
	void MPOI() {
		int i;
		int nodeSize = nodes.size();
		for (i = 0; i < POIN; i++) {
			clock_t st = clock();
			srand((unsigned)st);
			long a=(long)rand()<<16;
			st = clock();
			srand((unsigned)st);
			long b=rand();
			int e = (a | b) % nodeSize;
			nodes[e]->is_poi = true;
		}
#ifdef _DEBUG_
		cout << "generate the POIs in road network successfully..." << endl;
#ifdef _LOG_
		gLog->WriteString("generate the POIs in road network successfully...\n");
#endif
#endif
	}

	/*
	* make network-based voronoi diagram
	*/
	void MNVD() {
		int i, j;
		int nodeSize = nodes.size();
		int edgeSize = edges.size();
		bool *visited = new bool[edgeSize];
		pair<int, double> *dp = new pair<int, double>[nodeSize];
		for (i = 0; i < nodeSize; i++) {
			if (nodes[i]->is_poi) {
				dp[i].first = i;
				dp[i].second = 0;
				continue;
			}
			memset(visited, 0, edgeSize);
			priority_queue<pair<int, double>, vector<pair<int, double>/**/>, Compare> queue;
			int oedgeSize = nodes[i]->edges.size();
			for (j = 0; j < oedgeSize; j++) {
				Edge* tEdge = edges[nodes[i]->edges[j]];
				int nid = tEdge->start == i ? tEdge->end : tEdge->start;
				queue.push(make_pair(nid, tEdge->length));
				visited[tEdge->id] = true;
			}
			while (!queue.empty()) {
				pair<int, double> top = queue.top();
				queue.pop();
				if (nodes[top.first]->is_poi) {
					dp[i].first = top.first;
					dp[i].second = top.second;
					break;
				}
				oedgeSize = nodes[top.first]->edges.size();
				for (j = 0; j < oedgeSize; j++) {
					Edge* tEdge = edges[nodes[top.first]->edges[j]];
					if (!visited[tEdge->id]) {
						int nid = tEdge->start == top.first ? tEdge->end : tEdge->start;
						queue.push(make_pair(nid, tEdge->length + top.second));
						visited[tEdge->id] = true;
					}
				}
			}
		}

		for (i = 0; i < edgeSize; i++) {
			Edge* tEdge = edges[i];
			if (dp[tEdge->start].first == dp[tEdge->end].first) {
				tEdge->is_divided = false;
				tEdge->poi_nodes.first = make_pair(dp[tEdge->start].first, dp[tEdge->start].first);
				tEdge->poi_nodes.second = -1;
				nodes[dp[tEdge->start].first]->voronoi_edges.push_back(make_pair(i,3));
			}
			else {
				tEdge->is_divided = true;
				tEdge->poi_nodes.first = make_pair(dp[tEdge->start].first, dp[tEdge->end].first);
				nodes[dp[tEdge->start].first]->voronoi_edges.push_back(make_pair(i,1));
				nodes[dp[tEdge->end].first]->voronoi_edges.push_back(make_pair(i,2));
				double sub = dp[tEdge->start].second - dp[tEdge->end].second;
				double abssub = sub>0?sub:-sub;
				double avg = (tEdge->length - abssub) / 2;
				tEdge->poi_nodes.second = (sub > 0 ? avg : avg + abssub) / tEdge->length;
			}
		}
#ifdef _DEBUG_
		cout << "construct the voronoi diagram successfully..." << endl;
#ifdef _LOG_
		gLog->WriteString("construct the voronoi diagram successfully...\n");
#endif
#endif
	}

	/*
	* road network distance of two location
	*/
	double NetworkDistance(int edge1,double ratio1,int edge2,double ratio2){
		double result = 0;
		int edgeSize = this->edges.size();
		bool *visited = new bool[edgeSize];
		memset(visited, 0, edgeSize);
		priority_queue<pair<int, pair<double, bool>/**/>,vector<pair<int, pair<double, bool>/**/>/**/>, Compare2> queue;
		int i;
		Edge *s = this->edges[edge1];
		Edge *e = this->edges[edge2];
		if(edge1==edge2){
			double dis=s->length*(ratio1-ratio2);
			delete visited;
			return dis>0?dis:-dis;
		}
		queue.push(make_pair(edge1, make_pair(s->length*(1-ratio1), true)));
		queue.push(make_pair(edge1, make_pair(s->length*ratio1, false)));
		visited[edge1] = true;
		while (!queue.empty()) {
			pair<int, pair<double, bool>/**/> top = queue.top();
			queue.pop();
			if (top.first == edge2) {
				if (top.second.second) {
					result = top.second.first - e->length * (1 - ratio2);
				}else {
					result = top.second.first - e->length * ratio2;
				}
				break;
			}
			Edge *tEdge = this->edges[top.first];
			Node *tNode;
			if (top.second.second) {
				tNode = this->nodes[tEdge->end];
			}else {
				tNode = this->nodes[tEdge->start];
			}
			int teSize = tNode->edges.size();
			for (i = 0; i < teSize; i++) {
				Edge* teEdge = this->edges[tNode->edges[i]];
				if (!visited[teEdge->id]) {
					visited[teEdge->id] = true;
					if (teEdge->start == tNode->id) {
						queue.push(make_pair(teEdge->id, make_pair(top.second.first + teEdge->length, true)));
					}else {
						queue.push(make_pair(teEdge->id, make_pair(top.second.first + teEdge->length, false)));
					}
				}
			}
		}
		delete visited;
		return result;
	}

	/*
	* euclidean distance between two edges
	*/
	double EDInEdge(int edge1,double ratio1,int edge2,double ratio2){
		Edge *tEdge1=this->edges[edge1];
		Edge *tEdge2=this->edges[edge2];
		Node *node1=this->nodes[tEdge1->start];
		Node *node2=this->nodes[tEdge1->end];
		Node *node3=this->nodes[tEdge2->start];
		Node *node4=this->nodes[tEdge2->end];
		double x1=node1->lat+(node2->lat-node1->lat)*ratio1;
		double y1=node1->lng+(node2->lng-node1->lng)*ratio1;
		double x2=node3->lat+(node4->lat-node3->lat)*ratio2;
		double y2=node3->lng+(node4->lng-node3->lng)*ratio2;
		return Util2D::ED(x1,y1,x2,y2);
	}
};

Graphic* gGraphic = new Graphic();

#endif
