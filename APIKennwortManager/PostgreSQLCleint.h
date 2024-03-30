#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <pqxx/pqxx>

class PostgreSQLCleint
{
public:
	PostgreSQLCleint(std::string connectionString);
	~PostgreSQLCleint();

	void initializeTables();

	std::string insert(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values);
	std::string insertAndReturnId(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values, std::string id_field_name);

	std::string updateById(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values, int id_field_index);

	std::string deleteByValues(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values);

	std::string getByName(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values);

	bool _exists(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values);

private:
	pqxx::connection* connection;
};

