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
	class ResourceObjectDefault;

	class ExpressionEvaluatorDefault:public ExpressionEvaluator
		{
		public:
			ExpressionEvaluatorDefault();
			~ExpressionEvaluatorDefault();
			ExpressionEvaluatorDefault& variablesClear() noexcept;
			ExpressionEvaluatorDefault& sysvarsLoad();
			ExpressionEvaluatorDefault& envvarsLoad();
			ExpressionEvaluatorDefault& cfgvarsLoad(const ResourceObjectDefault& vars);

			Variant variableGet(const Stringkey& variable) const noexcept;
			Variant evaluate(const char* expression) const;

		private:
			std::map<Stringkey,Variant> m_variables;
		};
	}
#endif
