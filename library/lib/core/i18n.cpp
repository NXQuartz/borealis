/*
    Copyright 2020-2021 natinusala
    Copyright 2021 EmreTech

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <tinyxml2/tinyxml2.h>

#include <borealis/core/application.hpp>
#include <borealis/core/assets.hpp>
#include <borealis/core/i18n.hpp>
#include <filesystem>
#include <unordered_map>

namespace brls
{

typedef std::unordered_map<std::string, std::string> locales;

// For Internal text
const std::string internalXML = R"xml(
    <brls:i18nDoc>
       <brls:List name="hints">
           <brls:String name="ok">OK</brls:String>
           <brls:String name="back">Back</brls:String>
           <brls:String name="exit">Exit</brls:String>
       </brls:List>
       <brls:List name="crash_frame">
           <brls:String name="button">OK</brls:String>
       </brls:List>
       <brls:List name="thumbnail_sidebar">
           <brls:String name="save">Save</brls:String>
       </brls:List>
    </brls:i18nDoc>)xml";

static locales xmlDefaultLocale; // For default locale (en-US)
static locales xmlCurrentLocale; // For current locale found by platform
static locales xmlInternalText; // For internal text

void getTextFromElements(tinyxml2::XMLElement* root, std::string existing_path, locales& target)
{
    if (!root)
    { // Checks if root is nullptr
        Logger::error("Root provided is nullptr."); // If it is, return an error message and return
        return;
    }

    for (tinyxml2::XMLElement* e2 = root->FirstChildElement(); e2 != NULL; e2 = e2->NextSiblingElement()) // Loops through all XML Elements until it equals null
    {
        std::string path = existing_path + std::string("/") + e2->Attribute("name");
        if (std::strcmp(e2->Name(), "brls:List") == 0) // If the XML Element is a brls:List,
            getTextFromElements(e2, path, target); // we use a recursive strategy to grab all elements.
        else if (std::strcmp(e2->Name(), "brls:String") == 0) // Otherwise, if the XML Element is a brls:String,
        {
            tinyxml2::XMLText* textFromElem = e2->FirstChild()->ToText(); // we query the text,
            target[path]                    = textFromElem->Value(); // and add a map element to the provided unordered map.
        }
        else // Otherwise, if the element is unknown,
        {
            Logger::error("Found unknown element in i18n file. Element name is {}. Continuing...", e2->Name()); // we give an error message and continue looping
            continue;
        }
    }
}

static void loadLocale(std::string locale, locales& target)
{
    std::string localePath = BRLS_ASSET("i18n/" + locale);

    if (!std::filesystem::exists(localePath)) // If the localePath above doesn't exist,
    {
        Logger::error("Cannot load locale {}: directory {} doesn't exist", locale, localePath); // we give an error message and return.
        return;
    }
    else if (!std::filesystem::is_directory(localePath)) // Otherwise, if the localePath points to a non-directory,
    {
        Logger::error("Cannot load locale {}: {} isn't a directory", locale, localePath); // we give an error message and return.
        return;
    }

    // Iterate over all XML files in the directory
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(localePath))
    {
        if (entry.is_directory()) // If the entry is a directory, we continue.
            continue;

        std::string name      = entry.path().filename().string();
        std::string extension = entry.path().filename().extension().string();

        if (!(extension == ".xml")) // If the entry's extension does not equal .xml, we continue.
            continue;

        std::string path = entry.path().string();

        // If the above checks pass, we define a XMLDocument and attempt to load the file
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError error = doc.LoadFile(path.c_str());

        if (error != tinyxml2::XML_SUCCESS) // If the error variable does not equal XML_SUCCESS (0),
        {
            Logger::error("Cannot load file {} found in locale {}: error code {}", entry.path().filename().string(), locale, std::to_string(error)); // we give an error message and return.
            Logger::error("More details: {}", doc.ErrorStr());
            return;
        }

        tinyxml2::XMLElement* root = doc.RootElement(); // We grab the root element
        if (std::strcmp(root->Name(), "brls:i18nDoc") != 0) // and check if the root element name equals brls:i18nDoc.
        {
            Logger::error("Could not find root element with name \"brls:i18nDoc\"."); // If not, we give an error message and continue.
            continue;
        }

        // Iterate over all XML elements in the file
        std::string path_2 = name.substr(0, name.find("."));
        getTextFromElements(root, path_2, target);
    }
}

void loadInternal()
{
    // We define a XMLDocument and attempt to load the file
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.Parse(internalXML.c_str());

    if (error != tinyxml2::XML_SUCCESS) // If the error variable does not equal XML_SUCCESS (0),
    {
        Logger::error("Cannot load internal XML: error code {}", std::to_string(error)); // we give an error message and return.
        Logger::error("More details: {}", doc.ErrorStr());
        return;
    }

    tinyxml2::XMLElement* root = doc.RootElement(); // We grab the root element (no need for the brls:i18nDoc check)

    // Iterate over all XML elements in the file
    getTextFromElements(root, "brls", xmlInternalText);
}

void loadTranslations()
{
    // Load internal text first
    loadInternal();

    // Then load text for default locale (en-US)
    loadLocale(LOCALE_DEFAULT, xmlDefaultLocale);

    std::string currentLocaleName = Application::getLocale();
    // If current locale doesn't equal default locale (en-US), try loading current locale.
    // If the locale doesn't exist, it falls back to the default one.
    if (currentLocaleName != LOCALE_DEFAULT)
        loadLocale(currentLocaleName, xmlCurrentLocale);
}

namespace internal
{
    std::string getRawStr(std::string stringName)
    {
        std::string currentLocaleName = Application::getLocale();

        // First look for translated string in current locale
        if (currentLocaleName != LOCALE_DEFAULT)
        {
            for (auto& [path, value] : xmlCurrentLocale)
            {
                if (stringName == path)
                    return value;
            }
        }

        // Then look for default locale
        else if (currentLocaleName == LOCALE_DEFAULT)
        {
            for (auto& [path, value] : xmlDefaultLocale)
            {
                if (stringName == path)
                    return value;
            }
        }

        // Fallback to returning the string name
        return stringName;
    }

    std::string getInternalRawStr(std::string stringName)
    {
        // Looks through Internal text (its locale is en-US)
        for (auto& [path, value] : xmlInternalText)
        {
            if (stringName == path)
                return value;
        }

        // Fallback to returning the string name
        return stringName;
    }
} // namespace internal

inline namespace literals
{
    std::string operator"" _i18n(const char* str, size_t len)
    {
        return internal::getRawStr(std::string(str, len));
    }

    std::string operator"" _internal(const char* str, size_t len)
    {
        return internal::getInternalRawStr(std::string(str, len));
    }
} // namespace literals

} // namespace brls
