//@	{"targets":[{"name":"graphedgewriter.hpp","type":"include"}]}

#ifndef MAIKE_GRAPHEDGEWRITER_HPP
#define MAIKE_GRAPHEDGEWRITER_HPP

namespace Maike
	{
	class GraphEdgeWriter
		{
		public:
			virtual void edgeWrite(const char* from,const char* to
				,const char* edgecolor)=0;
		protected:
			~GraphEdgeWriter()=default;
		};
	}

#endif
