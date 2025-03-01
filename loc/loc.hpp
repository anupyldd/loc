#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <exception>
#include <format>

namespace loc
{
    // callback that is called whenever an error occurs
    // you should redefine it with your custom error handler
    // does nothing by default
    std::function<void(const std::string&)> error_callback = [](const std::string& msg) {};

    // class that keeps language-translation mapping
    // template arguments:
    //  - lang_t:   language type, e.g. std::string (default) or enum
    //  - str_t:    string type, e.g. std::string (default) or custom string class
    //  - map_t:    map type for storing lang-str pairs, e.g std::unordered_map (default)
    template
        <
        class lang_t = std::string, 
        class str_t = std::string,
        template<class, class> class map_t = std::unordered_map
        >
    class multi_str
    {
        using map_type = map_t<lang_t, str_t>;

    public:
        // add translation to language
        multi_str& add(const lang_t& lang, const str_t& str)
        {
            translations.try_emplace(lang, str);
            return *this;
        }
        // remove translation to language
        multi_str& remove(const lang_t& lang)
        {
            translations.erase(lang);
            return *this;
        }
        
        // get translation to language
        const str_t& get(const lang_t& lang) const
        {
            try
            {
                return translations.at(lang);
            }
            catch (const std::exception& e)
            {
                error_callback(std::format("Failed to get translation: {}", e.what()));
            }
            static const str_t empty_str{};
            return empty_str;
        }
        // get translation to language
        const str_t& operator () (const lang_t& lang) const
        {
            return get(lang);
        }
        
        // check if map contains translation to specified language
        bool has(const lang_t& lang) const
        {
            return translations.contains(lang);
        }

        // get language-translation map
        const map_type& get_map() const
        {
            return translations;
        }

    private:
        map_type translations;
    };

    // -----------------------------------------------------
    
    // class that holds id-multistr pairs
    // template arguments:
    //  - id_t:     string identifier type, e.g. std::string (default), UID, etc.
    // (next three are used to specify template of multistr in the dictionary)
    //  - lang_t:   language type, e.g. std::string (default) or enum
    //  - str_t:    string type, e.g. std::string (default) or custom string class
    //  - map_t:    map type for storing lang-str pairs, e.g std::unordered_map (default)
    template
        <
        class id_t = std::string,
        class lang_t = std::string,
        class str_t = std::string,
        template<class, class> class map_t = std::unordered_map
        >
    class localization
    {
        using map_type = map_t<id_t, multi_str<lang_t, str_t>>;
        using loader_func = std::function<bool(const std::filesystem::path&)>;
    
    public:
        // set function that will be used to load data
        // this is required before loading, since default loader does nothing
        void set_loader(loader_func func) 
        { 
            loader = std::move(func); 
        }

        // load data using specified loader function
        void load(const std::filesystem::path& path)
        {
            if (!loader(path))
            {
                error_callback("Failed to load localization data");
            }
        }

        // clear localization map
        void clear()
        {
            loc_map.clear();
        }

        // get localization map
        const map_type& get_map() const
        {
            return loc_map;
        }

        // get localization map
        map_type& get_map()
        {
            return loc_map;
        }

        // get translation of string id to language lang
        const str_t& get(const id_t& id, const lang_t& lang) const
        {
            try
            {
                return loc_map.at(id).get(lang);
            }
            catch (const std::exception& e)
            {
                error_callback(std::format("Failed to get localization: {}", e.what()));
            }
            static const str_t empty_str{};
            return empty_str;
        }

        void add(const id_t& id, const lang_t& lang, const str_t& str)
        {
            loc_map[id].add(lang, str);
        }

    private:
        map_type loc_map;
        loader_func loader = [](const std::filesystem::path& path) { return true; };
    };
}