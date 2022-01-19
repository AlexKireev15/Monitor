#pragma once

#include <memory>
#include <vector>

namespace GUI
{
	class CmdStringsContainer
	{
	public:
		CmdStringsContainer() { m_strings.reserve(1024); m_strings.push_back('\0'); }

		void AddString(const char * string);
		char * GetStrings();
	private:
		std::vector<char> m_strings;
	};
}