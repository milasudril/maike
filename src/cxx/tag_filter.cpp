//@	{
//@	 "targets":[{"name":"tag_filter.o","type":"object"}]
//@	}

#include "./tag_filter.hpp"

#include "src/input_buffer.hpp"
#include "src/output_buffer.hpp"

void Maike::Cxx::TagFilter::run(Reader src,
                                SourceOutStream source_stream,
                                TagsOutStream tag_stream) const
{
	InputBuffer input{src};
	OutputBuffer tags{tag_stream};
	OutputBuffer source{source_stream};
	enum class State : int
	{
		Newline,
		CommentFirst,
		TagSymbol,
		Tag,
		SkipLine,
		CommentBlock,
		CommentBlockPreEnd,
		CommentFirstSkipLine,
		RawStringBegin,
		RawStringTag,
		RawString,
		RawStringPreEnd
	};
	auto state = State::Newline;
	std::string raw_string_tag;
	std::string raw_string_end_mark;
	while(true)
	{
		auto ch_in = input.getchar();
		if(ch_in == -1) { return; }

		// For simplicity, copy raw stream to source output
		source.putchar(static_cast<std::byte>(ch_in));

		switch(state)
		{
			case State::Newline:
				switch(ch_in)
				{
					case '/':
						state = State::CommentFirst;
						tags.putchar(' ');
						break;

					case 'R':
						state = State::RawStringBegin;
						raw_string_tag.clear();
						raw_string_end_mark.clear();
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

			case State::CommentFirst:
				switch(ch_in)
				{
					case '/':
						state = State::TagSymbol;
						tags.putchar(' ');
						break;
					case '*': state = State::CommentBlock; break;
					case '\n':
						state = State::Newline;
						tags.putchar('\n');
						break;

					default: state = State::SkipLine;
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
					case 'R':
						raw_string_tag.clear();
						raw_string_end_mark.clear();
						state = State::RawStringBegin;
						break;
					case '/': state = State::CommentFirstSkipLine; break;
					default: break;
				}
				break;

			case State::CommentBlock:
				switch(ch_in)
				{
					case '\n': tags.putchar('\n'); break;
					case '*': state = State::CommentBlockPreEnd; break;
				}
				break;

			case State::CommentBlockPreEnd:
				switch(ch_in)
				{
					case '/': state = State::SkipLine; break;
					case '\n':
						state = State::CommentBlock;
						tags.putchar('\n');
						break;
					default: state = State::CommentBlock; break;
				}
				break;

			case State::CommentFirstSkipLine:
				switch(ch_in)
				{
					case '*': state = State::CommentBlock; break;
					case '\n':
						state = State::Newline;
						tags.putchar('\n');
						break;
					default: state = State::SkipLine;
				}
				break;

			case State::RawStringBegin:
				switch(ch_in)
				{
					case '"': state = State::RawStringTag; break;
					case '\n':
						state = State::Newline;
						tags.putchar('\n');
						break;
					default: state = State::SkipLine;
				}
				break;

			case State::RawStringTag:
				switch(ch_in)
				{
					case '(': state = State::RawString; break;
					case '\n':
						state = State::Newline;
						tags.putchar('\n');
						break;
					default: raw_string_tag += ch_in;
				}
				break;

			case State::RawString:
				switch(ch_in)
				{
					case '\n': tags.putchar('\n'); break;
					case ')': state = State::RawStringPreEnd; break;
				}
				break;

			case State::RawStringPreEnd:
				switch(ch_in)
				{
					case '"':
						state = raw_string_end_mark == raw_string_tag ? State::SkipLine : State::RawString;
						raw_string_end_mark.clear();
						break;

					case '\n':
						state = State::RawString;
						tags.putchar('\n');
						break;

					default: raw_string_end_mark += ch_in;
				}
				break;
		}
	}
}