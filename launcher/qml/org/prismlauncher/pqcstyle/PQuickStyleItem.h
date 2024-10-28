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
 *  This file incorporates work covered by the following copyright and
 *  permission notice:
 *
 *    Copyright © 2017 Marco Martin <mart@kde.org>
 *    Copyright © 2017 David Edmundson <davidedmundson@kde.org>
 *    Copyright © 2019 David Redondo <david@david-redondo.de>
 *    Copyright © 2023 ivan tkachenko <me@ratijas.tk>
 *
 *    Licensed under LGPL-3.0-only OR GPL-2.0-only OR
 *    GPL-3.0-only OR LicenseRef-KFQF-Accepted-GPL OR
 *    LicenseRef-Qt-Commercial
 *      
 *          https://community.kde.org/Policies/Licensing_Policy
 */

/*
 * PQuickStyleItem is heavly inspired by and modified from qqc2-desktop-style by KDE
 *
 * https://invent.kde.org/frameworks/qqc2-desktop-style
 * https://github.com/KDE/qqc2-desktop-style/blob/master/plugin/kquickstyleitem.cpphttps://invent.kde.org/frameworks/qqc2-desktop-style/-/blob/master/plugin/kquickstyleitem.cpp
 *
 * however it removed any dependence on other KDE dependencies such as kirigami
 *
 * Code reused and modified under GPL-3.0
 *
 */

#pragma once

#include "PQuickPadding.h"
#include "PStylePalette.h"

#include <QIcon>
#include <QImage>
#include <QLoggingCategory>
#include <QPointer>
#include <QQuickImageProvider>
#include <QQuickItem>

Q_DECLARE_LOGGING_CATEGORY(pqcstyleC)

#include <qqmlregistration.h>

#include <memory>

class QWidget;
class QStyleOption;
class QStyle;

class PQuickStyleItem : public QQuickItem {
    Q_OBJECT

    QML_NAMED_ELEMENT(PStyleItem)

    Q_PROPERTY(PQuickPadding* border READ border CONSTANT)

    Q_PROPERTY(bool sunken READ sunken WRITE setSunken NOTIFY sunkenChanged)
    Q_PROPERTY(bool raised READ raised WRITE setRaised NOTIFY raisedChanged)
    Q_PROPERTY(bool flat READ flat WRITE setFlat NOTIFY flatChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(bool hasFocus READ hasFocus WRITE setHasFocus NOTIFY hasFocusChanged)
    Q_PROPERTY(bool on READ on WRITE setOn NOTIFY onChanged)
    Q_PROPERTY(bool hover READ hover WRITE setHover NOTIFY hoverChanged)
    Q_PROPERTY(bool horizontal READ horizontal WRITE setHorizontal NOTIFY horizontalChanged)
    Q_PROPERTY(bool isTransient READ isTransient WRITE setTransient NOTIFY transientChanged)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString activeControl READ activeControl WRITE setActiveControl NOTIFY activeControlChanged)
    Q_PROPERTY(QString styleName READ styleName NOTIFY styleNameChanged)
    Q_PROPERTY(QVariantMap hints READ hints WRITE setHints NOTIFY hintChanged RESET resetHints)
    Q_PROPERTY(QVariantMap properties READ properties WRITE setProperties NOTIFY propertiesChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont RESET resetFont NOTIFY fontChanged)

