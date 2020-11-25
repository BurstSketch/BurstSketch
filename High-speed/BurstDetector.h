#ifndef burstdetector_H
#define burstdetector_H
#include<bits/stdc++.h>
#include "BOBHash32.h"
#include "param.h"
#include "Burst.h"
class ScreenLayer
{
	public:
	int size, m;
	uint32_t* counter;
	uint64_t* id;
	BOBHash32* bobhash[hash_num];
	ScreenLayer(){};
	ScreenLayer(int _size, int _m)
	{
		size = _size;
		m = _m;
		counter = new uint32_t[size];
		id = new uint64_t[size];
		for(int i = 0; i < size; i++)
		{
			id[i] = 0;
			counter[i] = 0;
		}
		for(int i = 0; i < hash_num; i++)
			bobhash[i] = new BOBHash32(i + 103);
	}
	void update()
	{
		for(int i = 0; i < size; i++)
		{
			counter[i] = 0;
			id[i] = 0;
		}
	}
	uint32_t lookup(int c)
	{
		return counter[c];
	}
	void clear(int c)
	{
		// the basic version 
		uint64_t flow_id = id[c];
		for(int i = 0; i < hash_num; i++)
		{
			int z = bobhash[i]->run((char *)&flow_id, 8) % size;
			if(id[z] == flow_id)
			{
				id[z] = 0;
				counter[z] = 0;
			}
		}
		/*
		// the optimized version
		counter[c] = 0;
		id[c] = 0;
		*/
	}
	int insert(uint64_t flow_id)
	{
		int flag = 0, ret = -1;
		// the basic version
		for(int i = 0; i < hash_num; i++)
		{
			int z = bobhash[i]->run((char *)&flow_id, 8) % size;
			if(id[z] == flow_id)
			{
				counter[z]++;
				if(counter[z] >= m)
					ret = z;
			}
			else if(id[z] == 0)
			{
				counter[z]++;
				id[z] = flow_id;
			}
			else
			{
				counter[z]--;
				if(counter[z] == 0)
					id[z] = 0;
			}
		}
		  /*
		// the optimized version
		for(int i = 0; i < hash_num; i++)
		{
			int z = bobhash[i]->run((char *)&flow_id, 8) % size;
			if(id[z] == flow_id)
				flag |= 1;
			else if(id[z] == 0)
				flag |= 2;
		}
		if(flag & 1)
		{
			for(int i = 0; i < hash_num; i++)
			{
				int z = bobhash[i]->run((char *)&flow_id, 8) % size;
				if(id[z] == flow_id)
				{
					counter[z]++;
					if(counter[z] >= m)
						ret = z;
				}
			}
		}
		else if(flag & 2)
		{
			for(int i = 0; i < hash_num; i++)
			{
				int z = bobhash[i]->run((char *)&flow_id, 8) % size;
				if(id[z] == 0)
				{
					counter[z] = 1;
					id[z] = flow_id;
					break;
				}
			}
		}
		else
		{
			for(int i = 0; i < hash_num; i++)
			{
				int z = bobhash[i]->run((char *)&flow_id, 8) % size;
					counter[z]--;
				if(counter[z] == 0)
					id[z] = 0;
			}
		}
		*/
		return ret;
	}
};
class Log // Stage 2
{
	public:
	BOBHash32* bobhash;
	bool flag;
	int size, m, screen_layer_threshold;
	uint32_t** counter[2];
	uint64_t** id;
	uint32_t** timestamp;
	vector<Burst> Record;
	vector<pair<uint64_t, uint32_t> > V;
	Log(){};
	Log(int _size,int _m, int _screen_layer_threshold)
	{
		flag = 0;
		size = _size;
		m = _m;
		screen_layer_threshold = _screen_layer_threshold;
		Record.clear();
		counter[0] = (uint32_t **)new uint32_t*[size];
		counter[1] = (uint32_t **)new uint32_t*[size];
		id = (uint64_t **)new uint64_t*[size];
		timestamp = (uint32_t **)new uint32_t*[size];
		for(int i = 0; i < size; i++)
		{
			counter[0][i] = new uint32_t[bucket_size];
			counter[1][i] = new uint32_t[bucket_size];
			id[i] = new uint64_t[bucket_size];
			timestamp[i] = new uint32_t[bucket_size];
		}
		for(int i = 0; i < size; i++)
			for(int j = 0; j < bucket_size; j++)
			{
				counter[0][i][j] = 0;
				counter[1][i][j] = 0;
				id[i][j] = 0;
				timestamp[i][j] = -1;
			}
		bobhash = new BOBHash32(1005);
	}
	void update(uint32_t time)
	{
		// clear
		V.clear();
		for(int i = 0; i < size; i++)
			for(int j = 0; j < bucket_size; j++)
			{
				if(id[i][j] != 0 && timestamp[i][j] != -1 && time - timestamp[i][j] > window_num)
				{
					counter[0][i][j] = 0;
					counter[1][i][j] = 0;
					id[i][j] = 0;
					timestamp[i][j] = -1;
				}
				if(id[i][j] != 0 && counter[0][i][j] < screen_layer_threshold && counter[1][i][j] < screen_layer_threshold)
				{
					counter[0][i][j] = 0;
					counter[1][i][j] = 0;
					id[i][j] = 0;
					timestamp[i][j] = -1;
				}
			} 
		// find burst
		for(int i = 0; i < size; i++)
			for(int j = 0; j < bucket_size; j++)
			{
				if(id[i][j] == 0)
					continue;
				if(counter[flag][i][j] <= counter[flag ^ 1][i][j] / 2 && timestamp[i][j] != -1)
				{
					//output burst
					Record.push_back(Burst(timestamp[i][j], time, id[i][j]));
					timestamp[i][j] = -1;
				}
				else if(counter[flag][i][j] < m)
					timestamp[i][j] = -1;
				else if(counter[flag][i][j] >= 2 * counter[flag ^ 1][i][j] && counter[flag][i][j] >= m)
					timestamp[i][j] = time;
				if(counter[flag][i][j] >= m)
					V.push_back(make_pair(id[i][j], counter[flag][i][j])); // output high-speed item
			}
		flag ^= 1;
		for(int i = 0; i < size; i++)
			for(int j = 0; j < bucket_size; j++)
				counter[flag][i][j] = 0;
	}
	bool lookup(uint64_t flow_id, uint32_t flow_time)
	{	
		int z = bobhash->run((char *)&flow_id, 8) % size;
		for(int j = 0; j < bucket_size; j++)
			if(id[z][j] == flow_id)
			{
				counter[flag][z][j]++;
				return true;
			}
		return false;
	}
	bool insert(uint64_t flow_id, uint32_t flow_time, uint32_t flow_count)
	{
		uint32_t mi = oo, l = 0, f = 0;
		int z = bobhash->run((char *)&flow_id, 8) % size;
		for(int j = 0; j < bucket_size; j++)
		{
			if(id[z][j] == 0)
			{
				l = j;
				break;
			}
			if(timestamp[z][j] == -1)
			{ 
				if(f == 0)
				{
					f = 1;
					mi = oo;
				}
				if(counter[flag][z][j] < mi)
				{
					mi = counter[flag][z][j];
					l = j;
				}
			}
			else if(!f)
			{
				if(counter[flag][z][j] < mi)
				{
					mi = counter[flag][z][j];
					l = j;
				}
			}
		}
		if(id[z][l] == 0)
		{
			id[z][l] = flow_id;
			timestamp[z][l] = -1;
			counter[flag][z][l] = flow_count; // evict a flow!
			counter[flag ^ 1][z][l] = 0;
			return true;
		}
		if(flow_count > counter[flag][z][l])
		{
			id[z][l] = flow_id;
			timestamp[z][l] = -1;
			counter[flag][z][l] = flow_count; // evict a flow!
			counter[flag ^ 1][z][l] = 0;
			return true;
		}
		return false;
	}
};
class BurstDetector // BurstSketch
{
	public:
	ScreenLayer screen_layer;
	uint64_t last_timestamp;
	Log log;
	BurstDetector(){};
	BurstDetector(int ScreenLayerSize, int ScreenLayerThreshold, int LogSize, int LogThreshold)
	{
		last_timestamp = 0;
		screen_layer = ScreenLayer(ScreenLayerSize, ScreenLayerThreshold);
		log = Log(LogSize, LogThreshold, ScreenLayerThreshold);
	}
	void insert(uint64_t id, uint32_t timestamp)
	{
		if(last_timestamp < timestamp)
		{
			for(uint32_t i = last_timestamp; i < timestamp; i++)
			{
				screen_layer.update();
				log.update(i);
			}
			last_timestamp = timestamp;
		}
		if(log.lookup(id, timestamp))
			return ;
		int ret = screen_layer.insert(id);
		if(ret == -1)
			return ;
		uint32_t count = screen_layer.lookup(ret);
		if(log.insert(id, timestamp, count))
			screen_layer.clear(ret);
	}
};
#endif
