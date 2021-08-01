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

#include <borealis/core/actions.hpp>
#include <borealis/core/application.hpp>
#include <borealis/views/hint.hpp>
#include <set>

namespace brls
{

const std::string hintXML = R"xml(
    <brls:Box
        width="auto"
        height="auto"
        axis="row"
        direction="leftToRight">

        <!-- Hint labels will be inserted into here -->

    </brls:Box>
)xml";

Hint::Hint()
    : Box(Axis::ROW)
{
    this->inflateFromXMLString(hintXML);

    this->registerBoolXMLAttribute("animated", [this](bool value){
        this->setAnimated(value);
    });

    this->globalFocusEventSubscriptor = Application::getGlobalFocusChangeEvent()->subscribe([this](View *newFocus)
    {
        this->rebuildHints();
    });

    this->globalHintsUpdateEventSubscriptor = Application::getGlobalHintsUpdateEvent()->subscribe([this]()
    {
        this->rebuildHints();
    });
}

void Hint::setAnimated(bool animate)
{
    this->animate = animate;
}

Hint::~Hint()
{
    Application::getGlobalFocusChangeEvent()->unsubscribe(this->globalFocusEventSubscriptor);
    Application::getGlobalHintsUpdateEvent()->unsubscribe(this->globalHintsUpdateEventSubscriptor);
}

View* Hint::create()
{
    return new Hint();
}

bool actionsSortFunc(Action a, Action b)
{
    if (a.button == ControllerButton::BUTTON_START)
        return true;

    if (b.button == ControllerButton::BUTTON_A)
        return true;

    if (b.button == ControllerButton::BUTTON_B && a.button != ControllerButton::BUTTON_A)
        return true;

    return false;
}

void Hint::rebuildHints()
{
    {
        View *focusParent = Application::getCurrentFocus();
        View *hintBaseParent = this;

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

    if (!currentLabels.empty())
    {
        for (auto *e : currentLabels)
        {
            this->removeView(e);
            //delete e;
        }
        currentLabels.clear();
    }

    std::set<ControllerButton> addedButtons;
    View *focusParent = Application::getCurrentFocus();

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

    for (Action a : actions)
    {
        std::string hintTxt = Hint::getKeyIcon(a.button) + "  " + a.hintText;

        Label *label = new Label();
        label->setFontSize(22.0f);
        label->setText(hintTxt);
        label->setMarginRight(30.0f);

        this->addView(label);
        currentLabels.push_back(label);
    }
}

std::string Hint::getKeyIcon(ControllerButton button)
{
    switch (button)
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
            return "\uE152";
    }
}

void Hint::willAppear(bool resetState)
{
    Hint::pushHint(this);
}

void Hint::willDisappear(bool resetState)
{
    Hint::popHint(this);
}

void Hint::pushHint(Hint* hint)
{
    Hint::globalHintStack.erase(std::remove(Hint::globalHintStack.begin(), Hint::globalHintStack.end(), hint), Hint::globalHintStack.end());
    Hint::globalHintStack.push_back(hint);

    Hint::animateHints();
}

void Hint::popHint(Hint* hint)
{
    Hint::globalHintStack.erase(std::remove(Hint::globalHintStack.begin(), Hint::globalHintStack.end(), hint), Hint::globalHintStack.end());
    Hint::animateHints();
}

void Hint::animateHints()
{
    for (size_t i = 0; i < Hint::globalHintStack.size(); i++)
    {
        if (i == Hint::globalHintStack.size() - 1)
            Hint::globalHintStack[i]->show([]{});
        else
            Hint::globalHintStack[i]->hide([]{});
    }
}

} // namespace brls
