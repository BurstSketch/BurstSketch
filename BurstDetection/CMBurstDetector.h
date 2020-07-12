#ifndef cmburstdetector_H
#define cmburstdetector_H
#include <bits/stdc++.h>
#include <queue>
#include "BOBHash32.h"
#include "param.h"
#include "Burst.h"
class CMsketch
{
	public:
	uint32_t* v;
	int size;
	BOBHash32* bobhash[hash_num];
	CMsketch(){};
	CMsketch(int _size)
	{
		size = _size;
		v = new uint32_t[size];
		memset(v, 0, size * sizeof(uint32_t));
		for(int i = 0; i < hash_num; i++)
			bobhash[i] = new BOBHash32(101 + i);
	}
	void clear()
	{
		memset(v, 0, size * sizeof(uint32_t));
	}
	void insert(uint64_t flow_id)
	{
		for(int i = 0; i < hash_num; i++)
			v[bobhash[i]->run((char *)&flow_id, 8) % size]++;
	}
	uint32_t query(uint64_t flow_id)
	{
		uint32_t ret = oo;
		for(int i = 0; i < hash_num; i++)
			ret = min(ret, v[bobhash[i]->run((char *)&flow_id, 8) % size]);
		return ret;
	}
};
/*
class BF
{
	public:
	int size;
	bool* c;
	BOBHash32* bobhash[hash_num];
	BF(){};
	BF(int _size)
	{
		size = _size;
		c = new bool[size];
		for(int i = 0; i < size; i++)
			c[i] = 0;
		for(int i = 0; i < hash_num; i++)
			bobhash[i] = new BOBHash32(i + 97);
	}
	void clear()
	{
		for(int i = 0; i < size; i++)
			c[i] = 0;
	}
	void insert(uint64_t flow_id)
	{
		for(int i = 0; i < hash_num; i++)
			c[bobhash[i]->run((char *)&flow_id, 8) % size] = 1;
	}
	bool query(uint64_t flow_id)
	{
		bool ret = 1;
		for(int i = 0; i < hash_num; i++)
			ret &= c[bobhash[i]->run((char *)&flow_id, 8) % size];
		return ret;
	}
};
*/
class CMBurstDetector
{
	public:
	int size, now, m;
	CMsketch* CM[window_num + 2];
	uint64_t last_timestamp;
	vector<Burst> Record;
	queue<pair<uint64_t, uint32_t> > Q;
	CMBurstDetector(){};
	CMBurstDetector(int _size, int _m)
	{
		now = 0;
		m = _m;
		size = _size;
		last_timestamp = 0;
		for(int i = 0; i < window_num + 2; i++)
			CM[i] = new CMsketch(size);
		while(!Q.empty())
			Q.pop();
	}
	void update()
	{
		now = (now + 1) % (window_num + 2);
		CM[now]->clear();
	}
	void detect()
	{
		static uint32_t w[window_num + 1];
		while(!Q.empty())
		{
			uint64_t id = Q.front().first;
			uint32_t time = Q.front().second;
			if(last_timestamp - time < window_num)
				break;
			Q.pop();
			for(int i = 0; i <= last_timestamp - time + 1; i++)
				w[i] = CM[(now - last_timestamp + time - 1 + i + window_num + 2) % (window_num + 2)]->query(id);
			if(w[1] < w[0] * 2)
				continue;
			for(int i = 2; i <= last_timestamp - time + 1; i++)
			{
				if(w[i] >= w[i - 1] * 2)
					break;
				if(2 * w[i] <= w[i - 1])
				{
					Record.push_back(Burst(time, time + i - 1, id));
					break;
				}
				if(w[i] < m)
					break;
			}	
		}
	}
	void insert(uint64_t id, uint32_t timestamp)
	{
		if(last_timestamp < timestamp)
		{
			detect();
			update();
			last_timestamp = timestamp;
		}
		CM[now]->insert(id);
		uint32_t w0 = CM[(now + window_num + 1) % (window_num + 2)]->query(id), w1 = CM[now]->query(id);
		if(w1 == (uint32_t) m)
			Q.push(make_pair(id, timestamp));
	}
};
#endif
