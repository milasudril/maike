//@	{
//@	 "targets":[{"name":"graphedgewriterdot.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"graphedgewriterdot.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_GRAPHEDGEWRITERDOT_HPP
#define MAIKE_GRAPHEDGEWRITERDOT_HPP

#include "graphedgewriter.hpp"

namespace Maike
	{
	class WriteBuffer;
	class GraphEdgeWriterDOT:public GraphEdgeWriter
		{
		public:
			GraphEdgeWriterDOT(WriteBuffer& wb);
			void edgeWrite(const char* from,const char* to,const char* color);
			~GraphEdgeWriterDOT();
		private:
			WriteBuffer& r_wb;
		};
	}

#endif
