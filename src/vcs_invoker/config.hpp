//@	{"targets":[{"name":"config.hpp","type":"include"}]}

#ifndef MAIKE_VCSINVOKER_CONFIG_HPP
#define MAIKE_VCSINVOKER_CONFIG_HPP

#include "src/command.hpp"

namespace Maike::VcsInvoker
{
	class Config
	{
	public:
		Config():
		   m_get_revision{"git", {"rev-parse", "HEAD"}},
		   m_get_version_tag{"git", {"describe", "--dirty"}},
		   m_get_branch{"git", {"rev-parse", "--abbrev-ref", "HEAD"}}
		{
		}

		Command const& getRevision() const
		{
			return m_get_revision;
		}

		Config& getRevision(Command&& get_revision)
		{
			m_get_revision = std::move(get_revision);
			return *this;
		}


		Command const getVersionTag() const
		{
			return m_get_version_tag;
		}

		Config& getVersionTag(Command&& get_version_tag)
		{
			m_get_version_tag = std::move(get_version_tag);
			return *this;
		}


		Command const& getBranch() const
		{
			return m_get_branch;
		}

		Config& getBranch(Command&& get_branch)
		{
			m_get_branch = std::move(get_branch);
			return *this;
		}


	private:
		Command m_get_revision;
		Command m_get_version_tag;
		Command m_get_branch;
	};

	inline bool operator==(Config const& a, Config const& b)
	{
		return a.getRevision() == b.getRevision() && a.getVersionTag() == b.getVersionTag()
		       && a.getBranch() == b.getBranch();
	}

	inline bool operator!=(Config const& a, Config const& b)
	{
		return !(a == b);
	}

	inline auto toJson(Config const& cfg)
	{
		return KeyValueStore::Compound{}
		   .set("get_revision", cfg.getRevision())
		   .set("get_version_tag", cfg.getVersionTag())
		   .set("get_branch", cfg.getBranch())
		   .takeHandle();
	}

	inline auto fromJson(KeyValueStore::Empty<Config>, KeyValueStore::JsonRefConst ref)
	{
		auto obj = ref.as<KeyValueStore::CompoundRefConst>();
		return Config{}
		   .getRevision(obj.get<Command>("get_revision"))
		   .getVersionTag(obj.get<Command>("get_version_tag"))
		   .getBranch(obj.get<Command>("get_branch"));
	}
}
#endif
