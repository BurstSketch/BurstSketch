#ifndef burst_H
#define burst_H
#include<bits/stdc++.h>
class Burst
{
	public:
	uint32_t start_window;
	uint32_t end_window;
	uint64_t flow_id;
	Burst(){};
	Burst(uint64_t s, uint64_t e, uint64_t id)
	{
		start_window = s;
		end_window = e;
		flow_id = id;
	}
};
#endif
