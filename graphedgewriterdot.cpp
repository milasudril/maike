//@	{"targets":[{"name":"graphedgewriterdot.o","type":"object"}]}

#include "graphedgewriterdot.hpp"
#include "writebuffer.hpp"

using namespace Maike;

GraphEdgeWriterDOT::GraphEdgeWriterDOT(WriteBuffer& wb):r_wb(wb)
	{
	r_wb.write("digraph \"G\"\n\t{\n");
	}

void GraphEdgeWriterDOT::edgeWrite(const char* from,const char* to,const char* color)
	{
	r_wb.write("\t\"").write(from).write("\"->\"").write(to)
		.write("\"[color=\"").write(color).write("\"]\n");
	}

GraphEdgeWriterDOT::~GraphEdgeWriterDOT()
	{
	r_wb.write("\t}\n");
	}
