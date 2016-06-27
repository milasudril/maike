//@	{"targets":[{"name":"exceptionhandler.o","type":"object"}]}

#include "exceptionhandler.hpp"
#include "errormessage.hpp"

using namespace Maike;

void Maike::exceptionHandlerDefault(const ErrorMessage& message)
	{
	throw message;
	}

static ExceptionHandler exception_handler=exceptionHandlerDefault;

void Maike::exceptionHandlerSet(ExceptionHandler eh)
	{
	exception_handler=eh;
	}

[[noreturn]] void Maike::exceptionRaise(const ErrorMessage& message)
	{
	exception_handler(message);
	}
