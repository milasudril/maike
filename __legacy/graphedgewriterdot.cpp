//@	{"targets":[{"name":"graphedgewriterdot.o","type":"object"}]}

#include "graphedgewriterdot.hpp"
#include "writebuffer.hpp"

using namespace Maike;

GraphEdgeWriterDOT::GraphEdgeWriterDOT(WriteBuffer& wb): r_wb(wb)
{
	r_wb.write("digraph \"G\"\n\t{\n");
}

void GraphEdgeWriterDOT::edgeWrite(const char* from, const char* to, const char* label)
{
	r_wb.write("\t\"")
	   .write(from)
	   .write("\"->\"")
	   .write(to)
	   .write("\"[label=\"")
	   .write(label)
	   .write("\"]\n");
}

GraphEdgeWriterDOT::~GraphEdgeWriterDOT()
{
	try
	{
		r_wb.write("\t}\n");
	}
	catch(...)
	{
	}
}
