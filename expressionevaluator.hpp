//@	{"targets":[{"name":"expressionevaluator.hpp","type":"include"}]}

#ifndef EXPRESSIONEVALUATOR_HPP
#define EXPRESSIONEVALUATOR_HPP

#include "variant.hpp"

namespace Maike
	{
	class ExpressionEvaluator
		{
		public:
			virtual Variant evaluate(const char* expression) const=0;
		protected:
			~ExpressionEvaluator()=default;
		};
	}

#endif
