//@	{
//@	  "dependencies_extra":[{"ref":"about.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_ABOUT_HPP
#define MAIKE_ABOUT_HPP

namespace Maike::Self
{
	extern char const* BuildId;
	extern char const* BuildStartTime;
	extern char const* VcsRevisionId;
	extern char const* VcsTag;
	extern char const* Copyright;
	extern char const* DescriptionShort;
	extern char const* LegalBrief;
}

#endif