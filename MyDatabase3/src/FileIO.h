#pragma once
#include <iostream>
#include "DBexceptions.h"


class FileIOException : public DatabaseException
{
public:
	FileIOException(const std::string& message) : DatabaseException(message) { }
};


class IFileIO
{
public:
	virtual ~IFileIO() = default;

	/*
		path: "path/to/folder"
		name: "filename" without extension
		content: actual bytes.
	*/
	virtual void WriteFile(const std::string& path, const std::string& name, const std::string& content) const = 0;

	/*
		path: "path/to/folder"
		name: "filename" without extension

		returns buffer of the bytes.
	*/
	virtual std::string ReadFile(const std::string& path, const std::string& name) const = 0;

protected:
	void _WriteFile_(std::ofstream& file, const std::string& content) const;
	std::string _ReadFile_(std::ifstream& file) const;
};


class BinaryFileIO : public IFileIO
{
public:
	virtual void WriteFile(const std::string& path, const std::string& name, const std::string& content) const override;
	virtual std::string ReadFile(const std::string& path, const std::string& name) const override;
};


class TextFileIO : public IFileIO
{
public:
	virtual void WriteFile(const std::string& path, const std::string& name, const std::string& content) const override;
	virtual std::string ReadFile(const std::string& path, const std::string& name) const override;
};
