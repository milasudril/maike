//@	{
//@	 "targets":[{"name":"exceptionhandler.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"exceptionhandler.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_EXCEPTIONHANDLER_HPP
#define MAIKE_EXCEPTIONHANDLER_HPP

#include "visibility.hpp"

namespace Maike
	{
	class ErrorMessage;

	PRIVATE [[noreturn]] void exceptionHandlerDefault(const ErrorMessage& message);

	typedef __attribute__((__noreturn__)) decltype(&exceptionHandlerDefault) ExceptionHandler;

	PRIVATE void exceptionHandlerSet(ExceptionHandler eh);
	PRIVATE [[noreturn]] void exceptionRaise(const ErrorMessage& message);
	}

#endif
