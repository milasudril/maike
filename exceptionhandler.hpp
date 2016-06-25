//@	{
//@	"targets":[{"name":"exceptionhandler.hpp","type":"include"}]}
//@	"dependencies_extra":[{"ref":"exceptionhandler.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_EXCEPTIONHANDLER_H
#define MAIKE_EXCEPTIONHANDLER_H

namespace Maike
	{
	class ErrorMessage;

	[[noreturn]] void exceptionHandlerDefault(const ErrorMessage& message);

	typedef __attribute__((__noreturn__)) decltype(&exceptionHandlerDefault) ExceptionHandler;

	void exceptionHandlerSet(ExceptionHandler eh);
	[[noreturn]] void exceptionRaise(const ErrorMessage& message);
	}

#endif
