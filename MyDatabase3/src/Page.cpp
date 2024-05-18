#include "Page.h"
#include <string>





DefaultPage::DefaultPage(const PageHeader& pageHeader)
{
	m_Header = pageHeader;
	m_Data = new char[m_Header.NumBytes];
	memset(m_Data, 0, m_Header.NumBytes);
}

DefaultPage::~DefaultPage()
{
	delete[] m_Data;
}

DefaultPage::DefaultPage(const DefaultPage& other)
{
	m_Header = other.m_Header;
	m_Data = new char[m_Header.NumBytes];
	memcpy(m_Data, other.m_Data, m_Header.NumBytes);
}

DefaultPage::DefaultPage(DefaultPage&& other) noexcept
{
	m_Data = other.m_Data;
	other.m_Data = nullptr;
	m_Header = other.m_Header;
}