/*
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

#include <borealis/core/storage_file.hpp>
#include <borealis/core/application.hpp>
#include <borealis/core/assets.hpp>

namespace brls
{

///////////////////
// StorageObject //
///////////////////

StorageObject::StorageObject(char* val, std::string name, std::string type)
{
    valueSet = new char[sizeof(val) + 1];
    this->setValue(val);
    this->setName(name);
    this->setType(type);
}

void StorageObject::setValue(char* newValue)
{
    valueSet = newValue;
}

char* StorageObject::value()
{
    return valueSet;
}

void StorageObject::setName(std::string val)
{
    nameInXML = val;
}

std::string StorageObject::name()
{
    return nameInXML;
}

void StorageObject::setType(std::string newType)
{
    typeOfValue = newType;
}

std::string StorageObject::type()
{
    return typeOfValue;
}

void StorageObject::setIsEmpty(bool newVal)
{
    isEmpty = newVal;
}

bool StorageObject::getIsEmpty()
{
    return isEmpty;
}

///////////////////////
// ListStorageObject //
///////////////////////

ListStorageObject::ListStorageObject(std::string name, char* value, std::string type)
{
    this->push_back(value);
    this->setName(name);
    this->setType(type);
}

void ListStorageObject::push_back(char* newVal)
{
    Value newPOD = Value {};
    newPOD.val   = newVal;
    values.push_back(newPOD);
}

void ListStorageObject::push_front(char* newVal)
{
    Value newPOD = Value {};
    newPOD.val   = newVal;
    values.insert(values.begin(), newPOD);
}

void ListStorageObject::insert(size_t index, char* newVal)
{
    Value newPOD = Value {};
    newPOD.val   = newVal;
    values.insert(values.begin() + index, newPOD);
}

char* ListStorageObject::get_front()
{
    auto valFromVec = values[0];
    return valFromVec.val;
}

char* ListStorageObject::get_back()
{
    auto valFromVec = values[values.size() - 1];
    return valFromVec.val;
}

char* ListStorageObject::get(size_t index)
{
    auto valFromVec = values[index];
    return valFromVec.val;
}

char* ListStorageObject::operator[](size_t index)
{
    return this->get(index);
}

bool ListStorageObject::operator==(const char* other)
{
    auto elem = this->get_back();
    return std::strcmp(elem, other) == 0;
}

bool ListStorageObject::operator!=(const char* other)
{
    auto elem = this->get_back();
    return std::strcmp(elem, other) != 0;
}

const size_t ListStorageObject::size()
{
    return values.size();
}

std::vector<ListStorageObject::Value>::iterator ListStorageObject::begin()
{
    return values.begin();
}

std::vector<ListStorageObject::Value>::iterator ListStorageObject::end()
{
    return values.end();
}

void ListStorageObject::setName(std::string val)
{
    nameInXML = val;
}

std::string ListStorageObject::name()
{
    return nameInXML;
}

void ListStorageObject::setType(std::string newType)
{
    typeOfValue = newType;
}

std::string ListStorageObject::type()
{
    return typeOfValue;
}

void ListStorageObject::setIsEmpty(bool newVal)
{
    isEmpty = newVal;
}

bool ListStorageObject::getIsEmpty()
{
    return isEmpty;
}

//////////////////////
// BasicStorageFile //
//////////////////////

bool BasicStorageFile::init(std::string filename, std::string appname)
{
    /*
    #ifndef __SWITCH__
        config_folder = std::filesystem::current_path().string() + "/config/" + appname + "/";
    #else
        config_folder = "/config/" + appname + "/";
    #endif
    */
    if (brls::Application::getPlatform()->getName() == "GLFW")
        config_folder = BRLS_ASSET("config/") + appname + "/";
    else
        config_folder = "/config" + appname + "/";


    if (!std::filesystem::exists(config_folder))
        std::filesystem::create_directories(config_folder);

    this->setConfigPath(filename);

    if (std::filesystem::exists(config_path))
    {
        Logger::debug("File already exists. Quitting out of init function...");
        return true;
    }

    std::fstream file;
    file.open(config_path, std::ios::out | std::ios::app);
    if (file.good())
    {
        file.close();
        Logger::debug("Successfully made file at {}!", config_path);
        return true;
    }
    else
    {
        Logger::error("Failed to init storage file {}.", filename);
        return false;
    }
}

