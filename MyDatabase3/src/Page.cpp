#include "Page.h"
#include <string>





DefaultPage::DefaultPage(const PageHeader& pageHeader)
{
	m_Header = pageHeader;
	m_Data = new char[m_Header.NumBytes];
}

DefaultPage::~DefaultPage()
{
	delete[] m_Data;
}


Page::Page(int numBytes)
{
	m_Page = new char[numBytes];
	m_Header.NumBytes = numBytes;

	memset(m_Page, 0, numBytes);
}

Page::~Page()
{
	if(m_Page)
		delete[] m_Page;
}

Page::Page(const Page& other)
{
	m_Header = other.m_Header;
	m_Page = new char[m_Header.NumBytes];
	memcpy(m_Page, other.m_Page, m_Header.NumBytes);
}

Page::Page(Page&& other) noexcept
{
	m_Page = other.m_Page;
	other.m_Page = nullptr;
	m_Header = other.m_Header;
}