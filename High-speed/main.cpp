#include<bits/stdc++.h>
#include "BurstDetector.h"
#include "CorrectBurstDetector.h"
using namespace std;
map<uint64_t, int> D;
vector<pair<int, int> > S[2000000];
map<uint64_t, uint32_t> M;
int w = 0;
int main()
{
	int mem = 60; // the size of memory
	double l = 0.3; // the ratio of the Running Track threshold to the burst threshold
	double r12 = 3.75; // the ratio of the size of Stage 1 to the size of Stage 2
	int screen_layer_threshold = l * threshold; // Running Track threshold
	int log_size = mem * 1024 / (12 * r12 + 20) / bucket_size; // number of buckets in Stage 2
	int screen_layer_size = log_size * r12 * bucket_size; // number of buckets in Stage 1
	int cm_size = mem * 1024 / (window_num + 2) / 4; // the size of CM Sketch
	BurstDetector A(screen_layer_size, screen_layer_threshold, log_size, threshold);
	CorrectBurstDetector B(threshold);
	uint64_t tmp;
	int window = 0, cnt = 0;
	int total = 0, correct = 0;
	double are = 0;
	for(int i = 0; i < 20000000; i++)
	{
		uint64_t tim, id; // caida
		fin.read((char *)&tim, sizeof(char) * 8);
		fin.read((char *)&id, sizeof(char) * 8);
		
		cnt++;
        if(cnt > 40000)
        {
        	cnt = 0;
        	window++;
        }
        A.insert(id, window);
        B.insert(id, window);
        // calculate
        if(cnt == 0)
        {
        	M.clear();
        	total += B.V.size();
        	correct += A.log.V.size();
        	for(int j = 0; j < A.log.V.size(); j++)
        		M[A.log.V[j].first] = A.log.V[j].second;
        	for(int j = 0; j < B.V.size(); j++)
				are += 1.0 * ((int) B.V[j].second - (int) M[B.V[j].first]) / B.V[j].second;
        }
	}
	// output results
	are /= total;
	printf("Recall: %.5lf\n", 100.0 * correct / total);
	printf("ARE: %.5lf\n", are);
	
	return 0;
}
