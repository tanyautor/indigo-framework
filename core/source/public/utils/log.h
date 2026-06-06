#pragma once

namespace indigo_log
{
	enum Severity : uint32
	{
		INFO = 0,
		WARNING = 1,
		ERROR = 2,
		FATAL = 3,
	};

	static constexpr auto magenta = "\033[35m";
	static constexpr auto green = "\033[32m";
	static constexpr auto yellow = "\033[33m";
	static constexpr auto red = "\033[31m";
	static constexpr auto white = "\033[37m";
	static constexpr auto reset = "\033[0m";
	static inline const char* GetColor(Severity _ser)
	{
		switch (_ser)
		{
		case indigo_log::INFO:
			return green;
			break;
		case indigo_log::WARNING:
			return yellow;
			break;
		case indigo_log::ERROR:
			return red;
			break;
		case indigo_log::FATAL:
			return magenta;
			break;
		default:
			return white;
			break;
		}
	}

	template <typename FormatString, typename... Args>
	static inline void log(Severity _ser, FormatString _c_string, const Args&... _args)
	{
		std::string severity(magic_enum::enum_name(_ser));
		printf("indigo_log [%s%s%s] ", GetColor(_ser), severity.c_str(), reset);
		fmt::print(fmt::runtime(_c_string), _args...);
		printf("\n");
	}
}