#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <httplib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "Crypto.h"
#include "PostgreSQLCleint.h"

int main(void)
{
    httplib::Server svr;

    std::fstream new_file;
    new_file.open("Files/DB_Connection.txt", std::ios::in);
    std::string connectionString;
    if (new_file.is_open()) {
        getline(new_file, connectionString);
        std::cout << connectionString << "\n";
        new_file.close();
    }
    else
    {
        std::cout << "Could not Open DB_Connection.txt" << std::endl;
    }

    PostgreSQLCleint postgreSQLCleint = PostgreSQLCleint(connectionString);

    svr.Get("/hi", [](const  httplib::Request& req, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
        std::cout << "get req" << std::endl;
        });

    svr.Get("/login", [&](const  httplib::Request& req, httplib::Response& res) {
        std::vector<std::string> keys{ "user_identity", "pass" };
        std::vector<std::string> values{
            req.get_param_value("identity"),
            Crypto::Hash2_SHA256(req.get_param_value("password"), 1000, req.get_param_value("identity"))
        };
        std::string msg = "succes";
        if (!postgreSQLCleint._exists("master_accounts", keys, values))
        {
            msg = "error";
        }
        res.set_content(msg, "text/plain");
        });

    svr.Get("/getSavedAccounts", [&](const  httplib::Request& req, httplib::Response& res) {
        std::vector<std::string> keys{ "master_account_identity" };
        std::vector<std::string> values{ Crypto::Hash2_SHA256(req.get_param_value("master_account_identity"), 1000, req.get_param_value("master_account_identity")) };
        std::string msg = postgreSQLCleint.getByName("saved_accounts", keys, values);
        res.set_content(msg, "text/plain");
        });

    svr.Post("/signUp", [&](const auto& req, auto& res) {
        std::vector<std::string> keys{ "user_identity", "pass" };
        std::vector<std::string> values{
            req.get_file_value("identity").content,
            Crypto::Hash2_SHA256(req.get_file_value("password").content, 1000, req.get_file_value("identity").content)
        };
        std::string msg = postgreSQLCleint.insert("master_accounts", keys, values);
        res.set_content(msg, "text/plain");
        });

    svr.Post("/addAccount", [&](const auto& req, auto& res) {
        std::vector<std::string> keys{ "master_account_identity", "platform", "saved_account_identity", "pass", "created_at", "updated_at", "salt" };
        std::vector<std::string> values{ 
            Crypto::Hash2_SHA256(req.get_file_value("master_account_identity").content, 1000, req.get_file_value("master_account_identity").content),
            req.get_file_value("platform").content,
            req.get_file_value("saved_account_identity").content,
            req.get_file_value("pass").content,
            req.get_file_value("created_at").content,
            req.get_file_value("updated_at").content,
            req.get_file_value("salt").content
        };
        std::string msg = postgreSQLCleint.insertAndReturnId("saved_accounts", keys, values, "saved_account_id");
        res.set_content(msg, "text/plain");
        });

    svr.Post("/updateAccount", [&](const auto& req, auto& res) {
        std::vector<std::string> keys{ "saved_account_id", "master_account_identity", "platform", "saved_account_identity", "pass", "created_at", "updated_at", "salt" };
        std::vector<std::string> values{
            req.get_file_value("saved_account_id").content,
            Crypto::Hash2_SHA256(req.get_file_value("master_account_identity").content, 1000, req.get_file_value("master_account_identity").content),
            req.get_file_value("platform").content,
            req.get_file_value("saved_account_identity").content,
            req.get_file_value("pass").content,
            req.get_file_value("created_at").content,
            req.get_file_value("updated_at").content,
            req.get_file_value("salt").content
        };
        std::string msg = postgreSQLCleint.updateById("saved_accounts", keys, values, 0);
        res.set_content(msg, "text/plain");
        });

    svr.Post("/deleteAccount", [&](const auto& req, auto& res) {
        std::vector<std::string> keys{ "saved_account_id", "master_account_identity" };
        std::vector<std::string> values{
            req.get_file_value("saved_account_id").content,
            Crypto::Hash2_SHA256(req.get_file_value("master_account_identity").content, 1000, req.get_file_value("master_account_identity").content),
        };
        std::string msg = postgreSQLCleint.deleteByValues("saved_accounts", keys, values);
        res.set_content(msg, "text/plain");
        });

    svr.listen("localhost", 1234);
}