/*
    Copyright 2020 natinusala
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

#include <borealis/views/hint.hpp>
#include <borealis/core/application.hpp>
#include <borealis/core/input.hpp>
#include <string>
#include <set>

namespace brls
{

std::string getKeyIcon(ControllerButton key)
{
    switch (key)
    {
        case ControllerButton::BUTTON_A:
            return "\uE0E0";
        case ControllerButton::BUTTON_B:
            return "\uE0E1";
        case ControllerButton::BUTTON_X:
            return "\uE0E2";
        case ControllerButton::BUTTON_Y:
            return "\uE0E3";
        case ControllerButton::BUTTON_LSB:
            return "\uE104";
        case ControllerButton::BUTTON_RSB:
            return "\uE105";
        case ControllerButton::BUTTON_LB:
            return "\uE0E4";
        case ControllerButton::BUTTON_RB:
            return "\uE0E5";
        case ControllerButton::BUTTON_START:
            return "\uE0EF";
        case ControllerButton::BUTTON_BACK:
            return "\uE0F0";
        case ControllerButton::BUTTON_LEFT:
            return "\uE0ED";
        case ControllerButton::BUTTON_UP:
            return "\uE0EB";
        case ControllerButton::BUTTON_RIGHT:
            return "\uE0EF";
        case ControllerButton::BUTTON_DOWN:
            return "\uE0EC";
        default:
            break;
    }

    return "\uE152";
}

bool actionsSortFunc(Action a, Action b)
{
    // From left to right:
    //  - first +
    //  - then all hints that are not B and A
    //  - finally B and A

    // + is before all others
    if (a.button == ControllerButton::BUTTON_START)
        return true;

    // A is after all others
    if (b.button == ControllerButton::BUTTON_A)
        return true;

    // B is after all others but A
    if (b.button == ControllerButton::BUTTON_B && a.button != ControllerButton::BUTTON_A)
        return true;

    // Keep original order for the rest
    return false;
}

const std::string hintXML = R"xml(
    <brls:Box
        width="auto"
        height="auto"
        axis="row"
        direction="leftToRight">

        <!--Labels will be inserted here-->

    </brls:Box>
)xml";

Hint::Hint()
{
    this->inflateFromXMLString(hintXML);

    this->globalFocusEventSubscriptor = Application::getGlobalFocusChangeEvent()->subscribe([this](View* newFocus) {
        this->rebuildHints();
    });

    this->globalHintsUpdateEventSubscriptor = Application::getGlobalHintsUpdateEvent()->subscribe([this]() {
        this->rebuildHints();
    });
}

Hint::~Hint()
{
    Application::getGlobalFocusChangeEvent()->unsubscribe(this->globalFocusEventSubscriptor);
    Application::getGlobalHintsUpdateEvent()->unsubscribe(this->globalHintsUpdateEventSubscriptor);
}

View *Hint::create()
{
    return new Hint();
}

void Hint::rebuildHints()
{
    {
        View* focusParent    = Application::getCurrentFocus();
        View* hintBaseParent = this;

        while (focusParent != nullptr)
        {
            if (focusParent->getParent() == nullptr)
                break;
            focusParent = focusParent->getParent();
        }

        while (hintBaseParent != nullptr)
        {
            if (hintBaseParent->getParent() == nullptr)
                break;
            hintBaseParent = hintBaseParent->getParent();
        }

        if (focusParent != hintBaseParent)
            return;
    }

    for (auto view : this->getChildren())
        this->removeView(view);
        
    std::set<ControllerButton> addedButtons;
    View* focusParent = Application::getCurrentFocus();

    std::vector<Action> actions;

    while (focusParent != nullptr)
    {
        for (auto &action : focusParent->getActions())
        {
            if (action.hidden)
                continue;

            if (addedButtons.find(action.button) != addedButtons.end())
                continue;

            addedButtons.insert(action.button);
            actions.push_back(action);
        }

        focusParent = focusParent->getParent();
    }

    std::stable_sort(actions.begin(), actions.end(), actionsSortFunc);

    for (Action action : actions)
    {
        std::string hintTxt = getKeyIcon(action.button) + "  " + action.hintText;

        Label *label = new Label();
        label->setFontSize(22.0f);
        label->setText(hintTxt);
        label->setMarginRight(30.0f);
        this->addView(label);
    }
}

} // namespace brls