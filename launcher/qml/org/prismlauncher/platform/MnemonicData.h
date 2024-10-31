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
 *
 *    Licensed under LGPL-2.0-or-later 
 *          
 *          https://community.kde.org/Policies/Licensing_Policy
 */

/*
 * Modified from https://invent.kde.org/frameworks/kirigami/-/blob/master/src/mnemonicattached.h
 * under GPL-3.0
 */

#pragma once

#include <QObject>
#include <QQuickWindow>

#include <QQmlEngine>

/**
 * Attached Property used to calculate keyboard sequences int eh QWidgets style
 * using the "&" anchor in a label
 */
class MnemonicData : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_ATTACHED(MnemonicData)
    QML_UNCREATABLE("Cannot create objects of type MnemonicData, use it as an attached property")

    /**
     * The label of the control we want to compute a mnemonic for, instance
     * "Label:" or "&Ok"
     */
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged FINAL)

    /**
     * The user-visible final label, which will have the shortcut letter underlined,
     * such as "&lt;u&gt;O&lt;/u&gt;k"
     */
    Q_PROPERTY(QString richTextLabel READ richTextLabel NOTIFY richTextLabelChanged FINAL)

    /**
     * The label with an "&" mnemonic in the place which will have the shortcut
     * assigned, regardless of whether the & was assigned by the user or automatically generated.
     */
    Q_PROPERTY(QString mnemonicLabel READ mnemonicLabel NOTIFY mnemonicLabelChanged FINAL)

    /**
     * Only if true this mnemonic will be considered for the global assignment
     * default: true
     */
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)

    /**
     * The type of control this mnemonic is attached: different types of controls have different importance and priority for shortcut
     * assignment.
     * @see ControlType
     */
    Q_PROPERTY(MnemonicData::ControlType controlType READ controlType WRITE setControlType NOTIFY controlTypeChanged FINAL)

    /**
     * The final key sequence assigned, if any: it will be Alt+alphanumeric char
     */
    Q_PROPERTY(QKeySequence sequence READ sequence NOTIFY sequenceChanged FINAL)

    /**
     * True when the user is pressing alt and the accelerators should be shown
     *
     */
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged FINAL)

   public:
    enum ControlType {
        ActionElement,    /**< pushbuttons, checkboxes etc */
        DialogButton,     /**< buttons for dialogs */
        MenuItem,         /**< Menu items */
        FormLabel,        /**< Buddy label in a FormLayout*/
        SecondaryControl, /**< Other controls that are considered not much important and low priority for shortcuts */
    };
    Q_ENUM(ControlType)
    explicit MnemonicData(QObject* parent = nullptr);
    ~MnemonicData() override;

    void setLabel(const QString& text);
    QString label() const;

    QString richTextLabel() const;
    QString mnemonicLabel() const;

    void setEnabled(bool enabled);
    bool enabled() const;

    void setControlType(MnemonicData::ControlType controlType);
    ControlType controlType() const;

    QKeySequence sequence();

    void setActive(bool active);
    bool active() const;

    // QML attached property
    static MnemonicData* qmlAttachedProperties(QObject* object);

   protected:
    void updateSequence();

   Q_SIGNALS:
    void labelChanged();
    void enabledChanged();
    void sequenceChanged();
    void richTextLabelChanged();
    void mnemonicLabelChanged();
    void controlTypeChanged();
    void activeChanged();

   private:
    QWindow* window() const;

    void onAltPressed();
    void onAltReleased();

    void calculateWeights();

    enum {
        // Additional weight for first character in string
        FIRST_CHARACTER_EXTRA_WEIGHT = 50,
        // Additional weight for the beginning of a word
        WORD_BEGINNING_EXTRA_WEIGHT = 50,
        // Additional weight for a 'wanted' accelerator ie string with '&'
        WANTED_ACCEL_EXTRA_WEIGHT = 150,
        // Default weight for an 'action' widget (ie, pushbuttons)
        ACTION_ELEMENT_WEIGHT = 50,
        // Additional weight for the dialog buttons (large, we basically never want these reassigned)
        DIALOG_BUTTON_EXTRA_WEIGHT = 300,
        // Weight for FormLayout labels (low)
        FORM_LABEL_WEIGHT = 20,
        // Weight for Secondary controls which are considered less important (low)
        SECONDARY_CONTROL_WEIGHT = 10,
        // Default weight for menu items
        MENU_ITEM_WEIGHT = 250,
    };

    // order word letters by weight
    int m_weight = 0;
    int m_baseWeight = 0;
    ControlType m_controlType = SecondaryControl;
    QMap<int, QChar> m_weights;

    QString m_label;
    QString m_actualRichTextLabel;
    QString m_richTextLabel;
    QString m_mnemonicLabel;
    QKeySequence m_sequence;
    bool m_enabled = true;
    bool m_active = false;

    QPointer<QQuickWindow> m_window;

    // global mapping of mnemonics
    static QHash<QKeySequence, MnemonicData*> s_sequenceToObject;
};

QML_DECLARE_TYPEINFO(MnemonicData, QML_HAS_ATTACHED_PROPERTIES)

class MnemonicEventFilter : public QObject {
    Q_OBJECT

   public:
    static MnemonicEventFilter& instance()
    {
        static MnemonicEventFilter s_instance;
        return s_instance;
    }

    bool eventFilter(QObject* watched, QEvent* event) override
    {
        Q_UNUSED(watched);

        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* ke = static_cast<QKeyEvent*>(event);
            if (ke->key() == Qt::Key_Alt) {
                m_altPressed = true;
                Q_EMIT altPressed();
            }
        } else if (event->type() == QEvent::KeyRelease) {
            QKeyEvent* ke = static_cast<QKeyEvent*>(event);
            if (ke->key() == Qt::Key_Alt) {
                m_altPressed = false;
                Q_EMIT altReleased();
            }
        } else if (event->type() == QEvent::ApplicationStateChange) {
            if (m_altPressed) {
                m_altPressed = false;
                Q_EMIT altReleased();
            }
        }

        return false;
    }

   Q_SIGNALS:
    void altPressed();
    void altReleased();

   private:
    MnemonicEventFilter() : QObject(nullptr) { qGuiApp->installEventFilter(this); }

    bool m_altPressed = false;
};
