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
#ifndef __GNUC__
	/**Function called when an exception happens.
	 *
	 * An ExceptionHandler is a function that is called when an exception happens
	 * There are three legal ways to leave an ExceptionHandler:
	 *
	 * 1. Throw the ErrorMessage. This is the default action.
	 * 2. Package the ErrorMessage into another object, and throw the result.
	 * 3. Terminate the process through any appropriate function provided by
	 *    the runtime library or the operating system.
	 *
	 * An ExceptionHandler should not throw anything else than the ErrorMessage,
	 * or a repackaged version of it.
	 */
	typedef void (*ExceptionHandler)(const ErrorMessage& message);
#else
	typedef __attribute__((__noreturn__)) decltype(&exceptionHandlerDefault) ExceptionHandler;
#endif

	PRIVATE void exceptionHandlerSet(ExceptionHandler eh);
	PRIVATE [[noreturn]] void exceptionRaise(const ErrorMessage& message);

	PRIVATE ExceptionHandler exceptionHandlerGet();
}

#endif
