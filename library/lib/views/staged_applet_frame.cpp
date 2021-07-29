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

#include <borealis/views/staged_applet_frame.hpp>
#include <borealis/core/application.hpp>

namespace brls
{

StagedAppletFrame::StagedAppletFrame()
    : AppletFrame()
{}

StagedAppletFrame::~StagedAppletFrame()
{
    for (View *view : this->allViews)
        delete view;

    this->allViews.clear();
    this->setContentView(nullptr);
}

void StagedAppletFrame::addStage(View *view)
{
    allViews.push_back(view);

    if (!currentStage)
        enterStage(0, false);
}

void StagedAppletFrame::nextStage()
{
    enterStage((int) currentStage+1, true);
}

void StagedAppletFrame::previousStage()
{
    enterStage((int) currentStage-1, true);
}

void StagedAppletFrame::enterStage(int index, bool requestFocus)
{
    if ((size_t) index >= allViews.size())
        return;

    if (!this->contentView)
        allViews.at(currentStage)->willDisappear(true);

    currentStage = (size_t)index;
    this->setContentView(allViews.at(currentStage));

    if (requestFocus)
        Application::giveFocus(this->getDefaultFocus());
}

size_t StagedAppletFrame::getCurrentStage()
{
    return this->currentStage;
}

size_t StagedAppletFrame::getAmountOfStages()
{
    return this->allViews.size();
}

bool StagedAppletFrame::isLastStage()
{
    return this->currentStage == this->allViews.size() - 1;
}

} // namespace brls
