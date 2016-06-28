//@	{
//@	"targets":[{"name":"expressionevaluatordefault.hpp","type":"include"}]}
//@	,"dependencies_extra":[{"ref":"expressionevaluatordefault.hpp","rel":"implementation"}]
//@	}

#ifndef MAIKE_EXPRESSIONEVALUATORDEFAULT_HPP
#define MAIKE_EXPRESSIONEVALUATORDEFAULT_HPP

#include "expressionevaluator.hpp"
#include <map>

namespace Maike
	{
	class Stringkey;
	class ResourceObject;

	class ExpressionEvaluatorDefault:public ExpressionEvaluator
		{
		public:
			ExpressionEvaluatorDefault();
			~ExpressionEvaluatorDefault();
			ExpressionEvaluatorDefault& variablesClear() noexcept;
			ExpressionEvaluatorDefault& sysvarsLoad();
			ExpressionEvaluatorDefault& envvarsLoad();
			ExpressionEvaluatorDefault& cfgvarsLoad(const ResourceObject& vars);

			int64_t variableGet(const Stringkey& variable) const noexcept;
			Variant evaluate(const char* expression) const;

		private:
			std::map<Stringkey,int64_t> m_variables;
		};
	}
#endif
