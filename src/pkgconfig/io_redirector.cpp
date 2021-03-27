//@	{
//@	 "targets":[{"name":"io_redirector.o","type":"object"}]
//@	}

#include "./io_redirector.hpp"

void Maike::PkgConfig::IoRedirector::operator()(void const* buffer,
                                                size_t buffer_size,
                                                Io::Redirector::StdOut)
{
	if(buffer_size == 0)
	{
		if(m_token.size() != 0) { m_stdout.push_back(std::move(m_token)); }
		return;
	}

	auto pos = reinterpret_cast<char const*>(buffer);
	auto end = pos + buffer_size;
	while(pos != end)
	{
		auto ch_in = *pos;
		++pos;
		switch(m_parser_state)
		{
			case StdOutParserState::Normal:
				switch(ch_in)
				{
					case '\\': m_parser_state = StdOutParserState::Escape; break;

					default:
						if(ch_in >= 0 && ch_in <= ' ') { m_stdout.push_back(std::move(m_token)); }
						else
						{
							m_token += ch_in;
						}
				}
				break;

			case StdOutParserState::Escape:
				m_parser_state = StdOutParserState::Normal;
				m_token += ch_in;
				break;
		}
	}

	if(m_token.size() != 0) { m_stdout.push_back(std::move(m_token)); }
}

void Maike::PkgConfig::IoRedirector::operator()(void const* buffer,
                                                size_t buffer_size,
                                                Io::Redirector::StdErr)
{
	auto ptr = reinterpret_cast<char const*>(buffer);
	std::copy(ptr, ptr + buffer_size, std::back_inserter(m_stderr));
}
