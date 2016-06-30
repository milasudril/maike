//@	{"targets":[{"name":"expressionevaluatordefault.o","type":"object"}]}

#include "expressionevaluatordefault.hpp"
#include "stringkey.hpp"
#include "sysvars.hpp"
#include "variant.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"
#include "twins.hpp"
#include "versionnumber.hpp"
#include <string>
#include <stack>
#include <vector>
#include <cstring>
#include <cstdlib>

using namespace Maike;

ExpressionEvaluatorDefault::ExpressionEvaluatorDefault()
	{}

ExpressionEvaluatorDefault::~ExpressionEvaluatorDefault()
	{}

ExpressionEvaluatorDefault& ExpressionEvaluatorDefault::variablesClear() noexcept
	{
	m_variables.clear();
	return *this;
	}

ExpressionEvaluatorDefault& ExpressionEvaluatorDefault::sysvarsLoad()
	{
	Maike::sysvarsLoad(m_variables);
	return *this;
	}

Variant ExpressionEvaluatorDefault::variableGet(const Stringkey& variable) const noexcept
	{
	auto i=m_variables.find(variable);
	if(i==m_variables.end())
		{return 0;}

	return i->second;
	}


namespace
	{
	struct Command
		{
		Command():name("nop"){}
		Stringkey name;
		std::vector< Variant > args;
		};

	static Variant argCreate(const char* value,bool string_is
		,std::vector<std::string>& strings
		,const ExpressionEvaluatorDefault& evaluator)
		{
		if(string_is)
			{
			strings.push_back(value);
			return strings.back().c_str();
			}

		if(!(value[0]>='0' && value[0]<='9')) //value is an identifier
			{
			return evaluator.variableGet(Stringkey(value));
			}

		if(strchr(value,'.')!=NULL)
			{
			char* ptr_end;
			auto x=strtod(value,&ptr_end);
			if(*ptr_end!='\0')
				{
				exceptionRaise(ErrorMessage("Expression error: "
					"\"#0;\" is not a valid floating point number.",{value}));
				}
			return x;
			}
		return static_cast<int64_t>( atoll(value) );
		}

	template<class T>
	T add(Twins<const Variant*> values,uintptr_t type_tag)
		{
		T ret=0;
		while(values.first!=values.second)
			{
			if(values.first->typeGet()!=type_tag)
				{exceptionRaise(ErrorMessage("Expression error: Inconsistent types.",{}));}
			ret+=static_cast<T>( *values.first );
			++values.first;
			}
		return ret;
		}

	template<class T>
	T multiply(Twins<const Variant*> values,uintptr_t type_tag)
		{
		T ret=1;
		while(values.first!=values.second)
			{
			if(values.first->typeGet()!=type_tag)
				{exceptionRaise(ErrorMessage("Expression error: Inconsistent types.",{}));}
			ret*=static_cast<T>( *values.first );
			++values.first;
			}
		return ret;
		}

	template<class T>
	int64_t and_(Twins<const Variant*> values,uintptr_t type_tag)
		{
		while(values.first!=values.second)
			{
			if(values.first->typeGet()!=type_tag)
				{exceptionRaise(ErrorMessage("Expression error: Inconsistent types.",{}));}
			if(static_cast<T>(*values.first)==0)
				{return 0;}
			++values.first;
			}
		return 1;
		}

	template<class T>
	int64_t or_(Twins<const Variant*> values,uintptr_t type_tag)
		{
		while(values.first!=values.second)
			{
			if(values.first->typeGet()!=type_tag)
				{exceptionRaise(ErrorMessage("Expression error: Inconsistent types.",{}));}
			if(static_cast<T>(*values.first)!=0)
				{return 1;}
			++values.first;
			}
		return 0;
		}

	template<class T>
	int64_t less_than(Twins<const Variant*> values,uintptr_t type_tag)
		{
		auto x_first=static_cast<T>(*values.first);
		++values.first;
		while(values.first!=values.second)
			{
			if(values.first->typeGet()!=type_tag)
				{exceptionRaise(ErrorMessage("Expression error: Inconsistent types.",{}));}
			auto x=static_cast<T>(*values.first);
			if(x_first > x)
				{return 0;}
			x_first=x;
			++values.first;
			}
		return 1;
		}

	template<class T>
	int64_t greater_than(Twins<const Variant*> values,uintptr_t type_tag)
		{
		auto x_first=static_cast<T>(*values.first);
		++values.first;
		while(values.first!=values.second)
			{
			if(values.first->typeGet()!=type_tag)
				{exceptionRaise(ErrorMessage("Expression error: Inconsistent types.",{}));}
			auto x=static_cast<T>(*values.first);
			if(x_first < x)
				{return 0;}
			x_first=x;
			++values.first;
			}
		return 1;
		}

	template<class T>
	int64_t equals(Twins<const Variant*> values,uintptr_t type_tag)
		{
		auto x_first=static_cast<T>(*values.first);
		++values.first;
		while(values.first!=values.second)
			{
			if(values.first->typeGet()!=type_tag)
				{exceptionRaise(ErrorMessage("Expression error: Inconsistent types.",{}));}
			auto x=static_cast<T>(*values.first);
			if(x != x_first)
				{return 0;}
			x_first=x;
			++values.first;
			}
		return 1;
		}


	Variant execute(const Command& cmd)
		{
		if(cmd.args.empty())
			{
			exceptionRaise(ErrorMessage("Expression error: A function needs at "
				"least one argument.",{}));
			}

		if(cmd.name==Stringkey("add"))
			{
			auto ptr=cmd.args.data();
			auto type=ptr->typeGet();
			switch(type)
				{
				case Variant::DOUBLE:
					return add<double>({ptr,ptr+cmd.args.size()},type);
				case Variant::INT:
					return add<int64_t>({ptr,ptr+cmd.args.size()},type);
				default:
					exceptionRaise(ErrorMessage("Expression error: The 'add' function "
						"requires either an int or a double.",{}));
				}
			}
		else
		if(cmd.name==Stringkey("multiply"))
			{
			auto ptr=cmd.args.data();
			auto type=ptr->typeGet();
			switch(type)
				{
				case Variant::DOUBLE:
					return multiply<double>({ptr,ptr+cmd.args.size()},type);
				case Variant::INT:
					return multiply<int64_t>({ptr,ptr+cmd.args.size()},type);
				default:
					exceptionRaise(ErrorMessage("Expression error: The 'multiply' function "
						"requires either an int or a double.",{}));
				}
			}
		else
		if(cmd.name==Stringkey("and"))
			{
			auto ptr=cmd.args.data();
			auto type=ptr->typeGet();
			switch(type)
				{
				case Variant::INT:
					return and_<int64_t>({ptr,ptr+cmd.args.size()},type);
				case Variant::BOOL:
					return and_<bool>({ptr,ptr+cmd.args.size()},type);
				default:
					exceptionRaise(ErrorMessage("Expression error: The 'and' function "
						"requires either an int or a bool.",{}));
				}
			}
		else
		if(cmd.name==Stringkey("or"))
			{
			auto ptr=cmd.args.data();
			auto type=ptr->typeGet();
			switch(type)
				{
				case Variant::INT:
					return or_<int64_t>({ptr,ptr+cmd.args.size()},type);
				case Variant::BOOL:
					return or_<bool>({ptr,ptr+cmd.args.size()},type);
				default:
					exceptionRaise(ErrorMessage("Expression error: The 'or' function "
						"requires either an int or a bool.",{}));
				}
			}
		else
		if(cmd.name==Stringkey("not"))
			{
			auto ptr=cmd.args.data();
			auto type=ptr->typeGet();

			switch(type)
				{
				case Variant::INT:
					return static_cast<int64_t>( !static_cast<int64_t>(ptr[0]) );
				case Variant::BOOL:
					return !static_cast<bool>(ptr[0]);
				default:
					exceptionRaise(ErrorMessage("Expression error: The 'not' function "
						"requires either an int or a bool.",{}));
				}

			return static_cast<int64_t>(version(static_cast<const char*>(ptr[0])));
			}
		else
		if(cmd.name==Stringkey("less_than"))
			{
			auto N=cmd.args.size();
			if(N<2)
				{
				exceptionRaise(ErrorMessage("Expression error: The 'less_than' function "
					"requires at least two arguments.",{}));
				}
			auto ptr=cmd.args.data();
			auto type=ptr->typeGet();
			switch(type)
				{
				case Variant::DOUBLE:
					return less_than<double>({ptr,ptr+N},type);
				case Variant::INT:
					return less_than<int64_t>({ptr,ptr+N},type);
				default:
					exceptionRaise(ErrorMessage("Expression error: The 'less_than' function "
						"requires either an int or a double.",{}));
				}
			}
		else
		if(cmd.name==Stringkey("greater_than"))
			{
			auto N=cmd.args.size();
			if(N<2)
				{
				exceptionRaise(ErrorMessage("Expression error: The 'greater_than' function "
					"requires at least two arguments.",{}));
				}
			auto ptr=cmd.args.data();
			auto type=ptr->typeGet();
			switch(type)
				{
				case Variant::DOUBLE:
					return greater_than<double>({ptr,ptr+N},type);
				case Variant::INT:
					return greater_than<int64_t>({ptr,ptr+N},type);
				default:
					exceptionRaise(ErrorMessage("Expression error: The 'greater_than' function "
						"requires either an int or a double.",{}));
				}
			}
		else
		if(cmd.name==Stringkey("equals"))
			{
			auto N=cmd.args.size();
			if(N<2)
				{
				exceptionRaise(ErrorMessage("Expression error: The 'equals' function "
					"requires at least two arguments.",{}));
				}
			auto ptr=cmd.args.data();
			auto type=ptr->typeGet();

			switch(type)
				{
				case Variant::INT:
					return equals<int64_t>({ptr,ptr+N},type);
				case Variant::BOOL:
					return equals<int64_t>({ptr,ptr+N},type);
				default:
					exceptionRaise(ErrorMessage("Expression error: The 'equals' function "
						"requires either an int or a bool.",{}));
				}

			return static_cast<int64_t>(version(static_cast<const char*>(ptr[0])));
			}
		else
		if(cmd.name==Stringkey("version"))
			{
			auto ptr=cmd.args.data();
			if(ptr->typeGet()!=Variant::STRING)
				{exceptionRaise(ErrorMessage("Expression error: 'version' expects a string.",{}));}
			return static_cast<int64_t>(version(static_cast<const char*>(ptr[0])));
			}

		exceptionRaise(ErrorMessage("Expression error: Unknown function.",{}));
		}
	};

