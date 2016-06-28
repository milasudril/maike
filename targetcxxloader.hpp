//@	{
//@	 "targets":[{"name":"targetcxxloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxloader.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXLOADER_HPP
#define MAIKE_TARGETCXXLOADER_HPP

#include "targetloader.hpp"

namespace Maike
	{
	class TargetCxxLoader:public TargetLoader
		{
		public:
			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,const ExpressionEvaluator& evaluator) const;
		};
	}

#endif
