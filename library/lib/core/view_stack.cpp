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

#include <borealis/core/view_stack.hpp>
//#include <borealis/core/application.hpp>

namespace brls
{

void ViewStack::pushView(View *view, TransitionAnimation animation)
{
    View *last = nullptr;
    if (viewStack.size() > 0)
        last = viewStack[viewStack.size() - 1];

    bool fadeOut = last && !last->isTranslucent() && !view->isTranslucent();
    bool wait = animation == TransitionAnimation::FADE;

    if (fadeOut)
    {
        view->setInFadeAnimation(true);

        if (!wait)
            view->show([]{}, true, view->getShowAnimationDuration(animation));
        
        last->hide([this, animation, wait] {
            View *newLast = this->viewStack[this->viewStack.size() - 1];
            newLast->setInFadeAnimation(false);

            if (wait)
                newLast->show([]{}, true, newLast->getShowAnimationDuration(animation));
        }, true, last->getShowAnimationDuration(animation));
    }

    if (!fadeOut)
        view->show([]{}, true, view->getShowAnimationDuration(animation));
    else
        view->setAlpha(0.0f);

    view->willAppear(true);
    viewStack.push_back(view);
}

void ViewStack::popView(TransitionAnimation animation, std::function<void(void)> cb)
{
    if (viewStack.size() <= 1) // Never pop the first view
        return;

    View *last = viewStack[viewStack.size() - 1];
    last->willDisappear(true);
    last->setInFadeAnimation(true);

    bool wait = animation == TransitionAnimation::FADE;

    last->hide([this, last, animation, wait, cb] {
        last->setInFadeAnimation(false);
        this->viewStack.pop_back();
        delete last;

        if (this->viewStack.size() > 0 && wait)
        {
            View *newLast = this->viewStack[this->viewStack.size() - 1];

            if (newLast->isHidden())
            {
                newLast->willAppear(false);
                newLast->show(cb, true, newLast->getShowAnimationDuration(animation));
            }
            else
                cb();
        }
    }, true, last->getShowAnimationDuration(animation));

    if (!wait && viewStack.size() > 1)
    {
        View *toShow = viewStack[viewStack.size() - 2];
        toShow->willAppear(false);
        toShow->show(cb, true, toShow->getShowAnimationDuration(animation));
    }
}

View *ViewStack::currentView()
{
    return viewStack[viewStack.size() - 1];
}

} // namespace brls
