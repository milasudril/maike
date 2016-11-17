//@	{"targets":[{"name":"targetarchive.o","type":"object"}]}

#include "targetarchive.hpp"
#include "targetarchivecompiler.hpp"
#include "../resourceobject.hpp"
#include "../stringkey.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include "../exceptionhandler.hpp"

using namespace Maike;

static TargetArchive::Type type(const char* name_src,const char* type)
	{
	auto key=Stringkey(type);
	if(key==Stringkey("tar"))
		{return TargetArchive::Type::TAR;}

	if(key==Stringkey("zip"))
		{return TargetArchive::Type::ZIP;}

	exceptionRaise(ErrorMessage("#0;: Unknown target type #1;.",{name_src,type}));
	}

static const char* type(TargetArchive::Type type)
	{
	switch(type)
		{
		case TargetArchive::Type::TAR:
			return "tar";
		case TargetArchive::Type::ZIP:
			return "zip";
		}
	return nullptr;
	}

static TargetArchive::Compression compression(const char* name_src,const char* compression)
	{
	auto key=Stringkey(compression);
	if(key==Stringkey("none"))
		{return TargetArchive::Compression::NONE;}

	if(key==Stringkey("gzip"))
		{return TargetArchive::Compression::GZIP;}

	exceptionRaise(ErrorMessage("#0;: Unknown compression scheme #1;.",{name_src,compression}));
	}

static const char* compression(TargetArchive::Compression compression)
	{
	switch(compression)
		{
		case TargetArchive::Compression::NONE:
			return "none";
		case TargetArchive::Compression::GZIP:
			return "gzip";
		}
	return nullptr;
	}


TargetArchive::TargetArchive(const ResourceObject& obj
	,const TargetArchiveCompiler& compiler,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count):
	TargetBase(obj,name_src,in_dir,root,id,line_count)
	,r_compiler(compiler)
	{
	m_type=type(name_src,static_cast<const char*>( obj.objectGet("type") ));
	m_compression=Compression::NONE;
	if(obj.objectExists("compression"))
		{m_compression=compression(name_src,static_cast<const char*>( obj.objectGet("compression") ));}
	}

void TargetArchive::dumpDetails(ResourceObject& target) const
	{
	target.objectSet("type",target.create(type(typeGet())))
		.objectSet("compression",target.create(compression(compressionGet())));
	}

bool TargetArchive::upToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir) const
	{
	printf("%s up to date?\n",nameGet());
	return 0;
	}

void TargetArchive::compileImpl(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	printf("Compile %s\n",nameGet());
	}

Maike::TargetArchive::~TargetArchive() noexcept
	{
	}


TargetArchive* TargetArchive::create(const ResourceObject& obj
	,const TargetArchiveCompiler& intpret,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count)
	{return new TargetArchive(obj,intpret,name_src,in_dir,root,id,line_count);}

void TargetArchive::destroy() noexcept
	{delete this;}
