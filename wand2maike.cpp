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
#include "twins.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include <stack>
#include <vector>
#include <string>
#include <map>
#include <memory>

using namespace Maike;


class PRIVATE SpellTree
	{
	public:
		typedef std::unique_ptr<SpellTree> TreeHandle;

		SpellTree& treeInsert(const char* name)
			{
			auto x=TreeHandle(new SpellTree);

			m_tree[Stringkey(name)].push_back(std::move(x));
			return *m_tree[Stringkey(name)].back().get();
			}

		SpellTree& itemAppend(const std::string& str)
			{
			m_items.push_back(str);
			return *this;
			}

		Twins<const TreeHandle*> find(const Stringkey& name) const noexcept
			{
			auto i=m_tree.find(name);
			if(i==m_tree.end())
				{return {nullptr,nullptr};}
			auto& tree=i->second;
			return {tree.data(),tree.data() + tree.size()};
			}

		const char* itemGet(size_t index) const noexcept
			{return m_items[index].c_str();}

		size_t itemCountGet() const noexcept
			{return m_items.size();}

	private:
		std::map< Stringkey,std::vector<TreeHandle > > m_tree;
		std::vector<std::string> m_items;
	};

static void dependencyGet(ResourceObject& dep,const SpellTree& dep_in)
	{
	if(dep_in.itemCountGet()>0)
		{dep.objectSet("ref",ResourceObject(dep_in.itemGet(0)));}
	if(dep_in.itemCountGet()>1)
		{dep.objectSet("rel",ResourceObject(dep_in.itemGet(1)));}
	else
		{dep.objectSet("rel",ResourceObject("implementation"));}
	}

static void targetGet(ResourceObject& target,const SpellTree& target_in)
	{
		{
		auto name=target_in.find(Stringkey("name")).first->get();
		if(name!=nullptr)
			{
			if( name->itemCountGet()!=0)
				{target.objectSet("name",ResourceObject(name->itemGet(0)));}
			}
		}

		{
		auto type=target_in.find(Stringkey("type")).first->get();
		if(type!=nullptr)
			{
			if( type->itemCountGet()!=0)
				{target.objectSet("type",ResourceObject(type->itemGet(0)));}
			}
		}

	auto deps_in=target_in.find(Stringkey("dependency"));
	ResourceObject deps(ResourceObject::Type::ARRAY);
	while(deps_in.first!=deps_in.second)
		{
		ResourceObject dep(ResourceObject::Type::OBJECT);
		auto& d=*(deps_in.first->get());
		dependencyGet(dep,d);
		deps.objectAppend(std::move(dep));
		++deps_in.first;
		}
	target.objectSet("dependencies",std::move(deps));
	}

static void build(ResourceObject& obj,const SpellTree& tree)
	{
	ResourceObject targets(ResourceObject::Type::ARRAY);
	auto targets_in=tree.find(Stringkey("target"));
	while(targets_in.first!=targets_in.second)
		{
		ResourceObject target(ResourceObject::Type::OBJECT);
		auto& t=*(targets_in.first->get());
		targetGet(target,t);
		targets.objectAppend(std::move(target));
		++targets_in.first;
		}
	obj.objectSet("targets",std::move(targets));
	}

class PRIVATE Encoder:public DataSink
	{
	public:
		Encoder(WriteBuffer& wb):r_wb(wb)
			{r_wb.write("//@\t");}
		size_t write(const void* buffer,size_t n);

	private:
		WriteBuffer& r_wb;
		void destroy(){}
	};

size_t Encoder::write(const void* buffer,size_t n)
	{
	auto buf=static_cast<const uint8_t*>(buffer);
	const auto N=n;
	while(n!=0)
		{
		auto ch_in=*buf;
		r_wb.write(ch_in);
		if(ch_in=='\n')
			{r_wb.write("//@\t");}
		++buf;
		--n;
		}
	return N;
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
	std::stack< SpellTree* > tree_stack;
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
				if(ch_in>=0 && ch_in<=' ')
					{break;}
				switch(ch_in)
					{
					case '#':
						{
						if(!tree_stack.empty())
							{throw "Spell error: unterminated spell tree";}
						state=State::SPELL_END;
						ResourceObject rc(ResourceObject::Type::OBJECT);
						build(rc,root);
						rc.write(Encoder(wb));
						}
						break;
					case '^':
						state=State::SPELL_ESC;
						break;
					case '[':
						{
						tree_stack.push(tree_current);
						tree_current=&tree_current->treeInsert(str.c_str());
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
	try
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
		}
	catch(const ErrorMessage& msg)
		{fprintf(stderr,"Error: %s\n",msg.messageGet());return -1;}

	return 0;
	}

