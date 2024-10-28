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

#pragma once

#include <QColor>
#include <QObject>
#include <QPalette>
#include <QPointer>
#include <QStyle>

#include <qqmlregistration.h>

class PStylePalette;

class PStyleColorGroup : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QColor alternateBase READ alternateBase WRITE setAlternateBase RESET resetAlternateBase NOTIFY alternateBaseChanged FINAL)
    Q_PROPERTY(QColor base READ base WRITE setBase RESET resetBase NOTIFY baseChanged FINAL)
    Q_PROPERTY(QColor brightText READ brightText WRITE setBrightText RESET resetBrightText NOTIFY brightTextChanged FINAL)
    Q_PROPERTY(QColor button READ button WRITE setButton RESET resetButton NOTIFY buttonChanged FINAL)
    Q_PROPERTY(QColor buttonText READ buttonText WRITE setButtonText RESET resetButtonText NOTIFY buttonTextChanged FINAL)
    Q_PROPERTY(QColor dark READ dark WRITE setDark RESET resetDark NOTIFY darkChanged FINAL)
    Q_PROPERTY(QColor highlight READ highlight WRITE setHighlight RESET resetHighlight NOTIFY highlightChanged FINAL)
    Q_PROPERTY(
        QColor highlightedText READ highlightedText WRITE setHighlightedText RESET resetHighlightedText NOTIFY highlightedTextChanged FINAL)
    Q_PROPERTY(QColor light READ light WRITE setLight RESET resetLight NOTIFY lightChanged FINAL)
    Q_PROPERTY(QColor link READ link WRITE setLink RESET resetLink NOTIFY linkChanged FINAL)
    Q_PROPERTY(QColor linkVisited READ linkVisited WRITE setLinkVisited RESET resetLinkVisited NOTIFY linkVisitedChanged FINAL)
    Q_PROPERTY(QColor mid READ mid WRITE setMid RESET resetMid NOTIFY midChanged FINAL)
    Q_PROPERTY(QColor midlight READ midlight WRITE setMidlight RESET resetMidlight NOTIFY midlightChanged FINAL)
    Q_PROPERTY(QColor shadow READ shadow WRITE setShadow RESET resetShadow NOTIFY shadowChanged FINAL)
    Q_PROPERTY(QColor text READ text WRITE setText RESET resetText NOTIFY textChanged FINAL)
    Q_PROPERTY(QColor toolTipBase READ toolTipBase WRITE setToolTipBase RESET resetToolTipBase NOTIFY toolTipBaseChanged FINAL)
    Q_PROPERTY(QColor toolTipText READ toolTipText WRITE setToolTipText RESET resetToolTipText NOTIFY toolTipTextChanged FINAL)
    Q_PROPERTY(QColor window READ window WRITE setWindow RESET resetWindow NOTIFY windowChanged FINAL)
    Q_PROPERTY(QColor windowText READ windowText WRITE setWindowText RESET resetWindowText NOTIFY windowTextChanged FINAL)
    Q_PROPERTY(
        QColor placeholderText READ placeholderText WRITE setPlaceholderText RESET resetPlaceholderText NOTIFY placeholderTextChanged FINAL)
    Q_PROPERTY(QColor accent READ accent WRITE setAccent RESET resetAccent NOTIFY accentChanged FINAL)

   public:
    explicit PStyleColorGroup(QObject* parent = nullptr);
    ~PStyleColorGroup();

    QColor alternateBase() const;
    void setAlternateBase(const QColor& color);
    void resetAlternateBase();

    QColor base() const;
    void setBase(const QColor& color);
    void resetBase();

    QColor brightText() const;
    void setBrightText(const QColor& color);
    void resetBrightText();

    QColor button() const;
    void setButton(const QColor& color);
    void resetButton();

    QColor buttonText() const;
    void setButtonText(const QColor& color);
    void resetButtonText();

    QColor dark() const;
    void setDark(const QColor& color);
    void resetDark();

    QColor highlight() const;
    void setHighlight(const QColor& color);
    void resetHighlight();

    QColor highlightedText() const;
    void setHighlightedText(const QColor& color);
    void resetHighlightedText();

    QColor light() const;
    void setLight(const QColor& color);
    void resetLight();

    QColor link() const;
    void setLink(const QColor& color);
    void resetLink();

    QColor linkVisited() const;
    void setLinkVisited(const QColor& color);
    void resetLinkVisited();

    QColor mid() const;
    void setMid(const QColor& color);
    void resetMid();

    QColor midlight() const;
    void setMidlight(const QColor& color);
    void resetMidlight();

    QColor shadow() const;
    void setShadow(const QColor& color);
    void resetShadow();

    QColor text() const;
    void setText(const QColor& color);
    void resetText();

    QColor toolTipBase() const;
    void setToolTipBase(const QColor& color);
    void resetToolTipBase();

    QColor toolTipText() const;
    void setToolTipText(const QColor& color);
    void resetToolTipText();

    QColor window() const;
    void setWindow(const QColor& color);
    void resetWindow();

    QColor windowText() const;
    void setWindowText(const QColor& color);
    void resetWindowText();

    QColor placeholderText() const;
    void setPlaceholderText(const QColor& color);
    void resetPlaceholderText();

    QColor accent() const;
    void setAccent(const QColor& color);
    void resetAccent();

    QPalette::ColorGroup groupTag() const;
    void setGroupTag(QPalette::ColorGroup tag);

    virtual void resetAll();

    static PStyleColorGroup* createWithParent(PStylePalette& parent);

    void setPalette(const QPalette&);

   signals:
    void alternateBaseChanged();
    void baseChanged();
    void brightTextChanged();
    void buttonChanged();
    void buttonTextChanged();
    void darkChanged();
    void highlightChanged();
    void highlightedTextChanged();
    void lightChanged();
    void linkChanged();
    void linkVisitedChanged();
    void midChanged();
    void midlightChanged();
    void shadowChanged();
    void textChanged();
    void toolTipBaseChanged();
    void toolTipTextChanged();
    void windowChanged();
    void windowTextChanged();
    void placeholderTextChanged();
    void accentChanged();

    void changed();

   protected:
    explicit PStyleColorGroup(PStylePalette& parent);

    static constexpr QPalette::ColorGroup defaultGroupTag() { return QPalette::Active; }

    virtual QPalette::ColorGroup currentColorGroup() const;

   private:
    using Notifier = void (PStyleColorGroup::*)();

    QColor color(QPalette::ColorRole role) const;

   private:
    QPalette::ColorGroup m_groupTag;
    QPalette* m_palette = nullptr;

    QColor m_alternateBase;
    QColor m_base;
    QColor m_brightText;
    QColor m_button;
    QColor m_buttonText;
    QColor m_dark;
    QColor m_highlight;
    QColor m_highlightedText;
    QColor m_light;
    QColor m_link;
    QColor m_linkVisited;
    QColor m_mid;
    QColor m_midlight;
    QColor m_shadow;
    QColor m_text;
    QColor m_toolTipBase;
    QColor m_toolTipText;
    QColor m_window;
    QColor m_windowText;
    QColor m_placeholderText;
    QColor m_accent;
};

class PStylePalette : public PStyleColorGroup {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(PStyleColorGroup* active READ active RESET resetActive NOTIFY activeChanged FINAL)
    Q_PROPERTY(PStyleColorGroup* inactive READ inactive RESET resetInactive NOTIFY inactiveChanged FINAL)
    Q_PROPERTY(PStyleColorGroup* disabled READ disabled RESET resetDisabled NOTIFY disabledChanged FINAL)
   public:
    explicit PStylePalette(QObject* parent = nullptr);
    PStyleColorGroup* active() const;
    PStyleColorGroup* inactive() const;
    PStyleColorGroup* disabled() const;
    void resetActive();
    void resetInactive();
    void resetDisabled();
    void resetAll() override;

   signals:
    void activeChanged();
    void inactiveChanged();
    void disabledChanged();

   private:
    QPointer<PStyleColorGroup> m_active;
    QPointer<PStyleColorGroup> m_disabled;
    QPointer<PStyleColorGroup> m_inactive;
};
