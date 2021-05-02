//@	{
//@	  "targets":[{"name":"compiler.o","type":"object"}]
//@	 }

#include "./compiler.hpp"

#include "src/exec/execve.hpp"

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
	if(m_recipe == "")
	{ return; }

	CmdOptsString cfg_str;
	store(m_config, cfg_str, KeyValueStore::PrettyPrint{false});

	std::vector<std::string> args{"configure", std::move(cfg_str.buffer)};
	auto result = Maike::Exec::execve(m_recipe, args);

	if(std::size(result.stderr()) != 0)
	{
		std::string str;
		std::transform(std::begin(result.stderr()), std::end(result.stderr()), std::back_inserter(str), [](auto val) {
			return static_cast<char>(val);
		});

		throw std::runtime_error{std::move(str)};
	}

	if(std::size(result.stdout()) != 0)
	{
		m_config = KeyValueStore::Compound{BufferReader{result.stdout(), 0}, "<config output>"};
	}
}