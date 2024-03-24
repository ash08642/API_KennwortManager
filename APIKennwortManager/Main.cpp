#include <httplib.h>
#include <iostream>

int main(void)
{
    using namespace httplib;

    Server svr;

    svr.Get("/hi", [](const Request& req, Response& res) {
        res.set_content("Hello World!", "text/plain");
        std::cout << "get req" << std::endl;
        });

    // Match the request path against a regular expression
    // and extract its captures
    svr.Get(R"(/numbers/(\d+))", [&](const Request& req, Response& res) {
        auto numbers = req.matches[1];
        res.set_content(numbers, "text/plain");
        });

    // Capture the second segment of the request path as "id" path param
    svr.Get("/users/:id", [&](const Request& req, Response& res) {
        auto user_id = req.path_params.at("id");
        res.set_content(user_id, "text/plain");
        });

    // Extract values from HTTP headers and URL query params
    svr.Get("/body-header-param", [](const Request& req, Response& res) {
        if (req.has_header("Content-Length")) {
            auto val = req.get_header_value("Content-Length");
        }
        if (req.has_param("key")) {
            auto val = req.get_param_value("key");
        }
        res.set_content(req.body, "text/plain");
        });

    svr.Get("/stop", [&](const Request& req, Response& res) {
        svr.stop();
        });

    svr.Post("/post", [](const Request& req, Response& res) {
        res.set_content(req.body, "text/plain");
        std::cout << "post req" << std::endl;
        });

    svr.Post("/multipart", [&](const auto& req, auto& res) {
        auto size = req.files.size();
        auto ret = req.has_file("name");
        const auto& file = req.get_file_value("name");
        std::cout << file.filename << std::endl;
        std::cout << file.content_type << std::endl;
        std::cout << file.content << std::endl;
        auto ret2 = req.has_file("age");
        const auto& file2 = req.get_file_value("age");
        std::cout << file2.filename << std::endl;
        std::cout << file2.content_type << std::endl;
        std::cout << file2.content << std::endl;
        res.set_content("Post Succesful", "text/plain");
        // file.filename;
        // file.content_type;
        // file.content;
        });

    svr.listen("localhost", 1234);
}