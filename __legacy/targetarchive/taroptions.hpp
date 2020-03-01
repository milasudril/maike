//@	{
//@	 "targets":[{"name":"taroptions.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"taroptions.o","rel":"implementation"}]
//@	}


#ifndef MAIKE_TAROPTIONS_HPP
#define MAIKE_TAROPTIONS_HPP


#include "../command.hpp"
#include "../stringkey.hpp"
#include <vector>
#include <string>
#include <map>

namespace Maike
{
	class ResourceObject;

	class PRIVATE Taroptions
	{
	public:
		Taroptions() noexcept
		{
		}

		Taroptions(const ResourceObject& options);

		const Command& commandGet() const noexcept
		{
			return m_command;
		}

		const char* targetStripGet() const noexcept
		{
			return m_target_strip.c_str();
		}

		const char* rootAppendGet() const noexcept
		{
			return m_root_append.c_str();
		}

		const char* compressorGet(const char* compressor) const noexcept;

		Taroptions& configAppendDefault();
		Taroptions& configAppend(const ResourceObject& options);
		void configDump(ResourceObject& options) const;

		void configClear() noexcept
		{
			m_compressors.clear();
		}

	private:
		Command m_command;
		std::map<Stringkey, Twins<std::string>> m_compressors;
		std::string m_target_strip;
		std::string m_root_append;
	};
}

#endif
