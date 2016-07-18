//@	{"targets":[{"name":"dltest","type":"application"}]}

#include "pluginloader.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"
#include "target_hook.hpp"

int main()
	{
	try
		{
		Maike::PluginLoader cxx("targetcxx");
		auto function=cxx.entryPointGet<decltype(&Maike_Target_Hook_create)>("Maike_Target_Hook_create");
		printf("%p\n",function);
		}
	catch(const Maike::ErrorMessage& message)
		{
		fprintf(stderr,"Error: %s\n",message.messageGet());
		return -1;
		}
	return 0;
	}
