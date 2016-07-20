//@	{"targets":[{"name":"dltest","type":"application"}]}

#include "target_hook_registry.hpp"
#include "systemtargetinfo.hpp"
#include "filein.hpp"
#include "resourceobject.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"
#include "stringkey.hpp"
#include "target_hook.hpp"
#include "stdstream.hpp"

class PRIVATE PrintAssoc:public Maike::Target_Hook_Registry::EnumCallbackFilenameExt
	{
	public:
		void operator()(const Maike::Stringkey& key,const Maike::Target_Hook& hook)
			{
			printf("%zu %p\n",static_cast<uint64_t>(key),&hook);
			}
	};

int main()
	{
	try
		{
		Maike::SystemTargetInfo targetinfo;
		Maike::Target_Hook_Registry reg(targetinfo);
		reg.configAppendDefault();

		/*	{
			Maike::ResourceObject rc{Maike::FileIn("maikeconfig-2.json")};
			reg.configAppend(rc.objectGet("target_hooks"));
			reg.enumerate(PrintAssoc());
			}*/

			{
			Maike::ResourceObject rc(Maike::ResourceObject::Type::OBJECT);
			Maike::ResourceObject targethooks(Maike::ResourceObject::Type::ARRAY);
			reg.configDump(targethooks);
			rc.objectSet("target_hooks",std::move(targethooks));
			rc.write(Maike::StdStream::output());
			}


		}
	catch(const Maike::ErrorMessage& message)
		{
		fprintf(stderr,"Error: %s\n",message.messageGet());
		return -1;
		}
	return 0;
	}
