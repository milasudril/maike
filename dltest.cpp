//@	{"targets":[{"name":"dltest","type":"application"}]}

#include "target_hook_registry.hpp"
#include "systemtargetinfo.hpp"
#include "filein.hpp"
#include "resourceobject.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"

int main()
	{
	try
		{
		Maike::SystemTargetInfo targetinfo;
		Maike::Target_Hook_Registry reg(targetinfo);
		Maike::ResourceObject rc{Maike::FileIn("maikeconfig-2.json")};
		reg.configAppend(rc.objectGet("target_hooks"));
		}
	catch(const Maike::ErrorMessage& message)
		{
		fprintf(stderr,"Error: %s\n",message.messageGet());
		return -1;
		}
	return 0;
	}
