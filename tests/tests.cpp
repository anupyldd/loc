#include "../loc/loc.hpp"

#include "picojson.h"

#include <cassert>
#include <iostream>
#include <map>
#include <fstream>
#include <print>

void test(const std::string& name)
{
    std::cout << std::format("\n=== test '{}' ===\n", name);
}

void test_complete()
{
    std::cout << "test complete\n";
}

void cl_log(const std::string& msg)
{
    std::cout << "[CUSTOM LOADER] " << msg << '\n';
}

enum class lang_enum { en, ru, jp };

// -------------------------------------

int main()
{
    using namespace loc;

    error_callback = [](const std::string& msg) { std::cout << "[ERROR] " << msg << '\n'; };

    test("default multi_str");
    multi_str s1;
    s1.add("en", "hello");
    s1.add("ru", "привет");
    s1.add("jp", "こんにちわ");
    assert(s1.get_map().size() == 3);
    assert(s1.get("en") == "hello");
    assert(s1.get("ru") == "привет");
    assert(s1.get("jp") == "こんにちわ");
    test_complete();

    test("customized multi_str");
    multi_str<lang_enum> s2;
    s2.add(lang_enum::en, "hello");
    s2.add(lang_enum::ru, "привет");
    s2.add(lang_enum::jp, "こんにちわ");
    assert(s2.get_map().size() == 3);
    assert(s2.get(lang_enum::en) == "hello");
    assert(s2.get(lang_enum::ru) == "привет");
    assert(s2.get(lang_enum::jp) == "こんにちわ");
    test_complete();

    test("localization loading");
    localization l;
    l.set_loader(
        [&l](const std::filesystem::path& path) {
            try
            {
                cl_log("Started loading localization");

                auto cwd = std::filesystem::current_path();
                std::ifstream file(cwd / path);
                if (!file)
                {
                    cl_log("Failed to open data file");
                    return false;
                }

                std::stringstream contents;
                contents << file.rdbuf();

                std::string str_cont = contents.str();
                picojson::value val;

                std::string err = picojson::parse(val, str_cont);
                if (!err.empty())
                {
                    cl_log("Failed to parse localization file: " + err);
                    return false;
                }
                if (!val.is<picojson::object>())
                {
                    cl_log("Localization json is not an object");
                    return false;
                }

                const picojson::value::object& obj = val.get<picojson::object>();

                // load loc info
                const auto& langs = { "en","ru","jp" };
                for (const auto& lang : langs)
                {
                    for (const auto& entry : obj)
                    {
                        // load entry
                        const auto& e= obj.at(entry.first).get<picojson::object>();
                        std::string str = e.at(lang).get<std::string>();
                        l.add(entry.first, lang, str);
                    }
                }

                return true;
            }
            catch (const std::exception& e)
            {
                cl_log(std::format("Exception occured while loading data: {}", e.what()));
                return false;
            }
        }
    );

    l.load("data.json");

    assert(l.get_map().size() == 3);

    std::cout << "hello:\n";
    std::cout << l.get("hello", "en") << '\n';
    std::cout << l.get("hello", "ru") << '\n';
    std::cout << l.get("hello", "jp") << '\n';

    std::cout << "bye:\n";
    std::cout << l.get("bye", "en") << '\n';
    std::cout << l.get("bye", "ru") << '\n';
    std::cout << l.get("bye", "jp") << '\n';

    std::cout << "what:\n";
    std::cout << l.get("what", "en") << '\n';
    std::cout << l.get("what", "ru") << '\n';
    std::cout << l.get("what", "jp") << '\n';

    //assert(l.get("hello", "ru") == "привет");
    //assert(l.get("hello", "jp") == "こんにちわ");
    //
    //assert(l.get("bye", "en") == "goodbye");
    //assert(l.get("bye", "ru") == "до свидания");
    //assert(l.get("bye", "jp") == "さよなら");
    //
    //assert(l.get("what", "en") == "what");
    //assert(l.get("what", "ru") == "что");
    //assert(l.get("what", "jp") == "何");

    test_complete();
}