bool BasicStorageFile::writeToFile(StorageObject& value, bool overwriteExisting)
{
    // Gets values from the StorageObject and configures the config_path variable
    const char* objectFromValue = value.value();
    std::string type            = value.type();
    std::string name            = value.name();

    if (!std::filesystem::exists(config_path)) {
        Logger::error("File not found. Make sure to call the init function before this.");
        return false;
    }

    // Defines an errorCode variable
    XMLError errorCode;

    // Defines a doc variable and attempts to load the file
    XMLDocument doc;
    errorCode = doc.LoadFile(config_path.c_str());

    XMLElement* root = doc.RootElement(); // We determine the root element (null or an actual element)

    if (!root || errorCode == XML_ERROR_EMPTY_DOCUMENT) // If root is null or LoadFile returned the XML_ERROR_EMPTY_DOCUMENT error (which means a new file)
    {
        // New root element
        XMLNode* pRoot = doc.NewElement("brls:StorageFile");
        doc.InsertFirstChild(pRoot);

        // New child element
        XMLElement* element = doc.NewElement("brls:Property");

        element->SetAttribute("name", name.c_str());
        element->SetAttribute("value", objectFromValue);
        element->SetAttribute("type", type.c_str());
        pRoot->InsertFirstChild(element);

        // We try to save the file
        errorCode = doc.SaveFile(config_path.c_str());

        if (errorCode == 0 || errorCode == XML_ERROR_EMPTY_DOCUMENT) // If it succedded
            return true;
        else
        { // Otherwise
            Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
            return false;
        }
    }
    else if (errorCode != XML_ERROR_EMPTY_DOCUMENT)
    { // Otherwise if there is a root element or LoadFile didn't return an error
        for (XMLElement* e = root->FirstChildElement(); e != NULL;
             e             = e->NextSiblingElement())
        {

            if (e == nullptr)
            {
                Logger::debug("nullptr???");
                break;
            }

            if (std::strcmp(e->Attribute("name"), name.c_str()) == 0 && overwriteExisting)
            {
                Logger::debug("Found another element with the same name as this new element. Overwrite Existing is enabled (by default).");
                root->DeleteChild(e);
                break;
            }
            else if (!overwriteExisting)
            {
                Logger::debug("Found another element with the same name as this new element. Overwrite Existing is disabled.");
                return false;
            }
        }

        // New child element
        XMLElement* element = doc.NewElement("brls:Property");

        element->SetAttribute("name", name.c_str());
        element->SetAttribute("value", objectFromValue);
        element->SetAttribute("type", type.c_str());
        root->InsertFirstChild(element);

        // We try to save the file
        errorCode = doc.SaveFile(config_path.c_str());

        if (errorCode == 0 || errorCode == XML_ERROR_EMPTY_DOCUMENT) // If it succedded
            return true;
        else
        { // Otherwise
            Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
            Logger::error("More details: {}", doc.ErrorStr());
            return false;
        }
    }
    else
    { // Else doc variable had other troubles loading the document
        Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
        Logger::error("More details: {}", doc.ErrorStr());
        return false;
    }
}

