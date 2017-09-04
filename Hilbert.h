#ifndef _HILBERT_H_
#define _HILBERT_H_

#include "Head.h"
#include "Log.h"
#include "Graphic.h"

const double MAX_RANGE = 1000000;

bool compare(pair<int, int> a, pair<int, int> b) {
	return a.second < b.second;
}

class Hilbert {
public:
	double left;
	double right;
	double top;
	double down;
	
	// the height and width of the grid, that is n*n
	long n;
	
	pair<char, char> hilbert_map[4][2][2];
	vector<pair<int, int>/**/> hilbert_POIs;

	Hilbert() {
		this->left = MAX_RANGE;
		this->right = -MAX_RANGE;
		this->top = -MAX_RANGE;
		this->down = MAX_RANGE;
		
		hilbert_map[0][0][0] = make_pair(0, 3);
		hilbert_map[0][0][1] = make_pair(1, 0);
		hilbert_map[0][1][0] = make_pair(3, 1);
		hilbert_map[0][1][1] = make_pair(2, 0);

		hilbert_map[1][0][0] = make_pair(2, 1);
		hilbert_map[1][0][1] = make_pair(1, 1);
		hilbert_map[1][1][0] = make_pair(3, 0);
		hilbert_map[1][1][1] = make_pair(0, 2);

		hilbert_map[2][0][0] = make_pair(2, 2);
		hilbert_map[2][0][1] = make_pair(3, 3);
		hilbert_map[2][1][0] = make_pair(1, 2);
		hilbert_map[2][1][1] = make_pair(0, 1);

		hilbert_map[3][0][0] = make_pair(0, 0);
		hilbert_map[3][0][1] = make_pair(3, 2);
		hilbert_map[3][1][0] = make_pair(1, 3);
		hilbert_map[3][1][1] = make_pair(2, 3);
	}

	/*
	* update the grid range according the road network, init the range of grid with road network
	*/
	void IRWN() {
		int i;
		for (i = 0; i < gGraphic->nodes.size(); i++) {
			this->left = this->left < gGraphic->nodes[i]->lng ? this->left : gGraphic->nodes[i]->lng;
			this->right = this->right > gGraphic->nodes[i]->lng ? this->right : gGraphic->nodes[i]->lng;
			this->top = this->top > gGraphic->nodes[i]->lat ? this->top : gGraphic->nodes[i]->lat;
			this->down = this->down < gGraphic->nodes[i]->lat ? this->down : gGraphic->nodes[i]->lat;
		}
		this->left-=1;
		this->right+=1;
		this->top+=1;
		this->down-=1;
#ifdef _DEBUG_
		cout << "update the grid range successfully..." << endl;
#ifdef _LOG_
		gLog->WriteString("update the grid range successfully...\n");
#endif
#endif // _DEBUG_
	}

	/*
	* make partition and hilbert curve
	*/
	void MPHC() {
		int i;
		this->n=pow(2,GN);
		int nSize=gGraphic->nodes.size();
		double widthUnit=(right-left)/this->n;
		double heightUnit=(right-left)/this->n;
		for(i=0;i<nSize;i++){
			Node *tNode=gGraphic->nodes[i];
			if(tNode->is_poi){
				int xn=(int)((tNode->lng-left)/widthUnit);
				int yn=(int)((tNode->lat-down)/heightUnit);
				this->hilbert_POIs.push_back(make_pair(i,xn*this->n+yn));
			}
		}
		sort(this->hilbert_POIs.begin(),this->hilbert_POIs.end(),compare);
	}
	
	int xy2d(int x, int y, int order) {
		int current_square = 1;
		int position = 0;
		int quad_x = 0;
		int quad_y = 0;
		int quad_position = 0;
		for (int i = order - 1; i >= 0; i--) {
			position <<= 2;
			quad_x = (x & (1 << i)) > 0 ? 1 : 0;
			quad_y = (y & (1 << i)) > 0 ? 1 : 0;
			
			pair<int,int> p = hilbert_map[current_square][quad_x][quad_y];
			quad_position = p.first;
			current_square = p.second;
			position |= quad_position;
		}
		return position;
	}
};

Hilbert *gHilbert = new Hilbert();

#endif
