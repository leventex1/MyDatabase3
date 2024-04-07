#pragma once
#include <memory>


struct PageHeader
{
	PageHeader() = default;
	PageHeader(int numBytes, int pageIndex) 
	:	NumBytes(numBytes),
		PageIndex(pageIndex)
	{ }
	
	int NumBytes = 0;
	int PageIndex = 0;
};


class IPage
{
public:
	virtual ~IPage() = default;
	virtual const char* GetData() const = 0;
	virtual char* GetData() = 0;
	virtual const PageHeader& GetHeader() const = 0;
	virtual PageHeader& GetHeader() = 0;
};


class IPageFactory
{
public:
	virtual ~IPageFactory() = default;
	virtual void ConstructPage() = 0;
	virtual std::unique_ptr<IPage> GetPage() = 0;
};


class DefaultPage : public IPage
{
public:
	DefaultPage(const PageHeader& pageHeader);
	virtual ~DefaultPage();

	virtual const char* GetData() const override { return m_Data; }
	virtual char* GetData() override { return m_Data; }
	virtual const PageHeader& GetHeader() const override { return m_Header; }
	virtual PageHeader& GetHeader() override { return m_Header; }

private:
	PageHeader m_Header;
	char* m_Data = nullptr;
};


class DefaultPageFactory : public IPageFactory
{
public:
	DefaultPageFactory(int numPageBytes) : m_NumPageBytes(numPageBytes) { }
	virtual ~DefaultPageFactory() = default;
	virtual void ConstructPage() override { m_Page = std::make_unique<DefaultPage>(PageHeader(m_NumPageBytes, 0)); }
	virtual std::unique_ptr<IPage> GetPage() override { return std::move(m_Page); }
private:
	int m_NumPageBytes;
	std::unique_ptr<IPage> m_Page;
};


class Page
{
public:
	Page(int numBytes);
	Page(const Page& other);
	Page(Page&& other) noexcept;
	~Page();

	inline const char* GetPage() const { return m_Page; }
	inline char* GetPage() { return m_Page; }

	inline const PageHeader& GetHeader() const { return m_Header; }
	inline PageHeader& GetHeader() { return m_Header; }

private:
	PageHeader m_Header;
	char* m_Page = nullptr;
};
