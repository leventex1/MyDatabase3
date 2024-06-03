#pragma once
#include <string>
#include "StorageEngine.h"
#include "Page.h"


class StorageEngineHeaderSerializer
{
public:
	StorageEngineHeaderSerializer(int version);

	EngineHeader Deserialize(const std::string& bytes) const;
	std::string Serialize(const EngineHeader& engineHeader) const;

private:
	int m_Version;
};


class IPageSerializer
{
public:
	IPageSerializer() = default;
	virtual ~IPageSerializer() = default;

	virtual std::unique_ptr<IPage> Deserialize(const std::string& bytes) const = 0;
	virtual std::string Serialize(const std::unique_ptr<IPage>& page) const = 0;
};


class DefaultPageSerializer : public IPageSerializer
{
public:
	DefaultPageSerializer(int version);

	std::unique_ptr<IPage> Deserialize(const std::string& bytes) const override;
	std::string Serialize(const std::unique_ptr<IPage>& page) const override;

private:
	int m_Version;
};