bool BasicStorageFile::writeListToFile(ListStorageObject& obj, bool overwriteExisting)
{
    std::string name = obj.name();
    std::string type = obj.type();

    if (!std::filesystem::exists(config_path))
        Logger::error("File not found. Make sure to call the init function before this.");

    // Defines an errorCode variable
    XMLError errorCode;

    // Defines a doc variable and attempts to load the file
    XMLDocument doc;
    errorCode = doc.LoadFile(config_path.c_str());

    XMLElement* root = doc.RootElement(); // We determine the root element (null or an actual element)

    if (!root || errorCode == XML_ERROR_EMPTY_DOCUMENT) // If root is null or LoadFile returned the XML_ERROR_EMPTY_DOCUMENT error (which means a new file)
    {

        // New root element
        XMLNode* pRoot = doc.NewElement("brls:StorageFile");
        doc.InsertFirstChild(pRoot);

        // New child element
        XMLElement* element = doc.NewElement("brls:ListProperty");

        element->SetAttribute("name", name.c_str());
        element->SetAttribute("type", type.c_str());
        for (size_t i { 0 }; i < obj.size(); i++)
        {
            XMLElement* listElement = doc.NewElement("brls:Value");
            listElement->SetAttribute("value", obj[i]);
            element->InsertEndChild(listElement);
        }
        pRoot->InsertFirstChild(element);

        // We try to save the file
        errorCode = doc.SaveFile(config_path.c_str());

        if (errorCode == 0 || errorCode == XML_ERROR_EMPTY_DOCUMENT) // If it succedded
            return true;
        else
        { // Otherwise
            Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
            Logger::error("More details: {}", doc.ErrorStr());
            return false;
        }
    }
    else if (errorCode != XML_ERROR_EMPTY_DOCUMENT)
    { // Otherwise if there is a root element or LoadFile didn't return an error

        for (XMLElement* e = root->FirstChildElement(); e != NULL;
             e             = e->NextSiblingElement())
        {

            if (e == nullptr)
            {
                Logger::debug("nullptr???");
                break;
            }

            if (std::strcmp(e->Attribute("name"), name.c_str()) == 0 && overwriteExisting)
            {
                Logger::debug("Found another element with the same name as this new element. Overwrite Existing is enabled (by default).");
                root->DeleteChild(e);
                break;
            }
            else if (!overwriteExisting)
            {
                Logger::debug("Found another element with the same name as this new element. Overwrite Existing is disabled.");
                return false;
            }
        }

        // New child element
        XMLElement* element = doc.NewElement("brls:ListProperty");

        element->SetAttribute("name", name.c_str());
        element->SetAttribute("type", type.c_str());
        for (size_t i { 0 }; i < obj.size(); i++)
        {
            XMLElement* listElement = doc.NewElement("brls:Value");
            listElement->SetAttribute("value", obj[i]);
            element->InsertEndChild(listElement);
        }
        root->InsertFirstChild(element);

        // We try to save the file
        errorCode = doc.SaveFile(config_path.c_str());

        if (errorCode == 0 || errorCode == XML_ERROR_EMPTY_DOCUMENT) // If it succedded
            return true;
        else
        { // Otherwise
            Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
            Logger::error("More details: {}", doc.ErrorStr());
            return false;
        }
    }
    else
    { // Else doc variable had other troubles loading the document
        Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
        Logger::error("More details: {}", doc.ErrorStr());
        return false;
    }
}

bool BasicStorageFile::parseXMLToObject(std::string name)
{
    /*
    if (!allStorageObjectsMap.empty())
    {
        Logger::debug("allStorageObjectsMap is already used, cleaning up...");
        allStorageObjectsMap.clear();
    }
    */
    if (currentStorageObject)
        delete currentStorageObject;
    
    XMLError errorCode;
    XMLDocument doc;
    errorCode = doc.LoadFile(config_path.c_str());

    if (errorCode == 0)
    {
        XMLNode* root = doc.RootElement();

        if (root == nullptr)
        {
            Logger::error("NULL root element. This means the XML file is blank (hasn't been written to yet), or the XML file is broken.");
            return false;
        }

        XMLElement* element = nullptr;

        for (XMLElement* e = root->FirstChildElement("brls:Property"); e != NULL;
             e             = e->NextSiblingElement("brls:Property"))
        {

            if (!e)
                break;
            
            if (std::strcmp(e->Attribute("name"), name.c_str()) == 0)
            {
                element = e;
                break;
            }
        }

        if (element)
        {
            const char* value;
            const char* type;

            element->QueryStringAttribute("value", &value);
            element->QueryStringAttribute("type", &type);

            currentStorageObject = new StorageObject();

            currentStorageObject->setName(name);
            currentStorageObject->setType(std::string(type));
            currentStorageObject->setValue(const_cast<char*>(value));

            return true;
        }
        else
        {
            Logger::warning("NULL element variable. This means there is a root element, but no child elements.");
            return false;
        }
    }
    else
    {
        Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
        Logger::error("More details: {}", doc.ErrorStr());
        return false;
    }
}

