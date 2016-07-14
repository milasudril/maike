//@	{"targets":[{"name":"graphedgewriterdot.o","type":"object"}]}

#include "graphedgewriterdot.hpp"
#include "textwriter.hpp"

using namespace Maike;

GraphEdgeWriterDOT::GraphEdgeWriterDOT(TextWriter& writer):r_writer(writer)
	{
	r_writer.write("digraph \"G\"\n\t{\n");
	}

void GraphEdgeWriterDOT::edgeWrite(const char* from,const char* to,const char* color)
	{
	r_writer.write("\t\"").write(from).write("\"->\"").write(to)
		.write("\"[color=\"").write(color).write("\"]\n");
	}

GraphEdgeWriterDOT::~GraphEdgeWriterDOT()
	{
	r_writer.write("\t}\n");
	}
