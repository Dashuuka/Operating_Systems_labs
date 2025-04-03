#include <iostream>
#include <vector>
#include <stdexcept>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "factorial.h"
#include "reverse_list.h"

// HTTP Listener для REST API
void handle_request(web::http::http_request request) {
    auto path = request.relative_uri().path();
    if (path == "/factorial") {
        try {
            auto query = request.query_parameters();
            if (query.find("n") != query.end()) {
                int n = std::stoi(query["n"]);
                web::json::value response = get_factorials(n);
                request.reply(web::http::status_codes::OK, response);
            }
            else {
                request.reply(web::http::status_codes::BadRequest, "Missing parameter n");
            }
        }
        catch (const std::exception& e) {
            request.reply(web::http::status_codes::InternalError, "Error: " + std::string(e.what()));
        }
    }
    else {
        request.reply(web::http::status_codes::NotFound, "Not Found");
    }
}

int main() {
    try {
        int n;
        std::cout << "Enter a natural number n: ";
        std::cin >> n;
        std::vector<long long> factorials = get_factorials(n);
        for (const auto& fact : factorials) {
            std::cout << fact << " ";
        }

        // REST API слушатель
        web::http::experimental::listener::http_listener listener(U("http://localhost:8080"));
        listener.support(web::http::methods::GET, handle_request);
        listener
            .open()
            .then([]() { std::wcout << L"Server is running at http://localhost:8080\n"; })
            .wait();

        std::string line;
        std::getline(std::cin, line); 
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
