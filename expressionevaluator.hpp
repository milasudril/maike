//@	{
//@	 "targets":[{"name":"expressionevaluator.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"expressionevaluator.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_EXPRESSIONEVALUATOR_HPP
#define MAIKE_EXPRESSIONEVALUATOR_HPP

#include <map>

namespace Maike
	{
	class Stringkey;
	class Variant;
	class SystemTargetInfo;

	class ExpressionEvaluator
		{
		public:
			ExpressionEvaluator(SystemTargetInfo&& targetinfo)=delete;
			ExpressionEvaluator(const SystemTargetInfo& targetinfo);
			~ExpressionEvaluator();

			Variant variableGet(const Stringkey& variable) const noexcept;
			Variant evaluate(const char* expression) const;

		private:
			const SystemTargetInfo& r_targetinfo;
		};
	}
#endif
