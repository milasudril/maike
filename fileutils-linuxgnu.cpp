//@	[
//@		[
//@		"and(not(less_than(linux,version('2.6.33'))),gnu)"
//@			,{
//@		 	"targets":[{"name":"fileutils.o","type":"object"}]
//@			}
//@		]
//@	]

#define _LARGEFILE64_SOURCE

#include "fileutils.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "strerror.hpp"
#include "stdstream.hpp"
#include "writebuffer.hpp"
#include "filein.hpp"
#include "fileout.hpp"
#include "readbuffer.hpp"
#include "pathutils.hpp"
#include "directorylister.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>

#include <vector>
#include <cstdint>
#include <tuple>
#include <string>
#include <regex>

using namespace Maike;

static WriteBuffer& escape(WriteBuffer& wb,const char* str)
	{
	wb.write(static_cast<uint8_t>('\''));
	while(*str!='\0')
		{
		auto ch_in=*str;
		switch(ch_in)
			{
			case '\'':
				wb.write("'\\''");
				break;
			default:
				wb.write(static_cast<uint8_t>(ch_in));
				break;
			}
		++str;
		}
	wb.write(static_cast<uint8_t>('\''));
	return wb;
	}

bool FileUtils::newer(const char* file_a,const char* file_b)
	{
	struct stat stat_a;
	struct stat stat_b;

	auto res_a=stat(file_a, &stat_a);
	auto errno_a=errno;
	auto res_b=stat(file_b, &stat_b);
	auto errno_b=errno;

	if(res_a==-1 && res_b==-1)
		{
		exceptionRaise(ErrorMessage("Error: None of the files #0;, and #1; are accessible.\n#0;: #2;.\n#1;: #3;."
			,{
			 file_a
			,file_b
			,static_cast<const char*>(Maike::strerror(errno_a))
			,static_cast<const char*>(Maike::strerror(errno_b))
			}));
		}

	if(res_a==-1)
		{return 0;}

	if(res_b==-1)
		{return 1;}

	return (stat_a.st_mtime > stat_b.st_mtime) && !S_ISDIR(stat_a.st_mode);
	}

void FileUtils::mkdir(const char* name)
	{
	WriteBuffer wb(StdStream::output());
	wb.write("mkdir ");
	escape(wb,name);
	wb.write(static_cast<uint8_t>('\n'));
	if( ::mkdir(name, S_IRWXU )==-1 )
		{
		exceptionRaise(ErrorMessage("It was not possible to create a directory with name #0;. #1;"
			,{name,static_cast<const char*>(Maike::strerror(errno))}));
		}
	}


bool FileUtils::exists(const char* file)
	{
	if(access(file,F_OK)==-1)
		{return 0;}
	return 1;
	}


namespace
	{
	class FileDescriptor
		{
		public:
			FileDescriptor(const char* file,int flags,int mode)
				{
				m_fd=open(file,flags,mode);
				if(m_fd==-1)
					{
					exceptionRaise(ErrorMessage("It was not possible to open the file #0;. #1;"
						,{static_cast<const char*>(Maike::strerror(errno))}));
					}
				}

			FileDescriptor(const char* file,int flags)
				{
				m_fd=open(file,flags);
				if(m_fd==-1)
					{
					exceptionRaise(ErrorMessage("It was not possible to open the file #0;. #1;"
						,{static_cast<const char*>(Maike::strerror(errno))}));
					}
				}

			~FileDescriptor() noexcept
				{close(m_fd);}

			int get() noexcept
				{return m_fd;}

		private:
			int m_fd;
		};
	}

void FileUtils::copy(const char* source,const char* dest)
	{
	WriteBuffer wb(StdStream::output());
	wb.write("cp ");
	escape(wb,source);
	escape(wb,dest);
	wb.write(static_cast<uint8_t>('\n'));

	FileDescriptor source_fd(source,O_RDONLY);
	FileDescriptor dest_fd(dest,O_CREAT|O_WRONLY,S_IRUSR|S_IWUSR);

	struct stat source_stat;
	if(fstat(source_fd.get(),&source_stat)==-1)
		{
		unlink(dest);
		exceptionRaise(ErrorMessage("stat error: #0;",{static_cast<const char*>(Maike::strerror(errno))}));
		}

	auto size=source_stat.st_size;
	while(size!=0)
		{
		auto res=sendfile(dest_fd.get(),source_fd.get(),nullptr,size);
		if(res==-1)
			{
			unlink(dest);
			exceptionRaise(ErrorMessage("It was not possible to copy #0; to #1;: "
				,{source,dest,static_cast<const char*>(Maike::strerror(errno))}));
			}

		size-=res;
		}
	}

