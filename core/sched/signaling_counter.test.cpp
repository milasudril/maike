//@	{
//@	 "targets":[{"name":"signaling_counter.test","type":"application", "autorun":1}]
//@	}

#include "./signaling_counter.hpp"

namespace Testcases
{
	void maikeSchedSignalingCounterIncAndSignal()
	{
		Maike::Sched::SignalingCounter counter{0};
		for(int k = 0; k < 3; ++k)
		{
			++counter;
		}
		counter.wait(3);
	}
}

int main()
{
	Testcases::maikeSchedSignalingCounterIncAndSignal();
	return 0;
}
