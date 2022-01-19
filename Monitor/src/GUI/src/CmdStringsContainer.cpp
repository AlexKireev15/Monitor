#include "../CmdStringsContainer.h"

void GUI::CmdStringsContainer::AddString(const char * string)
{
	m_strings.insert(m_strings.end() - 1, string, string + strlen(string));
	m_strings.insert(m_strings.end() - 1, '\n');
}

char * GUI::CmdStringsContainer::GetStrings()
{
	return m_strings.data();
}
