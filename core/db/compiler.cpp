//@	{
//@	  "targets":[{"name":"compiler.o","type":"object"}]
//@	 }

#include "./compiler.hpp"

#include "core/exec/execve.hpp"

#include <algorithm>
#include <cstring>

Maike::Db::Compiler Maike::Db::operator|(Compiler const& a, Compiler const& b)
{
	if(auto b_origin = b.origin(); b_origin)
	{
		return Compiler{fs::path{b.recipe() != "" ? b.recipe() : a.recipe()},
		                b_origin,
		                KeyValueStore::Compound{a.config()} | b.config().reference()};
	}
	return Compiler{fs::path{b.recipe() != "" ? b.recipe() : a.recipe()},
	                a.origin(),
	                KeyValueStore::Compound{a.config()} | b.config().reference()};
}

namespace
{
	struct CmdOptsString
	{
		std::string buffer;
	};

	void write(CmdOptsString& str, char const* vals, size_t n)
	{
		std::copy_n(vals, n, std::back_inserter(str.buffer));
	}

	struct BufferReader
	{
		std::reference_wrapper<std::vector<std::byte> const> buffer;
		size_t read_offset;
	};

	size_t read(BufferReader& reader, std::byte* output, size_t n)
	{
		auto const bytes_left = std::size(reader.buffer.get()) - reader.read_offset;
		auto const bytes_to_read = std::min(n, bytes_left);
		memcpy(output, reader.buffer.get().data() + reader.read_offset, bytes_to_read);

		return bytes_to_read;
	}
}

void Maike::Db::Compiler::configRecipe()
{
	if(m_recipe == "") { return; }

	CmdOptsString cfg_str;
	store(m_config, cfg_str, KeyValueStore::PrettyPrint{false});

	std::vector<std::string> args{"configure", std::move(cfg_str.buffer)};
	auto result = Maike::Exec::execve(m_recipe, args);

	if(std::size(result.stderr()) != 0)
	{
		std::string str;
		std::transform(std::begin(result.stderr()),
		               std::end(result.stderr()),
		               std::back_inserter(str),
		               [](auto val) { return static_cast<char>(val); });

		fprintf(stderr, "%s: %s\n", m_recipe.c_str(), str.c_str());
	}

	if(failed(result))
	{
		std::string msg{"Failed to configure "};
		msg += m_recipe;
		throw std::runtime_error{std::move(msg)};
	}

	if(std::size(result.stdout()) != 0)
	{ m_config = KeyValueStore::Compound{BufferReader{result.stdout(), 0}, "<config output>"}; }
}

Maike::KeyValueStore::Compound Maike::Db::getTags(Compiler const& compiler,
                                                  fs::path const& src_file)
{
	KeyValueStore::Compound cmd_opts;
	cmd_opts.set("source_file", src_file.c_str());

	CmdOptsString str;
	store(cmd_opts, str, KeyValueStore::PrettyPrint{false});

	std::vector<std::string> args{"get_tags", std::move(str.buffer)};
	auto result = Maike::Exec::execve(
	   compiler.recipe() == "" ? makeShellExecPath(src_file) : compiler.recipe(), args);

	if(std::size(result.stderr()) != 0)
	{
		std::string str;
		std::transform(std::begin(result.stderr()),
		               std::end(result.stderr()),
		               std::back_inserter(str),
		               [](auto val) { return static_cast<char>(val); });

		fprintf(stderr, "%s: %s\n", src_file.c_str(), str.c_str());
	}

	if(failed(result))
	{
		std::string msg{"Failed to get additional use deps for "};
		msg += src_file;
		throw std::runtime_error{std::move(msg)};
	}

	if(std::size(result.stdout()) != 0)
	{
		auto str = src_file.string();
		return KeyValueStore::Compound{BufferReader{result.stdout(), 0}, str};
	}
	return KeyValueStore::Compound{};
}