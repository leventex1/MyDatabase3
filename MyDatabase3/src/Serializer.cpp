#include "Serializer.h"
#include "DBexceptions.h"
#include <sstream>


StorageEngineHeaderSerializer::StorageEngineHeaderSerializer(int version) : m_Version(version) { }

EngineHeader StorageEngineHeaderSerializer::Deserialize(const std::string& bytes) const
{
	try
	{
		switch (m_Version)
		{
			case 1: // [1:version][4:NumPages][4:FreeRecordIndex]
			{
				if (bytes.size() != 9 || bytes[0] != 1)
					break;
				const char* data = bytes.c_str();
				EngineHeader header;
				
				header.NumPages = *(int*)(data + 1);
				header.FreeRecordIndex = *(int*)(data + 1 + 4);
	
				return header;
			}
		}
	}
	catch (...)
	{
		throw DatabaseException("Failed to deserialize StorageEngineHeader at version: " + std::to_string(m_Version) + " with data: " + bytes);
	}

	throw DatabaseException("Unknown StorageEngineHeader deserialization version or serialization data!");
}

std::string StorageEngineHeaderSerializer::Serialize(const EngineHeader& header) const
{
	try
	{
		switch (m_Version)
		{
			case 1:  // [1:version][4:NumPages][4:FreeRecordIndex]
			{
				std::stringstream ss;
				ss << char(1) << header.NumPages << header.FreeRecordIndex;
				return ss.str();
			}
		}
	}
	catch (...)
	{
		throw DatabaseException("Failed to serialize StorageEngineHeader at version: " + std::to_string(m_Version));
	}

	throw DatabaseException("Unknown StorageEngineHeader serialization version or deserialization data!");
}


DefaultPageSerializer::DefaultPageSerializer(int version) : m_Version(version) { }

std::unique_ptr<IPage> DefaultPageSerializer::Deserialize(const std::string& bytes) const
{
	switch (m_Version)
	{
		case 1:  // [(4:NumBytes)(4:PageIndex):PageHeader][n{4096}:Data]
		{
			if (bytes.size() != (1 + 4 + 4 + 4096) || bytes[0] != 1)
				break;

			const char* data = bytes.c_str();

			std::unique_ptr<DefaultPage> page = std::make_unique<DefaultPage>();

			page->GetHeader().NumBytes = *(int*)(data + 1);
			page->GetHeader().PageIndex = *(int*)(data + 1 + 4);

			char* pageData = page->GetData();
			pageData = new char[page->GetHeader().NumBytes];
			memcpy(pageData, data + 1 + 2 * 4, page->GetHeader().NumBytes);

			return page;
		}
	}

	throw DatabaseException("Unknown DefaultPageSerializer deserialization version or serialization data! Num bytes: " + std::to_string(bytes.size()));
}

std::string DefaultPageSerializer::Serialize(const std::unique_ptr<IPage>& page) const
{
	switch (m_Version)
	{
		case 1: // [(4:NumBytes)(4:PageIndex):PageHeader][n:Data]
		{
			std::stringstream ss;
			ss << char(1) << page->GetHeader().NumBytes << page->GetHeader().PageIndex;
			ss << std::string(page->GetData(), page->GetHeader().NumBytes);
			return ss.str();
		}
	}
	throw DatabaseException("Unknown DefaultPageSerializer serialization version or deserialization data!");
}