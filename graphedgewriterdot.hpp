//@	{
//@	 "targets":[{"name":"graphedgewriterdot.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"graphedgewriterdot.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_GRAPHEDGEWRITERDOT_HPP
#define MAIKE_GRAPHEDGEWRITERDOT_HPP

#include "graphedgewriter.hpp"

namespace Maike
	{
	class TextWriter;
	class GraphEdgeWriterDOT:public GraphEdgeWriter
		{
		public:
			GraphEdgeWriterDOT(TextWriter& writer);
			void edgeWrite(const char* from,const char* to,const char* color);
			~GraphEdgeWriterDOT();
		private:
			TextWriter& r_writer;
		};
	}

#endif
