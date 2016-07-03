//@	{
//@	 "targets":[{"name":"expressionevaluatordefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"expressionevaluatordefault.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_EXPRESSIONEVALUATORDEFAULT_HPP
#define MAIKE_EXPRESSIONEVALUATORDEFAULT_HPP

#include "expressionevaluator.hpp"
#include <map>

namespace Maike
	{
	class Stringkey;
	class Variant;
	class SystemTargetInfo;

	class ExpressionEvaluatorDefault:public ExpressionEvaluator
		{
		public:
			ExpressionEvaluatorDefault(SystemTargetInfo&& targetinfo)=delete;
			ExpressionEvaluatorDefault(const SystemTargetInfo& targetinfo);
			~ExpressionEvaluatorDefault();

			Variant variableGet(const Stringkey& variable) const noexcept;
			Variant evaluate(const char* expression) const;

		private:
			const SystemTargetInfo& r_targetinfo;
		};
	}
#endif
