//@	{
//@	 "targets":[{"name":"pingpong_buffer.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./pingpong_buffer.hpp"

#include <random>
#include <vector>
#include <limits>
#include <algorithm>
#include <cassert>
#include <thread>

namespace Testcases
{
	void maikePingpongBufferProduceAndConsumeData()
	{
		Maike::PingPongBuffer<int> buffer{1024};
		std::vector<int> src;
		std::generate_n(std::back_inserter(src), static_cast<size_t>(10.5*buffer.capacity()), [u = std::uniform_int_distribution{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()}, rng = std::random_device{}]() mutable {
			return u(rng);
		});

		std::thread producer{[&src, &buffer]()
			{
				auto ptr = src.data();
				auto N = src.size();
				while(N!=0)
				{
					auto n = buffer.write(ptr, N);
					N -= n;
					ptr += n;
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
				}
				buffer.flush();
				printf("Done\n");
			}};

		std::vector<int> result;
		std::thread consumer{[&buffer, &result](){
			std::vector<int> tmp;
			tmp.resize(buffer.capacity());
			while(true)
			{
				auto n = buffer.read(tmp.data());
				result.insert(std::end(result), std::begin(tmp), std::begin(tmp) + n);
				if( n == 0)
				{ return;}
			}
		}};

		producer.join();
		consumer.join();

		printf("%zu %zu\n", src.size(), result.size());

		assert(src.size() == result.size());
		assert(src == result);
	}
}

int main()
{
	Testcases::maikePingpongBufferProduceAndConsumeData();
	return 0;
}