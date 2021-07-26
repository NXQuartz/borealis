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

#include <borealis/core/bind.hpp>
#include <borealis/core/box.hpp>
#include <borealis/views/label.hpp>

namespace brls
{

// Displays button hints for the currently focused view
// Button hints depends on the view's available actions
// There can only be one Hint visible at any time
struct Hint : public Box
{
    Hint();
    ~Hint();

    static View* create();

    void willAppear(bool resetState) override;
    void willDisappear(bool resetState) override;

    void setAnimated(bool animate);

    private:
    bool animate;

    GenericEvent::Subscription globalFocusEventSubscriptor;
    VoidEvent::Subscription globalHintsUpdateEventSubscriptor;

    std::vector<Label*> currentLabels;
    inline static std::vector<Hint*> globalHintStack;

    static void pushHint(Hint *hint);
    static void popHint(Hint *hint);
    static void animateHints();

    static std::string getKeyIcon(ControllerButton button);

    void rebuildHints();
};

} // namespace brls