Variant ExpressionEvaluatorDefault::evaluate(const char* expression) const
	{
	Variant ret=static_cast<int64_t>(0);
	enum class State:int{NORMAL,STRING,ESCAPE};

	auto state=State::NORMAL;
	auto state_old=state;


	std::vector<std::string> strings;

	std::pair<std::string,bool> tok_current;
	Command cmd_current;

	std::stack<Command> cmdstack;

	while(true)
		{
		auto ch_in=*expression;
		switch(state)
			{
			case State::NORMAL:
				{
				switch(ch_in)
					{
					case '(':
						cmdstack.push(cmd_current);
						cmd_current.name=Stringkey( tok_current.first.c_str() );
						cmd_current.args.clear();
						tok_current.first.clear();
						break;
					case ',':
					case '\0':
						if(tok_current.first.size()!=0)
							{
							ret=argCreate(tok_current.first.c_str(),tok_current.second
								,strings,*this);
							cmd_current.args.push_back(ret);
							tok_current.first.clear();
							tok_current.second=0;
							}
						if(ch_in=='\0')
							{
							if(!cmdstack.empty())
								{exceptionRaise(ErrorMessage("Expression error: Missing ')'.",{}));}
							return ret;
							}
						break;
					case ')':
						if(cmdstack.empty())
							{
							exceptionRaise(ErrorMessage("Expression error: "
								"Unexpected ')' after #0;.",{tok_current.first.c_str()}));
							}
						if(tok_current.first.size()!=0)
							{
							cmd_current.args.push_back(argCreate(tok_current.first.c_str()
								,tok_current.second,strings,*this));
							}
						ret=execute(cmd_current);
						tok_current.first.clear();
						tok_current.second=0;
						cmd_current=std::move( cmdstack.top() );
						cmd_current.args.push_back(ret);
						cmdstack.pop();
						break;
					case '\'':
						state_old=state;
						state=State::STRING;
						break;
					case '\\':
						state_old=state;
						state=State::ESCAPE;
						break;
					default:
						if(!(ch_in>='\0' && ch_in<=' '))
							{tok_current.first+=ch_in;}
					};
				}
				break;

			case State::STRING:
				{
				switch(ch_in)
					{
					case '\'':
						state=state_old;
						tok_current.second=1;
						break;
					case '\\':
						state=State::ESCAPE;
						break;
					case '\0':
						exceptionRaise(ErrorMessage("Expression error: "
							"Unterminated string constant.",{}));
					default:
						tok_current.first+=ch_in;
					};
				}
				break;

			case State::ESCAPE:
				if(ch_in=='\0')
					{
					exceptionRaise(ErrorMessage("Expression error: "
						"Expected character after '\\'.",{}));
					}
				tok_current.first+=ch_in;
				state=state_old;
				break;
			}
		++expression;
		}
	}