void FileUtils::copyFilter(const char* source,const char* dest
	,const char* comment_line_regexp)
	{
		{
		WriteBuffer wb(StdStream::output());
		wb.write("grep -v '").write(comment_line_regexp).write("' ");
		escape(wb,source);
		wb.write(" > ");
		escape(wb,dest);
		wb.write("\n");
		}

	std::regex comment_line(comment_line_regexp,std::regex_constants::basic);

	FileIn src(source);
	FileOut dst(dest);
	ReadBuffer rb(src);
	WriteBuffer wb(dst);
	std::string line;
	while(!rb.eof())
		{
		auto ch_in=rb.byteRead();
		switch(ch_in)
			{
			case '\r':
				break;
			case '\n':
				if(!std::regex_search(line,comment_line))
					{
					line+=ch_in;
					wb.write(line.c_str());
					}
				line.clear();
				break;
			default:
				line+=ch_in;
			}
		}
	}

void FileUtils::removeTree(const char* name
	,const std::set<Stringkey>& keeplist)
	{
	WriteBuffer wb(StdStream::output());
	std::stack< std::pair<std::string,bool> > nodes;
	nodes.push({name,0});

	while(!nodes.empty())
		{
		auto node=std::move( nodes.top() );
		nodes.pop();
		if(rmdir(node.first.c_str())==-1)
			{
			auto res=errno;
			if(node.second)
				{continue;}
			switch(res)
				{
				case ENOENT:
					break;
				case ENOTDIR:
					if(unlink(node.first.c_str())==-1)
						{
						exceptionRaise(ErrorMessage("It was not possible to remove #0: "
							,{node.first.c_str(),static_cast<const char*>(Maike::strerror(errno))}));
						}
					else
						{
						wb.write("rm ");
						escape(wb,node.first.c_str());
						wb.write(static_cast<uint8_t>('\n'));
						}
					break;
				case ENOTEMPTY:
				case EEXIST:
					{
					nodes.push({node.first,1});
					DirectoryLister dir(node.first.c_str());
					const char* entry;
					while((entry=dir.read())!=nullptr)
						{
						auto elem=dircat(node.first,entry);
						if( keeplist.find(Stringkey(elem.c_str()))==keeplist.end() )
							{nodes.push({elem,0});}
						}
					}
					break;
				default:
					exceptionRaise(ErrorMessage("It was not possible to remove #0: "
						,{node.first.c_str(),static_cast<const char*>(Maike::strerror(errno))}));
				}
			}
		else
			{
			wb.write("rm -d");
			escape(wb,node.first.c_str());
			wb.write(static_cast<uint8_t>('\n'));
			}
		}
	}

void FileUtils::remove(const char* name)
	{
	if(rmdir(name)==-1)
		{
		auto res=errno;
		switch(res)
			{
			case ENOTEMPTY:
			case EEXIST:
			case ENOENT:
				return;
			case ENOTDIR:
				if(unlink(name)==-1)
					{
					exceptionRaise(ErrorMessage("It was not possible to remove #0: "
						,{name,static_cast<const char*>(Maike::strerror(errno))}));
					}
				break;
			default:
				exceptionRaise(ErrorMessage("It was not possible to remove #0: "
					,{name,static_cast<const char*>(Maike::strerror(errno))}));
			}
		}
	WriteBuffer wb(StdStream::output());
	wb.write("rm -d ");
	escape(wb,name);
	wb.write(static_cast<uint8_t>('\n'));
	}

void FileUtils::echo(const char* str,const char* filename)
	{
	//	Shell stuff
		{
		WriteBuffer wb(StdStream::output());
		wb.write("cat << 'MAIKE_CONFIG' > ");
		escape(wb,filename);
		wb.write("\n").write(str).write("\nMAIKE_CONFIG\n");
		}
	
		{
		FileOut file(filename);
		WriteBuffer wb(file);
		wb.write(str);
		}
	}