bool BasicStorageFile::parseXMLToListObject(std::string name)
{
    /*
    if (!allListStorageObjectsMap.empty())
    {
        Logger::debug("allListStorageObjectsMap is already used, cleaning up...");
        allListStorageObjectsMap.clear();
    }
    */
    if (currentListStorageObject)
        delete currentListStorageObject;

    XMLError errorCode;
    XMLDocument doc;
    errorCode = doc.LoadFile(config_path.c_str());

    if (errorCode == 0)
    {
        XMLNode* root = doc.RootElement();

        if (root == nullptr)
        {
            Logger::error("NULL root element. This means the XML file is blank (hasn't been written to yet), or the XML file is broken.");
            return false;
        }

        XMLElement* element = nullptr;

        for (XMLElement* e = root->FirstChildElement("brls:ListProperty"); e != NULL;
             e             = e->NextSiblingElement("brls:ListProperty"))
        {

            if (!e)
                break;
            
            if (std::strcmp(e->Attribute("name"), name.c_str()) == 0)
            {
                element = e;
                break;
            }
        }

        if (element)
        {
            const char* type;
            element->QueryStringAttribute("type", &type);

            currentListStorageObject = new ListStorageObject();
            currentListStorageObject->setName(name);
            currentListStorageObject->setType(std::string(type));

            for (XMLElement* e = element->FirstChildElement("brls:Value"); e != NULL;
                 e             = e->NextSiblingElement("brls:Value"))
            {
                const char* currentAttributeElem;
                e->QueryStringAttribute("value", &currentAttributeElem);
                currentListStorageObject->push_back(const_cast<char*>(currentAttributeElem));
            }

            return true;
        }
        else
        {
            Logger::warning("NULL element variable. This means there is a root element, but no child elements.");
            return false;
        }
    }
    else
    {
        Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
        Logger::error("More details: {}", doc.ErrorStr());
        return false;
    }
}

StorageObject BasicStorageFile::readFromFile(std::string name)
{
    if (!std::filesystem::exists(config_path)) {
        Logger::error("File not found. Make sure to call the init function before this.");
        StorageObject nullStorageObject;
        nullStorageObject.setIsEmpty(true);
        return nullStorageObject;
    }

    bool result = this->parseXMLToObject(name);
    if (result)
        return *currentStorageObject;
    
    StorageObject nullStorageObject;
    nullStorageObject.setIsEmpty(true);
    return nullStorageObject;
}

ListStorageObject BasicStorageFile::readListFromFile(std::string name)
{
    if (!std::filesystem::exists(config_path)) {
        Logger::error("File not found. Make sure to call the init function before this.");
        ListStorageObject nullStorageObject;
        nullStorageObject.setIsEmpty(true);
        return nullStorageObject;
    }

    bool result = this->parseXMLToListObject(name);
    if (result)
        return *currentListStorageObject;
    
    ListStorageObject nullStorageObject;
    nullStorageObject.setIsEmpty(true);
    return nullStorageObject;
}

bool BasicStorageFile::isFileEmpty()
{
    XMLDocument doc;
    doc.LoadFile(config_path.c_str());
    XMLElement* root = doc.RootElement();
    if (!root || doc.ErrorID() == XML_ERROR_EMPTY_DOCUMENT)
        return true;
    else
        return false;
}

/////////////////
// StorageFile //
/////////////////

bool StorageFile::save(StorageObject& obj)
{
    return this->writeToFile(obj);
}

bool StorageFile::save(ListStorageObject& obj)
{
    return this->writeListToFile(obj);
}

bool StorageFile::grab(StorageObject& obj, std::string name)
{
    obj = this->readFromFile(name);
    return !obj.getIsEmpty();
}

bool StorageFile::grab(ListStorageObject& obj, std::string name)
{
    obj = this->readListFromFile(name);
    return !obj.getIsEmpty();
}

} // namespace brls