    Q_PROPERTY(PStylePalette* palette READ palette NOTIFY paletteChanged)
    // For range controls
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int step READ step WRITE setStep NOTIFY stepChanged)
    Q_PROPERTY(int paintMargins READ paintMargins WRITE setPaintMargins NOTIFY paintMarginsChanged)

    Q_PROPERTY(int contentWidth READ contentWidth WRITE setContentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(int contentHeight READ contentHeight WRITE setContentHeight NOTIFY contentHeightChanged)

    Q_PROPERTY(int textureWidth READ textureWidth WRITE setTextureWidth NOTIFY textureWidthChanged)
    Q_PROPERTY(int textureHeight READ textureHeight WRITE setTextureHeight NOTIFY textureHeightChanged)

    Q_PROPERTY(int leftPadding READ leftPadding NOTIFY leftPaddingChanged)
    Q_PROPERTY(int topPadding READ topPadding NOTIFY topPaddingChanged)
    Q_PROPERTY(int rightPadding READ rightPadding NOTIFY rightPaddingChanged)
    Q_PROPERTY(int bottomPadding READ bottomPadding NOTIFY bottomPaddingChanged)

    Q_PROPERTY(QQuickItem* control READ control WRITE setControl NOTIFY controlChanged)

    PQuickPadding* border() { return &m_border; }

   public:
    PQuickStyleItem(QQuickItem* parent = nullptr);
    ~PQuickStyleItem() override;

    enum MenuItemType {
        SeparatorType = 0,
        ItemType,
        MenuType,
        ScrollIndicatorType,
    };

    void paint(QPainter*);
    bool sunken() const { return m_sunken; }
    bool raised() const { return m_raised; }
    bool flat() const { return m_flat; }
    bool active() const { return m_active; }
    bool selected() const { return m_selected; }
    bool hasFocus() const { return m_focus; }
    bool on() const { return m_on; }
    bool hover() const { return m_hover; }
    bool horizontal() const { return m_horizontal; }
    bool isTransient() const { return m_transient; }

    int minimum() const { return m_minimum; }
    int maximum() const { return m_maximum; }
    int step() const { return m_step; }
    int value() const { return m_value; }
    int paintMargins() const { return m_paintMargins; }

    QString elementType() const { return m_type; }
    QString text() const { return m_text; }
    QString activeControl() const { return m_activeControl; }
    QVariantMap hints() const { return m_hints; }
    QVariantMap properties() const { return m_properties; }

    PStylePalette* palette() const { return m_palette; }
    QFont font() const { return m_font; }
    QString styleName() const;

    void setSunken(bool sunken)
    {
        if (m_sunken != sunken) {
            m_sunken = sunken;
            polish();
            Q_EMIT sunkenChanged();
        }
    }
    void setRaised(bool raised)
    {
        if (m_raised != raised) {
            m_raised = raised;
            polish();
            Q_EMIT raisedChanged();
        }
    }
    void setFlat(bool flat)
    {
        if (m_flat != flat) {
            m_flat = flat;
            polish();
            Q_EMIT flatChanged();
        }
    }
    void setActive(bool active)
    {
        if (m_active != active) {
            m_active = active;
            polish();
            Q_EMIT activeChanged();
        }
    }
    void setSelected(bool selected)
    {
        if (m_selected != selected) {
            m_selected = selected;
            polish();
            Q_EMIT selectedChanged();
        }
    }
    void setHasFocus(bool focus)
    {
        if (m_focus != focus) {
            m_focus = focus;
            polish();
            Q_EMIT hasFocusChanged();
        }
    }
    void setOn(bool on)
    {
        if (m_on != on) {
            m_on = on;
            polish();
            Q_EMIT onChanged();
        }
    }
    void setHover(bool hover)
    {
        if (m_hover != hover) {
            m_hover = hover;
            polish();
            Q_EMIT hoverChanged();
        }
    }
    void setHorizontal(bool horizontal)
    {
        if (m_horizontal != horizontal) {
            m_horizontal = horizontal;
            updateSizeHint();
            polish();
            Q_EMIT horizontalChanged();
        }
    }
    void setTransient(bool transient)
    {
        if (m_transient != transient) {
            m_transient = transient;
            polish();
            Q_EMIT transientChanged();
        }
    }

    void setMinimum(int minimum)
    {
        if (m_minimum != minimum) {
            m_minimum = minimum;
            polish();
            Q_EMIT minimumChanged();
        }
    }
    void setMaximum(int maximum)
    {
        if (m_maximum != maximum) {
            m_maximum = maximum;
            polish();
            Q_EMIT maximumChanged();
        }
    }
    void setValue(int value)
    {
        if (m_value != value) {
            m_value = value;
            polish();
            Q_EMIT valueChanged();
        }
    }
    void setStep(int step)
    {
        if (m_step != step) {
            m_step = step;
            polish();
            Q_EMIT stepChanged();
        }
    }
    void setPaintMargins(int value)
    {
        if (m_paintMargins != value) {
            m_paintMargins = value;
            polish();
            Q_EMIT paintMarginsChanged();
        }
    }
    void setText(const QString& str)
    {
        if (m_text != str) {
            m_text = str;
            updateSizeHint();
            polish();
            Q_EMIT textChanged();
        }
    }
    void setHints(const QVariantMap& hints);
    void setProperties(const QVariantMap& props)
    {
        if (m_properties != props) {
            m_properties = props;
            m_iconDirty = true;
            updateSizeHint();
            polish();
            Q_EMIT propertiesChanged();
        }
    }
    void setFont(const QFont& font);
    void resetHints();
    void resetFont();

    int contentWidth() const { return m_contentWidth; }
    void setContentWidth(int arg);

    int contentHeight() const { return m_contentHeight; }
    void setContentHeight(int arg);

    void updateStyleOption();
    void resolvePalette();
    virtual QPalette getResolvedPalette();
    int topPadding() const;
    int leftPadding() const;
    int rightPadding() const;
    int bottomPadding() const;

    Q_INVOKABLE qreal textWidth(const QString&);
    Q_INVOKABLE qreal textHeight(const QString&);

    int textureWidth() const { return m_textureWidth; }
    void setTextureWidth(int w);

    int textureHeight() const { return m_textureHeight; }
    void setTextureHeight(int h);

    QQuickItem* control() const;
    void setControl(QQuickItem* control);

    static QStyle* style();

   public:
    // Virtual Public functions

    virtual void doPaint(QPainter*);

    // must call updateStyleOption
    // does general setup, calls doInitStyleOption for impl specific setup
    void initStyleOption();

    // initialize and update style Option
    // impl specific
    // should set m_preventMirroring and m_needsResolvePalette every call if different from defaults
    virtual void doInitStyleOption() {}

    virtual void setActiveControl(const QString& str)
    {
        if (m_activeControl != str) {
            m_activeControl = str;
            polish();
            Q_EMIT activeControlChanged();
        }
    }

    virtual QSize getContentSize(int width, int height);

   public Q_SLOTS:
    // Virtual Slots
    virtual QString hitTest(int x, int y);
    virtual QRectF subControlRect(const QString& subcontrolString);

   public Q_SLOTS:
    int pixelMetric(const QString&);
    QVariant styleHint(const QString&);
    void updateSizeHint();
    void updateRect();
    void updateBaselineOffset();
    void updateItem() { polish(); }

    QSize sizeFromContents(int width, int height);
    QRect computeBoundingRect(const QList<QRect>& rects);
    QString elidedText(const QString& text, int elideMode, int width);
    bool hasThemeIcon(const QString&) const;

   private Q_SLOTS:
    void updateFocusReason();

   Q_SIGNALS:
    void elementTypeChanged();
    void textChanged();
    void sunkenChanged();
    void raisedChanged();
    void flatChanged();
    void activeChanged();
    void selectedChanged();
    void hasFocusChanged();
    void onChanged();
    void hoverChanged();
    void horizontalChanged();
    void transientChanged();
    void minimumChanged();
    void maximumChanged();
    void stepChanged();
    void valueChanged();
    void activeControlChanged();
    void styleNameChanged();
    void paintMarginsChanged();
    void hintChanged();
    void propertiesChanged();
    void paletteChanged();
    void fontChanged();
    void controlChanged();

    void contentWidthChanged(int arg);
    void contentHeightChanged(int arg);

    void textureWidthChanged(int w);
    void textureHeightChanged(int h);

    void leftPaddingChanged();
    void topPaddingChanged();
    void rightPaddingChanged();
    void bottomPaddingChanged();

   protected:
    bool event(QEvent*) override;
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;
    void updatePolish() override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value) override;

    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

    virtual QIcon iconFromIconProperty() const;
    virtual int padding(Qt::Edge edge) const
    {
        Q_UNUSED(edge);
        return 0;
    };

    virtual qreal baselineOffset() const { return 0.0; };
    qreal baselineOffsetFromRect(QRect rect, bool ceilResult = true) const;

    virtual const char* classNameForItem() const { return ""; };

    // used to do setup for resolving a object palette
    // called when the style changes
    virtual void doResolvePalette() {}

   private:
    int handleWidth() const;
    void styleChanged();

   protected:
    QStyleOption* m_styleoption;
    QPointer<QQuickItem> m_control;
    QPointer<QWindow> m_window;

    QString m_type;
    QString m_text;
    QString m_activeControl;
    QVariantMap m_hints;
    QVariantMap m_properties;
    QFont m_font;

    bool m_sunken;
    bool m_raised;
    bool m_flat;
    bool m_active;
    bool m_selected;
    bool m_focus;
    bool m_hover;
    bool m_on;
    bool m_horizontal;
    bool m_transient;
    bool m_sharedWidget;
    bool m_iconDirty = true;

    int m_minimum;
    int m_maximum;
    int m_value;
    int m_step;
    int m_paintMargins;

    int m_contentWidth;
    int m_contentHeight;

    int m_textureWidth;
    int m_textureHeight;

    bool m_preventMirroring = false;
    bool m_needsResolvePalette = true;

    QMetaProperty m_focusReasonProperty;
    Qt::FocusReason m_focusReason;

    QImage m_image;
    PQuickPadding m_border;

    static std::unique_ptr<QStyle> s_qstyle;
    QPointer<PStylePalette> m_palette;
};
