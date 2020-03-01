//@	{
//@	 "targets":[{"name":"zipoptions.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"zipoptions.o","rel":"implementation"}]
//@	}


#ifndef MAIKE_ZIPOPTIONS_HPP
#define MAIKE_ZIPOPTIONS_HPP

#include "../command.hpp"

namespace Maike
{
	class ResourceObject;

	class PRIVATE Zipoptions
	{
	public:
		Zipoptions() noexcept
		{
		}

		Zipoptions(const ResourceObject& options);

		const Command& createGet() const noexcept
		{
			return m_create;
		}

		const Command& renameGet() const noexcept
		{
			return m_rename;
		}

		Zipoptions& configAppendDefault();
		Zipoptions& configAppend(const ResourceObject& options);
		void configDump(ResourceObject& options) const;

	private:
		Command m_create;
		Command m_rename;
	};
}

#endif
