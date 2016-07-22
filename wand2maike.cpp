//@	{"targets":[{"name":"wand2maike","type":"application"}]}

#include "fileutils.hpp"
#include "fileinfo.hpp"
#include "filein.hpp"
#include "fileout.hpp"
#include "readbuffer.hpp"
#include "writebuffer.hpp"
#include "pathutils.hpp"
#include "resourceobject.hpp"
#include "stringkey.hpp"
#include <stack>
#include <vector>
#include <string>
#include <map>

using namespace Maike;


class PRIVATE SpellTree
	{
	public:
		SpellTree& treeInsert(const Stringkey& name)
			{return m_tree[name];}

		SpellTree& itemAppend(const std::string& str)
			{
			m_values.push_back(str);
			return *this;
			}

		const SpellTree* find(const Stringkey& name) const noexcept
			{
			auto i=m_tree.find(name);
			if(i==m_tree.end())
				{return nullptr;}
			return &i->second;
			}

	private:
		std::map<Stringkey,SpellTree> m_tree;
		std::vector<std::string> m_values;
	};

void build(ResourceObject& obj,const SpellTree& tree)
	{
	}

static void process(const char* filename,const char* out_dir)
	{
	printf("Processing %s\n",filename);
	FileIn source(filename);
	ReadBuffer rb(source);
	FileOut dest(dircat(out_dir,filename).c_str());
	WriteBuffer wb(dest);
	enum class State:unsigned int{INIT,SPELL,SPELL_ESC,SPELL_END};
	auto state=State::INIT;
	std::string str;
	SpellTree root;
	auto tree_current=&root;
	std::stack<SpellTree*> tree_stack;
	while(!rb.eof())
		{
		auto ch_in=rb.byteRead();
		switch(state)
			{
			case State::INIT:
				switch(ch_in)
					{
					case '\r':
						break;
					case '\n':
						if(str=="#ifdef __WAND__")
							{state=State::SPELL;}
						else
							{wb.write(str.c_str()).write("\n");}
						str.clear();
						break;
					default:
						str+=ch_in;
					}
				break;

			case State::SPELL:
				switch(ch_in)
					{
					case '#':
						{
						if(!tree_stack.empty())
							{throw "Spell error: unterminated spell tree";}
						state=State::SPELL_END;
						ResourceObject rc(ResourceObject::Type::OBJECT);
						build(rc,root);
						rc.write(dest);
						}
						break;
					case '^':
						state=State::SPELL_ESC;
						break;
					case '[':
						{
						tree_stack.push(tree_current);
						tree_current=&tree_current->treeInsert(Stringkey(str.c_str()));
						str.clear();
						}
						break;
					case ';':
						tree_current->itemAppend(str);
						str.clear();
						break;
					case ']':
						if(tree_stack.empty())
							{throw "Spell error: too many ']'";}
						if(str.size()!=0)
							{tree_current->itemAppend(str);}
						tree_current=tree_stack.top();
						tree_stack.pop();
						str.clear();
						break;
					default:
						str+=ch_in;
					}
				break;
			case State::SPELL_ESC:
				state=State::SPELL;
				str+=ch_in;
				break;
			case State::SPELL_END:
				if(ch_in=='\n')
					{state=State::INIT;}
				break;
			}
		}
	}

int main(int argc,char** argv)
	{
	if(argc<2)
		{
		printf("Usage: wand2maike files ...\n\n");
		return 0;
		}

	const char* out_dir="__wand2maike";
	if(!FileUtils::exists(out_dir))
		{FileUtils::mkdir(out_dir);}

	--argc;
	++argv;
	while(argc!=0)
		{
		if(FileInfo(*argv).typeGet()!=FileInfo::Type::DIRECTORY)
			{process(*argv,out_dir);}
		--argc;
		++argv;
		}

	return 0;
	}
