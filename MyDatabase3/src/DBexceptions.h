#pragma once
#include <string>
#include <stdexcept>


class DatabaseException : public std::runtime_error
{
public:
	DatabaseException(const std::string& message) : std::runtime_error(message) { }
};


class InsertionException : public DatabaseException
{
public: 
	InsertionException(const std::string& message) : DatabaseException(message) { }
};


class RecordBuilderException : public DatabaseException
{
public:
	RecordBuilderException(const std::string& message) : DatabaseException(message) { }
};


class StorageManagerException : public DatabaseException
{
public:
	StorageManagerException(const std::string& message) : DatabaseException(message) { }
};


class StorageEngineException : public DatabaseException
{
public:
	StorageEngineException(const std::string& message) : DatabaseException(message) { }
};


class FileManagerExcpetion : public DatabaseException
{
public:
	FileManagerExcpetion(const std::string& message) : DatabaseException(message) { }
};


class ConditionException : public DatabaseException
{
public:
	ConditionException(const std::string& message) : DatabaseException(message) { }
};