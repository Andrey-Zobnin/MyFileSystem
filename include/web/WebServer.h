#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <string>
#include <iostream>
#include <memory>
#include "crow_all.h"
#include "../FileSystem.h"

class WebServer {
public:
    WebServer(std::shared_ptr<FileSystem> fs, int port = 5000) 
        : fileSystem(fs), port(port) {}

    void start() {
        crow::SimpleApp app;

        // Static files
        CROW_ROUTE(app, "/")([]() {
            crow::response res;
            res.set_static_file_info("web/static/index.html");
            return res;
        });

        CROW_ROUTE(app, "/styles.css")([]() {
            crow::response res;
            res.set_static_file_info("web/static/styles.css");
            return res;
        });

        CROW_ROUTE(app, "/app.js")([]() {
            crow::response res;
            res.set_static_file_info("web/static/app.js");
            return res;
        });

        // API routes
        CROW_ROUTE(app, "/api/ls")
        ([this](const crow::request& req) {
            auto path = req.url_params.get("path");
            if (!path) path = const_cast<char*>("/");

            try {
                auto items = fileSystem->listDirectory(path);
                crow::json::wvalue response;
                response["success"] = true;
                
                crow::json::wvalue itemsArray = crow::json::wvalue::list();
                for (size_t i = 0; i < items.size(); ++i) {
                    crow::json::wvalue item;
                    item["name"] = items[i].first;
                    item["type"] = items[i].second ? "folder" : "file";
                    itemsArray[i] = std::move(item);
                }
                
                response["items"] = std::move(itemsArray);
                return crow::response(response);
            } catch (const std::exception& e) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = e.what();
                return crow::response(response);
            }
        });

        CROW_ROUTE(app, "/api/mkdir")
        ([this](const crow::request& req) {
            auto path = req.url_params.get("path");
            if (!path) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = "Path parameter is required";
                return crow::response(response);
            }

            try {
                fileSystem->createDirectory(path);
                crow::json::wvalue response;
                response["success"] = true;
                return crow::response(response);
            } catch (const std::exception& e) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = e.what();
                return crow::response(response);
            }
        });

        CROW_ROUTE(app, "/api/rmdir")
        ([this](const crow::request& req) {
            auto path = req.url_params.get("path");
            if (!path) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = "Path parameter is required";
                return crow::response(response);
            }

            try {
                fileSystem->removeDirectory(path);
                crow::json::wvalue response;
                response["success"] = true;
                return crow::response(response);
            } catch (const std::exception& e) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = e.what();
                return crow::response(response);
            }
        });

        CROW_ROUTE(app, "/api/write").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = "Invalid JSON body";
                return crow::response(response);
            }

            try {
                std::string path = body["path"].s();
                std::string content = body["content"].s();
                
                // Write file to real filesystem temporarily
                std::string tempFilePath = "temp_write_file";
                std::ofstream tempFile(tempFilePath);
                tempFile << content;
                tempFile.close();
                
                // Write from temp file to virtual filesystem
                fileSystem->writeFile(path, tempFilePath);
                
                // Remove temporary file
                std::remove(tempFilePath.c_str());
                
                crow::json::wvalue response;
                response["success"] = true;
                return crow::response(response);
            } catch (const std::exception& e) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = e.what();
                return crow::response(response);
            }
        });

        CROW_ROUTE(app, "/api/read")
        ([this](const crow::request& req) {
            auto path = req.url_params.get("path");
            if (!path) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = "Path parameter is required";
                return crow::response(response);
            }

            try {
                // Create a temporary file to read content into
                std::string tempFilePath = "temp_read_file";
                
                // Read from virtual filesystem to temp file
                fileSystem->readFile(path, tempFilePath);
                
                // Read content from temp file
                std::ifstream tempFile(tempFilePath);
                std::string content((std::istreambuf_iterator<char>(tempFile)),
                                     std::istreambuf_iterator<char>());
                tempFile.close();
                
                // Remove temporary file
                std::remove(tempFilePath.c_str());
                
                crow::json::wvalue response;
                response["success"] = true;
                response["content"] = content;
                return crow::response(response);
            } catch (const std::exception& e) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = e.what();
                return crow::response(response);
            }
        });

        CROW_ROUTE(app, "/api/delete")
        ([this](const crow::request& req) {
            auto path = req.url_params.get("path");
            if (!path) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = "Path parameter is required";
                return crow::response(response);
            }

            try {
                fileSystem->deleteFile(path);
                crow::json::wvalue response;
                response["success"] = true;
                return crow::response(response);
            } catch (const std::exception& e) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = e.what();
                return crow::response(response);
            }
        });

        // Start the server
        std::cout << "Web server starting on port " << port << std::endl;
        app.bindaddr("0.0.0.0").port(port).multithreaded().run();
    }

private:
    std::shared_ptr<FileSystem> fileSystem;
    int port;
};

#endif // WEB_SERVER_H