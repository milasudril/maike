	class ExitStatus
	{
	public:
		enum class Type
		{
			ReturnValue,
			Signal
		};

		constexpr explicit ExitStatus(Type type, int value): m_type{type}, m_value{value}
		{
		}

		constexpr int value() const
		{
			return m_value;
		}

		constexpr Type type() const
		{
			return m_type;
		}

	private:
		Type m_type;
		int m_value;
	};

	struct ExecResult
	{
		std::vector<std::byte> sysout;
		std::vector<std::byte> syserr;
		ExitStatus exit_status;
	};