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

#pragma once

#include <borealis/views/applet_frame.hpp>
#include <vector>

namespace brls
{

struct StagedAppletFrame : public AppletFrame
{
    StagedAppletFrame();
    ~StagedAppletFrame();

    static View* create();

    void addStage(View *view);
    void nextStage();
    void previousStage();

    size_t getCurrentStage();
    size_t getAmountOfStages();
    bool isLastStage();

    private:
    std::vector<View*> allViews;
    size_t currentStage = 0;

    void enterStage(int index, bool requestFocus);
};

} // namespace brls
