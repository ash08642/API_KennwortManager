#include "PostgreSQLCleint.h"

PostgreSQLCleint::PostgreSQLCleint(std::string connectionString)
{
    connection = new pqxx::connection{ connectionString };

	initializeTables();

	std::cout << connection->dbname() << std::endl;
}

PostgreSQLCleint::~PostgreSQLCleint()
{
	delete connection;
}

void PostgreSQLCleint::initializeTables()
{
	pqxx::work txn{ *connection };
	txn.exec("CREATE TABLE IF NOT EXISTS master_accounts( user_identity varchar(80) PRIMARY KEY, pass varchar(64)) ");
	txn.exec("CREATE TABLE IF NOT EXISTS saved_accounts( saved_account_id SERIAL PRIMARY KEY, master_account_identity varchar(64), platform varchar(100), saved_account_identity varchar(80), pass varchar(300), created_at date, updated_at date, salt varchar(80) )");
	txn.commit();
}

std::string PostgreSQLCleint::insert(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values)
{
	std::string queryString =
		"INSERT INTO " + table_name + " ( " + keys[0];
	for (size_t i = 1; i < keys.size(); i++)
	{
		queryString += ", " + keys[i];
	}
	queryString += " ) VALUES ( '" + values[0];

	bool addQuate = true;
	for (size_t i = 1; i < values.size(); i++)
	{
		if (!values[i].compare("date_now"))
		{
			if (addQuate)
			{
				queryString += "', NOW()";
				addQuate = false;
			}
			else
			{
				queryString += ", NOW()";
				addQuate = false;
			}
		}
		else if (!values[i].compare("date_null"))
		{
			if (addQuate)
			{
				queryString += "', NULL";
				addQuate = false;
			}
			else
			{
				queryString += ", NULL";
				addQuate = false;
			}
		}
		else
		{
			if (addQuate)
			{
				queryString += "', '" + values[i];
				addQuate = true;
			}
			else
			{
				queryString += ", '" + values[i];
				addQuate = true;
			}
		}
	}
	if (addQuate)
	{
		queryString += "')";
	}
	else
	{
		queryString += ")";
	}

	pqxx::work txn{ *connection };

	try
	{
		txn.exec(queryString);
	}
	catch (const std::exception&)
	{
		return "error";
	}
	txn.commit();

	return "succes from api";
}

std::string PostgreSQLCleint::insertAndReturnId(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values, std::string id_field_name)
{
	std::string queryString =
		"INSERT INTO " + table_name + " ( " + keys[0];
	for (size_t i = 1; i < keys.size(); i++)
	{
		queryString += ", " + keys[i];
	}
	queryString += " ) VALUES ( '" + values[0];

	bool addQuate = true;
	for (size_t i = 1; i < values.size(); i++)
	{
		if (!values[i].compare("date_now"))
		{
			if (addQuate)
			{
				queryString += "', NOW()";
				addQuate = false;
			}
			else
			{
				queryString += ", NOW()";
				addQuate = false;
			}
		}
		else if (!values[i].compare("date_null"))
		{
			if (addQuate)
			{
				queryString += "', NULL";
				addQuate = false;
			}
			else
			{
				queryString += ", NULL";
				addQuate = false;
			}
		}
		else
		{
			if (addQuate)
			{
				queryString += "', '" + values[i];
				addQuate = true;
			}
			else
			{
				queryString += ", '" + values[i];
				addQuate = true;
			}
		}
	}
	if (addQuate)
	{
		queryString += "') RETURNING " + id_field_name;
	}
	else
	{
		queryString += ") RETURNING " + id_field_name;
	}

	pqxx::work txn{ *connection };

	try
	{
		pqxx::result r = txn.exec(queryString);
		txn.commit();
		return r[0][0].c_str();
	}
	catch (const std::exception&)
	{
		return "error";
	}
}

std::string PostgreSQLCleint::updateById(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values, int id_field_index)
{
	std::string queryString = "UPDATE " + table_name + " SET ";
	if (id_field_index == 0)
	{
		queryString += keys[1] + " = '" + values[1] + "'";
		for (size_t i = 2; i < keys.size(); i++)
		{
			queryString += ", " + keys[i] + " = '" + values[i] + "'";
		}
		queryString += " WHERE " + keys[0] + " = " + values[0];
	}
	else
	{
		queryString += keys[0] + " = '" + values[0] + "'";
		for (size_t i = 1; i < keys.size(); i++)
		{
			if (i == id_field_index)
			{
				continue;
			}
			queryString += ", " + keys[i] + " = '" + values[i] + "'";
		}
		queryString += " WHERE " + keys[id_field_index] + " = " + values[id_field_index];
	}

	pqxx::work txn{ *connection };

	try
	{
		txn.exec(queryString);
	}
	catch (const std::exception&)
	{
		return "error";
	}
	txn.commit();

	return "succes from api";
}

std::string PostgreSQLCleint::deleteByValues(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values)
{
	std::string queryString = "DELETE FROM " + table_name + " WHERE ";
	queryString += keys[0] + " = " + values[0];
	for (size_t i = 1; i < keys.size(); i++)
	{
		queryString += " AND " + keys[i] + " = '" + values[i] + "'";
	}

	pqxx::work txn{ *connection };

	try
	{
		txn.exec(queryString);
	}
	catch (const std::exception&)
	{
		return "error";
	}
	txn.commit();

	return "succes from api";
}

std::string PostgreSQLCleint::getByName(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values)
{
	std::string queryString = "SELECT * FROM " + table_name + " WHERE " + keys[0] + " = '" + values[0] + "'";
	for (size_t i = 1; i < keys.size(); i++)
	{
		queryString += " AND " + keys[i] + " = '" + values[i] + "'";
	}

	try
	{
		std::stringstream ss;
		pqxx::work txn{ *connection };
		pqxx::result r = txn.exec(queryString);
		for (auto const& row : r)
		{
			for (auto const& field : row) ss << field.c_str() << ";col;";
			ss << ";spal;";
		}
		return ss.str();
	}
	catch (const std::exception&)
	{
		return "error";
	}
}

bool PostgreSQLCleint::_exists(std::string table_name, std::vector<std::string> keys, std::vector<std::string> values)
{
	std::string queryString =
		"SELECT COUNT(*) FROM " + table_name + " WHERE " + keys[0] + " = '" + values[0] + "'";
	for (size_t i = 1; i < keys.size(); i++)
	{
		queryString += " AND " + keys[i] + " = '" + values[i] + "'";
	}
	pqxx::work txn{ *connection };
	pqxx::result r = txn.exec(queryString);
	int i = from_string<int>(r[0][0]);
	if (i > 0)
	{
		return true;
	}
	return false;
}
