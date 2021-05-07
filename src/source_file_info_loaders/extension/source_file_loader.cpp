//@	{
//@	 "targets":[{"name":"source_file_loader.o","type":"object"}]
//@	}

#include "./source_file_loader.hpp"

#include "src/io/input_buffer.hpp"
#include "src/io/output_buffer.hpp"

void Extension::SourceFileLoader::filterInput(
   Maike::Io::Reader input,
   Maike::SourceFileInfoLoaders::SourceOutStream source_stream,
   Maike::SourceFileInfoLoaders::TagsOutStream tag_stream) const
{
	Maike::Io::InputBuffer input_buff{input};
	Maike::Io::OutputBuffer tags{tag_stream};
	Maike::Io::OutputBuffer source{source_stream};
	enum class State : int
	{
		Newline,
		TagSymbol,
		Tag,
		SkipLine
	};
	auto state = State::Newline;
	std::string raw_string_tag;
	std::string raw_string_end_mark;
	while(true)
	{
		auto ch_in = input_buff.getchar();
		if(ch_in == -1) { return; }

		// For simplicity, copy raw stream to source output
		source.putchar(static_cast<std::byte>(ch_in));

		switch(state)
		{
			case State::Newline:
				switch(ch_in)
				{
					case '#':
						state = State::TagSymbol;
						tags.putchar(' ');
						break;

					default:
						if(!(ch_in >= '\0' && ch_in <= ' ')) { state = State::SkipLine; }
						else
						{
							tags.putchar(ch_in);
						}
						break;
				}
				break;

			case State::TagSymbol:
				switch(ch_in)
				{
					case '@':
						state = State::Tag;
						tags.putchar(' ');
						break;
					case '\n':
						state = State::Newline;
						tags.putchar('\n');
						break;
					default: state = State::SkipLine; break;
				}
				break;

			case State::Tag:
				if(ch_in == '\n') { state = State::Newline; }
				tags.putchar(static_cast<std::byte>(ch_in));
				break;

			case State::SkipLine:

				switch(ch_in)
				{
					case '\n':
						state = State::Newline;
						tags.putchar('\n');
						break;
					default: break;
				}
				break;
		}
	}
}
