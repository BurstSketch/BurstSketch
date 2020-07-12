#include<bits/stdc++.h>
#include "BurstDetector.h"
#include "CorrectBurstDetector.h"
#include "CMBurstDetector.h"
using namespace std;
map<uint64_t, int> D;
vector<pair<int, int> > S[2000000];
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
	CMBurstDetector C(cm_size, threshold);
	uint64_t tmp;
	int window = 0, cnt = 0;
	for(int i = 0; i < 20000000; i++)
	{
		
		uint64_t tim, id;
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
        C.insert(id, window); 
	}
	
	printf("Totally %d distinct flows\n", B.w);
	printf("BurstSketch totally reports %d bursts!\n", A.log.Record.size());
	printf("CM Burst detector totally reports %d bursts!\n", C.Record.size());
	printf("Actually there exist %d bursts!\n", B.Record.size());
	
	D.clear();
	for(int i = 0; i < B.Record.size(); i++)
	{
		if(D.find(B.Record[i].flow_id) == D.end())
			D[B.Record[i].flow_id] = w++;
		S[D[B.Record[i].flow_id]].push_back(make_pair(B.Record[i].start_window, B.Record[i].end_window));
	}
	int bd_correct = 0, bd_overlap = 0, bd_error = 0;
	int cm_correct = 0, cm_overlap = 0, cm_error = 0;
	// calculate experimental results
	for(int i = 0; i < A.log.Record.size(); i++)
	{
		if(D.find(A.log.Record[i].flow_id) == D.end())
			continue;
		int z = D[A.log.Record[i].flow_id];
		for(int j = 0; j < S[z].size(); j++)
		{
			if(A.log.Record[i].start_window == S[z][j].first && A.log.Record[i].end_window == S[z][j].second)
			{
				bd_correct++;
				break;
			}
			if(A.log.Record[i].start_window <= S[z][j].second && A.log.Record[i].end_window >= S[z][j].first)
			{
				bd_overlap++;
				bd_error += abs((S[z][j].second - S[z][j].first) - ((int)A.log.Record[i].end_window - (int)A.log.Record[i].start_window));
				break;
			}	
		}
	}
	
	for(int i = 0; i < C.Record.size(); i++)
	{
		if(D.find(C.Record[i].flow_id) == D.end())
			continue;
		int z = D[C.Record[i].flow_id];
		for(int j = 0; j < S[z].size(); j++)
		{
			if(C.Record[i].start_window == S[z][j].first && C.Record[i].end_window == S[z][j].second)
			{
				cm_correct++;
				break;
			}
			if(C.Record[i].start_window <= S[z][j].second && C.Record[i].end_window >= S[z][j].first)
			{
				cm_overlap++;	
				cm_error += abs((S[z][j].second - S[z][j].first) - ((int)C.Record[i].end_window - (int)C.Record[i].start_window));
				break;
			}
			
		}
	}
	// output BurstSketch results
	printf("BurstSketch:\n");
	printf("Precision: %.5lf\n", 100.0 * (bd_correct + bd_overlap) / A.log.Record.size());
	printf("Recall: %.5lf\n", 100.0 *(bd_correct + bd_overlap) / B.Record.size());
	printf("Average deviation: %.5lf\n", 1.0 * bd_error / (bd_correct + bd_overlap));
	printf("Deviation rate: %.5lf\n\n", 100.0 * bd_overlap / (bd_correct + bd_overlap));

	// output the strawman solution's results
	printf("The strawman solution:\n");
	printf("Precision: %.5lf\n", 100.0 * (cm_correct + cm_overlap) / C.Record.size());
	printf("Recall: %.5lf\n", 100.0 *(cm_correct + cm_overlap) / B.Record.size());
	printf("Average deviation: %.5lf\n", 1.0 * cm_error / (cm_correct + cm_overlap));
	printf("Deviation rate: %.5lf\n\n", 100.0 * cm_overlap / (cm_correct + cm_overlap));
	return 0;
}
