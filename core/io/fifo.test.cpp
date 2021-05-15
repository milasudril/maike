//@	{
//@	 "targets":[{"name":"fifo.test","type":"application",
//@	 	"dependencies":[{"ref":"pthread", "rel":"external"}], "autorun":1}]
//@	}

#undef NDEBUG

#include "./fifo.hpp"

#include <random>
#include <vector>
#include <limits>
#include <algorithm>
#include <cassert>
#include <thread>


namespace Testcases
{
	void maikeFifoProduceAndConsumeConsumeSmallChunks()
	{
		constexpr auto NumElems = static_cast<size_t>(65521);
		std::vector<uint8_t> src;
		src.reserve(NumElems);
		std::generate_n(std::back_inserter(src),
		                NumElems,
		                [u = std::uniform_int_distribution{std::numeric_limits<uint8_t>::min(),
		                                                   std::numeric_limits<uint8_t>::max()},
		                 rng = std::mt19937{}]() mutable { return u(rng); });

		Maike::Io::Fifo<uint8_t> fifo;

		std::vector<uint8_t> result;
		result.reserve(NumElems);
		std::thread consumer{[&fifo, &result]() {
			std::vector<uint8_t> tmp;
			tmp.resize(2251);
			while(true)
			{
				auto n = fifo.read(tmp.data(), tmp.size());
				result.insert(std::end(result), std::begin(tmp), std::begin(tmp) + n);
				if(n == 0) { return; }
			}
		}};

		std::thread producer{[&src, &fifo]() {
			auto ptr = src.data();
			auto const N = src.size();
			auto n_left = N;
			while(n_left != 0)
			{
				auto const n = std::min(static_cast<size_t>(8191), n_left);
				fifo.write(ptr, n);
				n_left -= n;
				ptr += n;
			}
			fifo.stop();
		}};


		producer.join();
		consumer.join();

		assert(src.size() == result.size());
		assert(src == result);
	}

	void maikeFifoProduceAndConsumeConsumeLargeChunks()
	{
		constexpr auto NumElems = static_cast<size_t>(65521);
		std::vector<uint8_t> src;
		src.reserve(NumElems);
		std::generate_n(std::back_inserter(src),
		                NumElems,
		                [u = std::uniform_int_distribution{std::numeric_limits<uint8_t>::min(),
		                                                   std::numeric_limits<uint8_t>::max()},
		                 rng = std::mt19937{}]() mutable { return u(rng); });

		Maike::Io::Fifo<uint8_t> fifo;

		std::vector<uint8_t> result;
		result.reserve(NumElems);
		std::thread consumer{[&fifo, &result]() {
			std::vector<uint8_t> tmp;
			tmp.resize(8191);
			while(true)
			{
				auto n = fifo.read(tmp.data(), tmp.size());
				result.insert(std::end(result), std::begin(tmp), std::begin(tmp) + n);
				if(n == 0) { return; }
			}
		}};

		std::thread producer{[&src, &fifo]() {
			auto ptr = src.data();
			auto const N = src.size();
			auto n_left = N;
			while(n_left != 0)
			{
				auto const n = std::min(static_cast<size_t>(2251), n_left);
				fifo.write(ptr, n);
				n_left -= n;
				ptr += n;
			}
			fifo.stop();
		}};

		producer.join();
		consumer.join();

		assert(src.size() == result.size());
		assert(src == result);
	}
}

int main()
{
	Testcases::maikeFifoProduceAndConsumeConsumeSmallChunks();
	Testcases::maikeFifoProduceAndConsumeConsumeLargeChunks();
	return 0;
}