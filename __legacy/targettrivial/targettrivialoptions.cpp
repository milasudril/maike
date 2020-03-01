//@	{
//@	 "targets":[{"name":"targettrivialoptions.o","type":"object"}]
//@	}

#include "targettrivialoptions.hpp"
#include "../resourceobject.hpp"

Maike::TargetTrivialOptions::TargetTrivialOptions()
{
}

Maike::TargetTrivialOptions::TargetTrivialOptions(const ResourceObject& options)
{
	configClear();
	configAppend(options);
}

void Maike::TargetTrivialOptions::configClear()
{
	m_build_cmd = Command{};
	m_deps.clear();
}

Maike::TargetTrivialOptions& Maike::TargetTrivialOptions::configAppendDefault()
{
	configClear();
	return *this;
}

Maike::TargetTrivialOptions& Maike::TargetTrivialOptions::configAppend(const ResourceObject& opts)
{
	if(opts.objectExists("command")) { m_build_cmd = Command(opts.objectGet("command")); }
	if(opts.objectExists("filename_ext"))
	{ m_filename_ext = static_cast<const char*>(opts.objectGet("filename_ext")); }
	if(opts.objectExists("dependencies"))
	{
		auto deps = opts.objectGet("dependencies");
		auto M = deps.objectCountGet();
		for(decltype(M) l = 0; l < M; ++l)
		{
			m_deps.push_back(Dependency{deps.objectGet(l)});
		}
	}
	return *this;
}

Maike::TargetTrivialOptions&
Maike::TargetTrivialOptions::configAppend(const TargetTrivialOptions& opts)
{
	m_build_cmd = opts.m_build_cmd;
	std::copy(std::begin(opts.m_deps), std::end(opts.m_deps), std::back_inserter(m_deps));
	return *this;
}

void Maike::TargetTrivialOptions::configDump(ResourceObject& options) const
{
	{
		auto cmd = options.createObject();
		m_build_cmd.configDump(cmd);
		options.objectSet("command", std::move(cmd));
	}

	{
		auto filename_ext = options.create(m_filename_ext.c_str());
		options.objectSet("filename_ext", std::move(filename_ext));
	}

	{
		auto deps = options.createArray();
		std::for_each(std::begin(m_deps), std::end(m_deps), [&deps](const Dependency& item) {
			auto dep = deps.createObject();
			item.dump(dep);
			deps.objectAppend(std::move(dep));
		});
		options.objectSet("dependencies", std::move(deps));
	}
}
