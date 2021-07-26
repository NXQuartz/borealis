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

#include <borealis/core/view.hpp>
#include <vector>

namespace brls
{

// A view stack contains multiple views
// which new ones can be pushed, old ones can be poped, and the current view can be accessed
// TODO: Add a transtition between pushs and pops (will be optional)
struct ViewStack
{
    ViewStack() = default;

    void pushView(View *view, TransitionAnimation animation);
    void popView(TransitionAnimation animation, std::function<void(void)> cb);

    View* currentView();

    private:
    std::vector<View*> viewStack;
};

} // namespace brls
