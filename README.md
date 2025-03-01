## loc
This is a tiny header-only utility for handling localization

### Structure

**multi_str**: used to store multiple translations of one string.

**localization**: stores id-multi_str mapping for retrieving translations by id and language.

Types for storing strings, languages and mappings can be customized with templates.

### Example

```cpp
#include "loc/loc.hpp"
#include <iostream>

int main()
{
    using namespace loc;
    
    // set custom error callback
    error_callback = [](const std::string& msg) 
        { std::cout << "[ERROR] " << msg << '\n'; };
     
    // create localization object
    localization l;
    
    // set custom data loader
    l.set_loader([&l](const std::filesystem::path& path) {
        // some data loading logic
        l.add("hi", "en", "hello");
        l.add("hi", "ru", "привет");
        
        l.add("bye", "en", "goodbye");
        l.add("bye", "ru", "до свидания");
        
        return true;
    });
    
    // load localization data
    l.load("path/to/data");
    
    // get loaded data
    std::cout << l.get("hi", "en") << '\n';
    std::cout << l.get("bye", "ru") << '\n';
}
```

### License

**loc** is released under CC0 / Public domain.







