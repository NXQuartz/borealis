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

// Note: This file, it's source counterpart, and any other files relating to the Storage File Demo can be taken down at demand.
// They are only here for testing purposes.

#pragma once

#include <borealis.hpp>
#include <iostream>
#include <string>

#include "settings_file.hpp"

struct StorageFileDemo : public brls::Box
{
    StorageFileDemo();

    static brls::View* create();

  private:
    bool onWriteDataButtonPressed(brls::View* view);
    bool onReadDataButtonPressed(brls::View* view);

    BRLS_BIND(brls::Label, verboseText, "verboseText");

    SettingsFile *settings = new SettingsFile();
};