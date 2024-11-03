// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2024 Rachel Powers <508861+Ryex@users.noreply.github.com>
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2024 Rachel Powers <508861+Ryex@users.noreply.github.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "PStylePalette.h"
#include <qpalette.h>
#include "PQuickStyleItem.h"

PStyleColorGroup::PStyleColorGroup(PStylePalette& parent) : QObject(&parent), m_groupTag(defaultGroupTag()) {}

PStyleColorGroup::~PStyleColorGroup() {
    if (m_palette) {
        delete m_palette;
    }
}

QPalette::ColorGroup PStyleColorGroup::currentColorGroup() const
{
    return groupTag();
}

QColor PStyleColorGroup::alternateBase() const
{
    return m_alternateBase;
}

void PStyleColorGroup::setAlternateBase(const QColor& color)
{
    if (m_alternateBase != color) {
        m_alternateBase = color;
        Q_EMIT alternateBaseChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetAlternateBase()
{
    auto defaultColor = color(QPalette::AlternateBase);
    if (m_alternateBase != defaultColor) {
        m_alternateBase = defaultColor;
        Q_EMIT alternateBaseChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::base() const
{
    return m_base;
}

void PStyleColorGroup::setBase(const QColor& color)
{
    if (m_base != color) {
        m_base = color;
        Q_EMIT baseChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetBase()
{
    auto defaultColor = color(QPalette::Base);
    if (m_base != defaultColor) {
        m_base = defaultColor;
        Q_EMIT baseChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::brightText() const
{
    return m_brightText;
}

void PStyleColorGroup::setBrightText(const QColor& color)
{
    if (m_brightText != color) {
        m_brightText = color;
        Q_EMIT brightTextChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetBrightText()
{
    auto defaultColor = color(QPalette::BrightText);
    if (m_brightText != defaultColor) {
        m_brightText = defaultColor;
        Q_EMIT brightTextChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::button() const
{
    return m_button;
}

void PStyleColorGroup::setButton(const QColor& color)
{
    if (m_button != color) {
        m_button = color;
        Q_EMIT buttonChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetButton()
{
    auto defaultColor = color(QPalette::Button);
    if (m_button != defaultColor) {
        m_button = defaultColor;
        Q_EMIT buttonChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::buttonText() const
{
    return m_buttonText;
}

void PStyleColorGroup::setButtonText(const QColor& color)
{
    if (m_buttonText != color) {
        m_buttonText = color;
        Q_EMIT buttonTextChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetButtonText()
{
    auto defaultColor = color(QPalette::ButtonText);
    if (m_buttonText != defaultColor) {
        m_buttonText = defaultColor;
        Q_EMIT buttonTextChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::dark() const
{
    return m_dark;
}

void PStyleColorGroup::setDark(const QColor& color)
{
    if (m_dark != color) {
        m_dark = color;
        Q_EMIT darkChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetDark()
{
    auto defaultColor = color(QPalette::Dark);
    if (m_dark != defaultColor) {
        m_dark = defaultColor;
        Q_EMIT darkChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::highlight() const
{
    return m_highlight;
}

void PStyleColorGroup::setHighlight(const QColor& color)
{
    if (m_highlight != color) {
        m_highlight = color;
        Q_EMIT highlightChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetHighlight()
{
    auto defaultColor = color(QPalette::Highlight);
    if (m_highlight != defaultColor) {
        m_highlight = defaultColor;
        Q_EMIT highlightChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::highlightedText() const
{
    return m_highlightedText;
}

void PStyleColorGroup::setHighlightedText(const QColor& color)
{
    if (m_highlightedText != color) {
        m_highlightedText = color;
        Q_EMIT highlightedTextChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetHighlightedText()
{
    auto defaultColor = color(QPalette::HighlightedText);

    if (m_highlightedText != defaultColor) {
        m_highlightedText = defaultColor;
        Q_EMIT highlightedTextChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::light() const
{
    return m_light;
}

void PStyleColorGroup::setLight(const QColor& color)
{
    if (m_light != color) {
        m_light = color;
        Q_EMIT lightChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetLight()
{
    auto defaultColor = color(QPalette::Light);
    if (m_light != defaultColor) {
        m_light = defaultColor;
        Q_EMIT lightChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::link() const
{
    return m_link;
}

void PStyleColorGroup::setLink(const QColor& color)
{
    if (m_link != color) {
        m_link = color;
        Q_EMIT linkChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetLink()
{
    auto defaultColor = color(QPalette::Link);
    if (m_link != defaultColor) {
        m_link = defaultColor;
        Q_EMIT linkChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::linkVisited() const
{
    return m_linkVisited;
}

void PStyleColorGroup::setLinkVisited(const QColor& color)
{
    if (m_linkVisited != color) {
        m_linkVisited = color;
        Q_EMIT linkVisitedChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetLinkVisited()
{
    auto defaultColor = color(QPalette::LinkVisited);
    if (m_linkVisited != defaultColor) {
        m_linkVisited = defaultColor;
        Q_EMIT linkVisitedChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::mid() const
{
    return m_mid;
}

void PStyleColorGroup::setMid(const QColor& color)
{
    if (m_mid != color) {
        m_mid = color;
        Q_EMIT midChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetMid()
{
    auto defaultColor = color(QPalette::Mid);
    if (m_mid != defaultColor) {
        m_mid = defaultColor;
        Q_EMIT midChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::midlight() const
{
    return m_midlight;
}

void PStyleColorGroup::setMidlight(const QColor& color)
{
    if (m_midlight != color) {
        m_midlight = color;
        Q_EMIT midlightChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetMidlight()
{
    auto defaultColor = color(QPalette::Midlight);
    if (m_midlight != defaultColor) {
        m_midlight = defaultColor;
        Q_EMIT midlightChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::shadow() const
{
    return m_shadow;
}

void PStyleColorGroup::setShadow(const QColor& color)
{
    if (m_shadow != color) {
        m_shadow = color;
        Q_EMIT shadowChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetShadow()
{
    auto defaultColor = color(QPalette::Shadow);
    if (m_shadow != defaultColor) {
        m_shadow = defaultColor;
        Q_EMIT shadowChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::text() const
{
    return m_text;
}

void PStyleColorGroup::setText(const QColor& color)
{
    if (m_text != color) {
        m_text = color;
        Q_EMIT textChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetText()
{
    auto defaultColor = color(QPalette::Text);
    if (m_text != defaultColor) {
        m_text = defaultColor;
        Q_EMIT textChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::toolTipBase() const
{
    return m_toolTipBase;
}

void PStyleColorGroup::setToolTipBase(const QColor& color)
{
    if (m_toolTipBase != color) {
        m_toolTipBase = color;
        Q_EMIT toolTipBaseChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetToolTipBase()
{
    auto defaultColor = color(QPalette::ToolTipBase);
    if (m_toolTipBase != defaultColor) {
        m_toolTipBase = defaultColor;
        Q_EMIT toolTipBaseChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::toolTipText() const
{
    return m_toolTipText;
}

void PStyleColorGroup::setToolTipText(const QColor& color)
{
    if (m_toolTipText != color) {
        m_toolTipText = color;
        Q_EMIT toolTipTextChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetToolTipText()
{
    auto defaultColor = color(QPalette::ToolTipText);
    if (m_toolTipText != defaultColor) {
        m_toolTipText = defaultColor;
        Q_EMIT toolTipTextChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::window() const
{
    return m_window;
}

void PStyleColorGroup::setWindow(const QColor& color)
{
    if (m_window != color) {
        m_window = color;
        Q_EMIT windowChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetWindow()
{
    auto defaultColor = color(QPalette::Window);
    if (m_window != defaultColor) {
        m_window = defaultColor;
        Q_EMIT windowChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::windowText() const
{
    return m_windowText;
}

void PStyleColorGroup::setWindowText(const QColor& color)
{
    if (m_windowText != color) {
        m_windowText = color;
        Q_EMIT windowTextChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetWindowText()
{
    auto defaultColor = color(QPalette::WindowText);
    if (m_windowText != defaultColor) {
        m_windowText = defaultColor;
        Q_EMIT windowTextChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::placeholderText() const
{
    return m_placeholderText;
}

void PStyleColorGroup::setPlaceholderText(const QColor& color)
{
    if (m_placeholderText != color) {
        m_placeholderText = color;
        Q_EMIT placeholderTextChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetPlaceholderText()
{
    auto defaultColor = color(QPalette::PlaceholderText);
    if (m_placeholderText != defaultColor) {
        m_placeholderText = defaultColor;
        Q_EMIT placeholderTextChanged();
        Q_EMIT changed();
    }
}

QColor PStyleColorGroup::accent() const
{
    return m_accent;
}

void PStyleColorGroup::setAccent(const QColor& color)
{
    if (m_accent != color) {
        m_accent = color;
        Q_EMIT accentChanged();
        Q_EMIT changed();
    }
}

void PStyleColorGroup::resetAccent()
{
    auto defaultColor = color(QPalette::Accent);
    if (m_accent != defaultColor) {
        m_accent = defaultColor;
        Q_EMIT accentChanged();
        Q_EMIT changed();
    }
}

QPalette::ColorGroup PStyleColorGroup::groupTag() const
{
    return m_groupTag;
}

PStyleColorGroup::PStyleColorGroup(QObject* parent) : QObject(parent), m_groupTag(defaultGroupTag()) {}

void PStyleColorGroup::setGroupTag(QPalette::ColorGroup tag)
{
    if (m_groupTag != tag) {
        m_groupTag = tag;
        Q_EMIT changed();
    }
}

PStyleColorGroup* PStyleColorGroup::createWithParent(PStylePalette& parent)
{
    return new PStyleColorGroup(parent);
}

void PStyleColorGroup::setPalette(const QPalette& palette) {
    m_palette = new QPalette(palette);
}
QColor PStyleColorGroup::color(QPalette::ColorRole role) const
{
    if (m_palette) {
        return m_palette->color(currentColorGroup(), role);
    }
    return PQuickStyleItem::style()->standardPalette().color(currentColorGroup(), role);
}


void PStyleColorGroup::resetAll()
{
    resetAlternateBase();
    resetBase();
    resetBrightText();
    resetButton();
    resetButtonText();
    resetDark();
    resetHighlight();
    resetHighlightedText();
    resetLight();
    resetLink();
    resetLinkVisited();
    resetMid();
    resetMidlight();
    resetShadow();
    resetText();
    resetToolTipBase();
    resetToolTipText();
    resetWindow();
    resetWindowText();
    resetPlaceholderText();
    resetAccent();
}

PStylePalette::PStylePalette(QObject* parent) : PStyleColorGroup(parent) {}

PStyleColorGroup* PStylePalette::active() const {
    if (m_active) {
        return m_active;
    }
    const_cast<PStylePalette*>(this)->m_active = PStyleColorGroup::createWithParent(*const_cast<PStylePalette*>(this));

    m_active->setGroupTag(QPalette::Active);

    PStyleColorGroup::connect(m_active, &PStyleColorGroup::changed, this, &PStylePalette::changed);
    return m_active;
}

PStyleColorGroup* PStylePalette::inactive() const {
    if (m_inactive) {
        return m_inactive;
    }
    const_cast<PStylePalette*>(this)->m_inactive = PStyleColorGroup::createWithParent(*const_cast<PStylePalette*>(this));

    m_inactive->setGroupTag(QPalette::Inactive);

    PStyleColorGroup::connect(m_inactive, &PStyleColorGroup::changed, this, &PStylePalette::changed);
    return m_inactive;
}


PStyleColorGroup* PStylePalette::disabled() const {
    if (m_disabled) {
        return m_disabled;
    }
    const_cast<PStylePalette*>(this)->m_disabled = PStyleColorGroup::createWithParent(*const_cast<PStylePalette*>(this));

    m_disabled->setGroupTag(QPalette::Disabled);

    PStyleColorGroup::connect(m_disabled, &PStyleColorGroup::changed, this, &PStylePalette::changed);
    return m_disabled;
}

void PStylePalette::resetActive() {
    active()->resetAll();
}

void PStylePalette::resetInactive() {
    inactive()->resetAll();
}

void PStylePalette::resetDisabled() {
    disabled()->resetAll();
}

void PStylePalette::resetAll() {
    PStyleColorGroup::resetAll();
    resetActive();
    resetInactive();
    resetDisabled();
}
