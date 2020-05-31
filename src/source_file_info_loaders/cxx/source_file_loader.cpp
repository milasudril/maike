//@	{
//@	 "targets":[{"name":"source_file_loader.o","type":"object"}]
//@	}

#include "./source_file_loader.hpp"

#include "src/io/input_buffer.hpp"
#include "src/io/output_buffer.hpp"

std::vector<Maike::Db::Dependency>
Cxx::SourceFileLoader::getDependencies(Maike::Io::Reader input) const
{
	Maike::Io::InputBuffer input_buff{input};
	std::vector<Maike::Db::Dependency> deps;
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
		PrePreprocessorDirective,
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
		auto ch_in = input_buff.getchar();
		if(ch_in == -1) { return deps; }

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
						state = State::PrePreprocessorDirective;
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

			case State::PrePreprocessorDirective:
				if(!(ch_in >= '\0' && ch_in <= ' '))
				{
					preproc_directive += ch_in;
					state = State::PreprocessorDirective;
				}
				break;

			case State::PreprocessorDirective:
				switch(ch_in)
				{
					case '<':
						state = preproc_directive == "include" ? State::IncludeSystem : State::SkipLine;
						break;
					case '"':
						state = preproc_directive == "include" ? State::IncludeProject : State::SkipLine;
						break;
					default:
						if(ch_in >= '\0' && ch_in <= ' ') { state = State::PreprocessorDirectivePreArg; }
						else
						{
							preproc_directive += ch_in;
						}
						break;
				}
				break;

			case State::PreprocessorDirectivePreArg:
				switch(ch_in)
				{
					case '\n': state = State::Newline; break;

					case '"': state = State::IncludeProject; break;

					case '<': state = State::IncludeSystem; break;

					default:
						if(!(ch_in >= '\0' && ch_in <= ' '))
						{
							state = State::SkipLine;
							break;
						}
				}
				break;

			case State::IncludeProject:
				switch(ch_in)
				{
					case '"':
						state = State::SkipLine;
						deps.push_back(Maike::Db::Dependency{
						   Maike::fs::path{std::move(include_path)},
						   Maike::Db::Dependency::Resolver::InternalLookup});
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
						deps.push_back(
						   Maike::Db::Dependency{Maike::fs::path{std::move(include_path)},
						                                   Maike::Db::Dependency::Resolver::None});
						break;

					case '\n': state = State::Newline; break;

					default: include_path += ch_in; break;
				}
				break;
		}
	}
}

void Cxx::SourceFileLoader::filterInput(
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
		auto ch_in = input_buff.getchar();
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
