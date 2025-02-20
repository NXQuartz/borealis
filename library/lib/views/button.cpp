/*
    Copyright 2019-2021 natinusala
    Copyright 2019 p-sam

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

#include <math.h>

#include <borealis/core/application.hpp>
#include <borealis/views/button.hpp>

namespace brls
{

const std::string buttonXML = R"xml(
    <brls:Box
        width="auto"
        height="auto"
        axis="row"
        focusable="true"
        justifyContent="center"
        alignItems="center"
        paddingTop="@style/brls/button/padding_top_bottom"
        paddingRight="@style/brls/button/padding_sides"
        paddingBottom="@style/brls/button/padding_top_bottom"
        paddingLeft="@style/brls/button/padding_sides"
        cornerRadius="@style/brls/button/corner_radius"
        highlightCornerRadius="@style/brls/button/corner_radius">

    <brls:Label
        id="brls/button/label"
        width="auto"
        height="auto"
        fontSize="@style/brls/button/text_size"
        horizontalAlign="center" />

    </brls:Box>
)xml";

const std::string buttonThemeXML = R"xml(
    <brls:Stylesheet theme="brls/default" prefix="brls/button">
        <brls:ThemeVariant name="light">
            <brls:Color name="primary_enabled_background" value="rgb(50,79,241)"/>
            <brls:Color name="primary_disabled_background" value="rgb(201,201,209)"/>
            <brls:Color name="primary_enabled_text" value="rgb(255,255,255)"/>
            <brls:Color name="primary_disabled_text" value="rgb(220,220,228)"/>

            <brls:Color name="default_enabled_background" value="rgb(255,255,255)"/>
            <brls:Color name="default_disabled_background" value="rgb(255,255,255)"/>
            <brls:Color name="default_enabled_text" value="rgb(45,45,45)"/>
            <brls:Color name="default_disabled_text" value="rgb(45,45,45)"/>

            <brls:Color name="highlight_enabled_text" value="rgb(49,79,235)"/>
            <brls:Color name="highlight_disabled_text" value="rgb(49,79,235)"/>

            <brls:Color name="enabled_border_color" value="rgb(45,45,45)"/>
            <brls:Color name="disabled_border_color" value="rgb(45,45,45)"/>
        </brls:ThemeVariant>

        <brls:ThemeVariant name="dark">
            <brls:Color name="primary_enabled_background" value="rgb(1,255,201)"/>
            <brls:Color name="primary_disabled_background" value="rgb(83,87,86)"/>
            <brls:Color name="primary_enabled_text" value="rgb(52,41,55)"/>
            <brls:Color name="primary_disabled_text" value="rgb(71,75,74)"/>

            <brls:Color name="default_enabled_background" value="rgb(80,80,80)"/>
            <brls:Color name="default_disabled_background" value="rgb(80,80,80)"/>
            <brls:Color name="default_enabled_text" value="rgb(255,255,255)"/>
            <brls:Color name="default_disabled_text" value="rgb(255,255,255)"/>

            <brls:Color name="highlight_enabled_text" value="rgb(7,247,198)"/>
            <brls:Color name="highlight_disabled_text" value="rgb(7,247,198)"/>

            <brls:Color name="enabled_border_color" value="rgb(255,255,255)"/>
            <brls:Color name="disabled_border_color" value="rgb(255,255,255)"/>
        </brls:ThemeVariant>

        <brls:Metric name="padding_top_bottom" value="15.0"/>
        <brls:Metric name="padding_sides" value="25.0"/>
        <brls:Metric name="corner_radius" value="5.0"/>
        <brls:Metric name="text_size" value="18.0"/>
        <brls:Metric name="primary_highlight_padding" value="2.0"/>
        <brls:Metric name="border_thickness" value="2.0"/>
    </brls:Stylesheet>
)xml";

Button::Button()
{
    Application::getTheme().inflateFromXMLString(buttonThemeXML);

    this->inflateFromXMLString(buttonXML);

    this->forwardXMLAttribute("text", this->label);
    this->forwardXMLAttribute("singleLine", this->label);
    this->forwardXMLAttribute("fontSize", this->label);
    this->forwardXMLAttribute("textColor", this->label);
    this->forwardXMLAttribute("lineHeight", this->label);
    this->forwardXMLAttribute("animated", this->label);
    this->forwardXMLAttribute("autoAnimate", this->label);
    this->forwardXMLAttribute("textHorizontalAlign", this->label, "horizontalAlign");

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "style", const ButtonStyle*, this->setStyle,
        {
            { "default", &BUTTONSTYLE_DEFAULT },
            { "primary", &BUTTONSTYLE_PRIMARY },
            { "highlight", &BUTTONSTYLE_HIGHLIGHT },
            { "bordered", &BUTTONSTYLE_BORDERED },
            { "borderless", &BUTTONSTYLE_BORDERLESS },
        });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "state", ButtonState, this->setState,
        {
            { "enabled", ButtonState::ENABLED },
            { "disabled", ButtonState::DISABLED },
        });

    this->applyStyle();
}

void Button::applyStyle()
{
    Style style = Application::getStyle();
    Theme theme = Application::getTheme();

    this->setShadowType(this->style->shadowType);
    this->setHideHighlightBackground(this->style->hideHighlightBackground);

    if (this->style->highlightPadding != "")
        this->setHighlightPadding(style[this->style->highlightPadding]);
    else
        this->setHighlightPadding(0.0f);

    if (this->style->borderThickness != "")
        this->setBorderThickness(style[this->style->borderThickness]);
    else
        this->setBorderThickness(0.0f);

    std::string backgroundColor;
    std::string textColor;
    std::string borderColor;

    switch (this->state)
    {
        case ButtonState::ENABLED:
            backgroundColor = this->style->enabledBackgroundColor;
            textColor       = this->style->enabledLabelColor;
            borderColor     = this->style->enabledBorderColor;

            break;
        case ButtonState::DISABLED:
            backgroundColor = this->style->disabledBackgroundColor;
            textColor       = this->style->disabledLabelColor;
            borderColor     = this->style->disabledBorderColor;

            break;
    }

    if (backgroundColor != "")
        this->setBackgroundColor(theme[backgroundColor]);
    else
        this->setBackground(ViewBackground::NONE);

    this->label->setTextColor(theme[textColor]);

    if (this->getBorderThickness() > 0.0f)
        this->setBorderColor(theme[borderColor]);
}

void Button::onFocusGained()
{
    Box::onFocusGained();

    this->setShadowVisibility(false);
}

void Button::onFocusLost()
{
    Box::onFocusLost();

    this->setShadowVisibility(true);
}

void Button::setStyle(const ButtonStyle* style)
{
    this->style = style;
    this->applyStyle();
}

void Button::setState(ButtonState state)
{
    this->state = state;
    this->applyStyle();
}

void Button::setText(std::string text)
{
    this->label->setText(text);
}

std::string Button::getText()
{
    return label->getFullText();
}

View* Button::create()
{
    return new Button();
}

} // namespace brls
