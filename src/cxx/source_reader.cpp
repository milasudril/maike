//@	{
//@	 "targets":[{"name":"source_reader.o","type":"object"}]
//@	}

#include "./source_reader.hpp"

#include "src/input_buffer.hpp"

void Maike::Cxx::SourceReader::run(Reader src, std::vector<Dependency>& deps) const
{
	InputBuffer input{src};
	enum class State : int
	{
		Newline,
		CommentFirst,
		SkipLine,
		CommentBlock,
		CommentBlockPreEnd,
		RawStringBegin,
		RawStringTag,
		RawString,
		RawStringPreEnd,
		PreprocessorDirective,
		PreprocessorDirectivePreArg,
		IncludeProject,
		IncludeSystem
	};
	auto state = State::Newline;
	std::string raw_string_tag;
	std::string raw_string_end_mark;
	std::string preproc_directive;
	std::string include_path;

	while(true)
	{
		auto ch_in = input.getchar();
		if(ch_in == -1) { return; }

		switch(state)
		{
			case State::Newline:
				switch(ch_in)
				{
					case '/': state = State::CommentFirst; break;

					case 'R':
						state = State::RawStringBegin;
						raw_string_tag.clear();
						raw_string_end_mark.clear();
						break;

					case '#':
						state = State::PreprocessorDirective;
						preproc_directive.clear();
						break;

					default:
						if(!(ch_in >= '\0' && ch_in <= ' ')) { state = State::SkipLine; }
						break;
				}
				break;

			case State::CommentFirst:
				switch(ch_in)
				{
					case '*': state = State::CommentBlock; break;
					case '\n': state = State::Newline; break;

					default: state = State::SkipLine;
				}
				break;

			case State::SkipLine:
				switch(ch_in)
				{
					case '\n': state = State::Newline; break;
					case 'R':
						raw_string_tag.clear();
						raw_string_end_mark.clear();
						state = State::RawStringBegin;
						break;
					case '/': state = State::CommentFirst; break;
					default: break;
				}
				break;

			case State::CommentBlock:
				switch(ch_in)
				{
					case '*': state = State::CommentBlockPreEnd; break;
				}
				break;

			case State::CommentBlockPreEnd:
				switch(ch_in)
				{
					case '/': state = State::SkipLine; break;
					case '\n': state = State::CommentBlock; break;
					default: state = State::CommentBlock; break;
				}
				break;

			case State::RawStringBegin:
				switch(ch_in)
				{
					case '"': state = State::RawStringTag; break;
					case '\n': state = State::Newline; break;
					default: state = State::SkipLine;
				}
				break;

			case State::RawStringTag:
				switch(ch_in)
				{
					case '(': state = State::RawString; break;
					case '\n': state = State::Newline; break;
					default: raw_string_tag += ch_in;
				}
				break;

			case State::RawString:
				switch(ch_in)
				{
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

					case '\n': state = State::RawString; break;

					default: raw_string_end_mark += ch_in;
				}
				break;

			case State::PreprocessorDirective:
				if(ch_in >= '\0' && ch_in <= ' ')
				{
					state = preproc_directive == "include" ? State::PreprocessorDirectivePreArg : State::SkipLine;
				}
				else
				{
					preproc_directive += ch_in;
				}
				break;

			case State::PreprocessorDirectivePreArg:
				switch(ch_in)
				{
					case '\n': state = State::Newline; break;

					case '"': state = State::IncludeProject; break;

					case '<': state = State::IncludeSystem; break;

					default: state = State::SkipLine; break;
				}
				break;

			case State::IncludeProject:
				switch(ch_in)
				{
					case '"':
						state = State::SkipLine;
						deps.push_back(
						   Dependency{fs::path{std::move(include_path)}, Dependency::Resolver::InternalLookup});
						break;

					case '\n': state = State::Newline; break;

					default: include_path += ch_in; break;
				}
				break;

			case State::IncludeSystem:
				switch(ch_in)
				{
					case '>':
						state = State::SkipLine;
						deps.push_back(Dependency{fs::path{std::move(include_path)}, Dependency::Resolver::None});
						break;

					case '\n': state = State::Newline; break;

					default: include_path += ch_in; break;
				}
				break;
		}
	}
}