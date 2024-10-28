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
 * https://github.com/KDE/qqc2-desktop-style/blob/master/plugin/kquickstyleitem.cpp
 * https://invent.kde.org/frameworks/qqc2-desktop-style/-/blob/master/plugin/kquickstyleitem.cpp
 *
 * however it removed any dependence on other KDE dependencies such as kirigami
 *
 * Code reused and modified under GPL-3.0
 *
 */

#include "PQuickStyleItem.h"

#include <qloggingcategory.h>
#include <qsgninepatchnode.h>
#include <qstyleoption.h>
#include <qtpreprocessorsupport.h>
#include <QApplication>
#include <QPainter>
#include <QPixmapCache>
#include <QQuickWindow>
#include <QStringBuilder>
#include <QStyle>
#include <QStyleFactory>

Q_LOGGING_CATEGORY(pqcstyleC, "org.prismlauncher.pqcstyle")

std::unique_ptr<QStyle> PQuickStyleItem::s_qstyle = nullptr;

QStyle* PQuickStyleItem::style()
{
    if (s_qstyle) {
        return s_qstyle.get();
    } else {
        return qApp->style();
    }
}

static bool isKeyFocusReason(Qt::FocusReason reason)
{
    return reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason;
}

PQuickStyleItem::PQuickStyleItem(QQuickItem* parent)
    : QQuickItem(parent)
    , m_styleoption(nullptr)
    , m_sunken(false)
    , m_raised(false)
    , m_flat(false)
    , m_active(true)
    , m_selected(false)
    , m_focus(false)
    , m_hover(false)
    , m_on(false)
    , m_horizontal(true)
    , m_transient(false)
    , m_sharedWidget(false)
    , m_minimum(0)
    , m_maximum(100)
    , m_value(0)
    , m_step(0)
    , m_paintMargins(0)
    , m_contentWidth(0)
    , m_contentHeight(0)
    , m_textureWidth(0)
    , m_textureHeight(0)
    , m_focusReason(Qt::NoFocusReason)
{
    // Check that we really are a QApplication before attempting to access the
    // application style.
    //
    // Functions used in this file which are safe to access through qApp are:
    //
    //   qApp->font() and qApp->fontChanged() - provided by QGuiApplication
    //   qApp->font("classname") - provided by QApplication but safe
    //   qApp->layoutDirection() - provided by QGuiApplication
    //   qApp->wheelScrollLines() - uses styleHints() provided by QGuiApplication
    //   qApp->testAttribute() and qApp->setAttribute() - provided by QCoreApplication
    //   qApp->devicePixelRatio() - provided by QGuiApplication
    //   qApp->palette("classname") - provided by QApplication but safe
    //
    // but any use of qApp->style(), even if only trying to test that it exists,
    // will assert.
    //
    // Any use of any other function provided by QApplication must either be checked
    // to ensure that it is safe to use if not a QApplication, or guarded.
    if (qobject_cast<QApplication*>(QCoreApplication::instance())) {
        // We are a QApplication.  Unfortunately there is no styleChanged signal
        // available, and it only sends QEvent::StyleChange events to all QWidgets.
        // So watch for the existing application style being destroyed, which means
        // that a new one is being applied.  See QApplication::setStyle().
        QStyle* style = qApp->style();
        if (style) {
            connect(style, &QObject::destroyed, this, &PQuickStyleItem::styleChanged);
        }
    } else if (!s_qstyle) {
        // We are not a QApplication.  Create an internal copy of the configured
        // desktop style, to be used for metrics, options and painting.
        const QString defaultStyleName = QStringLiteral("Fusion");
        s_qstyle.reset(QStyleFactory::create(defaultStyleName));
        // Prevent inevitable crashes on nullptr dereference
        if (!s_qstyle) {
            qWarning() << "org.prismlauncher.pqcstyle: Could not find QStyle Fusion";
            ::exit(EXIT_FAILURE);
        }
    }

    doResolvePalette();
    m_palette = new PStylePalette(this);

    m_font = qApp->font();
    setFlag(QQuickItem::ItemHasContents, true);
    setSmooth(false);

    qGuiApp->installEventFilter(this);
}

PQuickStyleItem::~PQuickStyleItem()
{
    if (const QStyleOptionButton* aux = qstyleoption_cast<const QStyleOptionButton*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionViewItem* aux = qstyleoption_cast<const QStyleOptionViewItem*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionHeader* aux = qstyleoption_cast<const QStyleOptionHeader*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionToolButton* aux = qstyleoption_cast<const QStyleOptionToolButton*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionToolBar* aux = qstyleoption_cast<const QStyleOptionToolBar*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionTab* aux = qstyleoption_cast<const QStyleOptionTab*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionFrame* aux = qstyleoption_cast<const QStyleOptionFrame*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionFocusRect* aux = qstyleoption_cast<const QStyleOptionFocusRect*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionTabWidgetFrame* aux = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionMenuItem* aux = qstyleoption_cast<const QStyleOptionMenuItem*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionComboBox* aux = qstyleoption_cast<const QStyleOptionComboBox*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionSpinBox* aux = qstyleoption_cast<const QStyleOptionSpinBox*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionSlider* aux = qstyleoption_cast<const QStyleOptionSlider*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionProgressBar* aux = qstyleoption_cast<const QStyleOptionProgressBar*>(m_styleoption)) {
        delete aux;
    } else if (const QStyleOptionGroupBox* aux = qstyleoption_cast<const QStyleOptionGroupBox*>(m_styleoption)) {
        delete aux;
    } else {
        delete m_styleoption;
    }

    m_styleoption = nullptr;
}

QPalette PQuickStyleItem::getResolvedPalette() {
    if (s_qstyle) {
        return s_qstyle->standardPalette();
    } else if (!QString(classNameForItem()).isEmpty()){
        return qApp->palette(classNameForItem());
    } else {
        return qApp->palette();
    }
}

void PQuickStyleItem::initStyleOption()
{
    if (m_styleoption) {
        m_styleoption->state = {};
    }

    doInitStyleOption();

    updateStyleOption();
}

void PQuickStyleItem::updateStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOption();
    }

    auto palette = this->palette();
    if (m_needsResolvePalette) {
        resolvePalette();
        palette->setPalette(m_styleoption->palette);
    }
    // propagate the option palette out
    palette->resetAll();

    m_styleoption->styleObject = this;
    const auto mirror = m_control == nullptr ? qApp->layoutDirection() == Qt::RightToLeft : m_control->property("mirrored").toBool();
    m_styleoption->direction = (mirror && !m_preventMirroring) ? Qt::RightToLeft : Qt::LeftToRight;

    int w = m_textureWidth > 0 ? m_textureWidth : width();
    int h = m_textureHeight > 0 ? m_textureHeight : height();

    m_styleoption->rect = QRect(m_paintMargins, 0, w - 2 * m_paintMargins, h);

    if (isEnabled()) {
        m_styleoption->state |= QStyle::State_Enabled;
        m_styleoption->palette.setCurrentColorGroup(QPalette::Active);
    } else {
        m_styleoption->palette.setCurrentColorGroup(QPalette::Disabled);
    }

    if (m_active) {
        m_styleoption->state |= QStyle::State_Active;
    } else {
        m_styleoption->palette.setCurrentColorGroup(QPalette::Inactive);
    }

    if (m_sunken) {
        m_styleoption->state |= QStyle::State_Sunken;
    }
    if (!m_sunken || m_raised) {
        m_styleoption->state |= QStyle::State_Raised;
    }
    if (m_selected) {
        m_styleoption->state |= QStyle::State_Selected;
    }
    if (m_focus) {
        m_styleoption->state |= QStyle::State_HasFocus;
    }
    if (m_on) {
        m_styleoption->state |= QStyle::State_On;
    }
    if (m_hover) {
        m_styleoption->state |= QStyle::State_MouseOver;
    }
    if (m_horizontal) {
        m_styleoption->state |= QStyle::State_Horizontal;
    }

    // some styles don't draw a focus rectangle if
    // QStyle::State_KeyboardFocusChange is not set
    if (window()) {
        if (isKeyFocusReason(m_focusReason)) {
            m_styleoption->state |= QStyle::State_KeyboardFocusChange;
        }
    }

    QString sizeHint = m_hints.value(QStringLiteral("size")).toString();
    if (sizeHint == QLatin1String("mini")) {
        m_styleoption->state |= QStyle::State_Mini;
    } else if (sizeHint == QLatin1String("small")) {
        m_styleoption->state |= QStyle::State_Small;
    }
}

QIcon PQuickStyleItem::iconFromIconProperty() const
{
    QIcon icon;
    const QVariant iconProperty = m_properties[QStringLiteral("icon")];
    switch (iconProperty.userType()) {
        case QMetaType::QIcon:
            icon = iconProperty.value<QIcon>();
            break;
        case QMetaType::QUrl:
        case QMetaType::QString: {
            QString iconSource = iconProperty.toString();
            if (iconSource.startsWith(QLatin1String("qrc:/"))) {
                iconSource = iconSource.mid(3);
            } else if (iconSource.startsWith(QLatin1String("file:/"))) {
                iconSource = QUrl(iconSource).toLocalFile();
            }
            if (iconSource.contains(QLatin1String("/"))) {
                icon = QIcon(iconSource);
            } else if (!iconSource.isEmpty()) {
                if (iconSource == QStringLiteral("SP_TitleBarMinButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_TitleBarMinButton);
                } else if (iconSource == QStringLiteral("SP_TitleBarMenuButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_TitleBarMenuButton);
                } else if (iconSource == QStringLiteral("SP_TitleBarMaxButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_TitleBarMaxButton);
                } else if (iconSource == QStringLiteral("SP_TitleBarCloseButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_TitleBarCloseButton);
                } else if (iconSource == QStringLiteral("SP_TitleBarNormalButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_TitleBarNormalButton);
                } else if (iconSource == QStringLiteral("SP_TitleBarShadeButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_TitleBarShadeButton);
                } else if (iconSource == QStringLiteral("SP_TitleBarUnshadeButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_TitleBarUnshadeButton);
                } else if (iconSource == QStringLiteral("SP_TitleBarContextHelpButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_TitleBarContextHelpButton);
                } else if (iconSource == QStringLiteral("SP_MessageBoxInformation")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MessageBoxInformation);
                } else if (iconSource == QStringLiteral("SP_MessageBoxWarning")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MessageBoxWarning);
                } else if (iconSource == QStringLiteral("SP_MessageBoxCritical")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MessageBoxCritical);
                } else if (iconSource == QStringLiteral("SP_MessageBoxQuestion")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MessageBoxQuestion);
                } else if (iconSource == QStringLiteral("SP_DesktopIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DesktopIcon);
                } else if (iconSource == QStringLiteral("SP_TrashIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_TrashIcon);
                } else if (iconSource == QStringLiteral("SP_ComputerIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_ComputerIcon);
                } else if (iconSource == QStringLiteral("SP_DriveFDIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DriveFDIcon);
                } else if (iconSource == QStringLiteral("SP_DriveHDIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DriveHDIcon);
                } else if (iconSource == QStringLiteral("SP_DriveCDIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DriveCDIcon);
                } else if (iconSource == QStringLiteral("SP_DriveDVDIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DriveDVDIcon);
                } else if (iconSource == QStringLiteral("SP_DriveNetIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DriveNetIcon);
                } else if (iconSource == QStringLiteral("SP_DirHomeIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DirHomeIcon);
                } else if (iconSource == QStringLiteral("SP_DirOpenIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DirOpenIcon);
                } else if (iconSource == QStringLiteral("SP_DirClosedIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DirClosedIcon);
                } else if (iconSource == QStringLiteral("SP_DirIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DirIcon);
                } else if (iconSource == QStringLiteral("SP_DirLinkIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DirLinkIcon);
                } else if (iconSource == QStringLiteral("SP_DirLinkOpenIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DirLinkOpenIcon);
                } else if (iconSource == QStringLiteral("SP_FileIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileIcon);
                } else if (iconSource == QStringLiteral("SP_FileLinkIcon")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileLinkIcon);
                } else if (iconSource == QStringLiteral("SP_FileDialogStart")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileDialogStart);
                } else if (iconSource == QStringLiteral("SP_FileDialogEnd")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileDialogEnd);
                } else if (iconSource == QStringLiteral("SP_FileDialogToParent")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileDialogToParent);
                } else if (iconSource == QStringLiteral("SP_FileDialogNewFolder")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileDialogNewFolder);
                } else if (iconSource == QStringLiteral("SP_FileDialogDetailedView")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileDialogDetailedView);
                } else if (iconSource == QStringLiteral("SP_FileDialogInfoView")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileDialogInfoView);
                } else if (iconSource == QStringLiteral("SP_FileDialogContentsView")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileDialogContentsView);
                } else if (iconSource == QStringLiteral("SP_FileDialogListView")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileDialogListView);
                } else if (iconSource == QStringLiteral("SP_FileDialogBack")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_FileDialogBack);
                } else if (iconSource == QStringLiteral("SP_DockWidgetCloseButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DockWidgetCloseButton);
                } else if (iconSource == QStringLiteral("SP_ToolBarHorizontalExtensionButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton);
                } else if (iconSource == QStringLiteral("SP_ToolBarVerticalExtensionButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_ToolBarVerticalExtensionButton);
                } else if (iconSource == QStringLiteral("SP_DialogOkButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogOkButton);
                } else if (iconSource == QStringLiteral("SP_DialogCancelButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogCancelButton);
                } else if (iconSource == QStringLiteral("SP_DialogHelpButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogHelpButton);
                } else if (iconSource == QStringLiteral("SP_DialogOpenButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogOpenButton);
                } else if (iconSource == QStringLiteral("SP_DialogSaveButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogSaveButton);
                } else if (iconSource == QStringLiteral("SP_DialogCloseButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogCloseButton);
                } else if (iconSource == QStringLiteral("SP_DialogApplyButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogApplyButton);
                } else if (iconSource == QStringLiteral("SP_DialogResetButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogResetButton);
                } else if (iconSource == QStringLiteral("SP_DialogDiscardButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogDiscardButton);
                } else if (iconSource == QStringLiteral("SP_DialogYesButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogYesButton);
                } else if (iconSource == QStringLiteral("SP_DialogNoButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogNoButton);
                } else if (iconSource == QStringLiteral("SP_ArrowUp")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_ArrowUp);
                } else if (iconSource == QStringLiteral("SP_ArrowDown")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_ArrowDown);
                } else if (iconSource == QStringLiteral("SP_ArrowLeft")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_ArrowLeft);
                } else if (iconSource == QStringLiteral("SP_ArrowRight")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_ArrowRight);
                } else if (iconSource == QStringLiteral("SP_ArrowBack")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_ArrowBack);
                } else if (iconSource == QStringLiteral("SP_ArrowForward")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_ArrowForward);
                } else if (iconSource == QStringLiteral("SP_CommandLink")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_CommandLink);
                } else if (iconSource == QStringLiteral("SP_VistaShield")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_VistaShield);
                } else if (iconSource == QStringLiteral("SP_BrowserReload")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_BrowserReload);
                } else if (iconSource == QStringLiteral("SP_BrowserStop")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_BrowserStop);
                } else if (iconSource == QStringLiteral("SP_MediaPlay")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MediaPlay);
                } else if (iconSource == QStringLiteral("SP_MediaStop")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MediaStop);
                } else if (iconSource == QStringLiteral("SP_MediaPause")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MediaPause);
                } else if (iconSource == QStringLiteral("SP_MediaSkipForward")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MediaSkipForward);
                } else if (iconSource == QStringLiteral("SP_MediaSkipBackward")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MediaSkipBackward);
                } else if (iconSource == QStringLiteral("SP_MediaSeekForward")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MediaSeekForward);
                } else if (iconSource == QStringLiteral("SP_MediaSeekBackward")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MediaSeekBackward);
                } else if (iconSource == QStringLiteral("SP_MediaVolume")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MediaVolume);
                } else if (iconSource == QStringLiteral("SP_MediaVolumeMuted")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_MediaVolumeMuted);
                } else if (iconSource == QStringLiteral("SP_LineEditClearButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_LineEditClearButton);
                } else if (iconSource == QStringLiteral("SP_DialogYesToAllButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogYesToAllButton);
                } else if (iconSource == QStringLiteral("SP_DialogNoToAllButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogNoToAllButton);
                } else if (iconSource == QStringLiteral("SP_DialogSaveAllButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogSaveAllButton);
                } else if (iconSource == QStringLiteral("SP_DialogAbortButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogAbortButton);
                } else if (iconSource == QStringLiteral("SP_DialogRetryButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogRetryButton);
                } else if (iconSource == QStringLiteral("SP_DialogIgnoreButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_DialogIgnoreButton);
                } else if (iconSource == QStringLiteral("SP_RestoreDefaultsButton")) {
                    icon = s_qstyle->standardIcon(QStyle::SP_RestoreDefaultsButton);
                } else {
                    qWarning(pqcstyleC) << "unable to load icon" << iconSource << "from iconSource property";
                }
            }
            break;
        }
        default:
            break;
    }
    return icon;
}

qreal PQuickStyleItem::baselineOffsetFromRect(QRect rect, bool ceilResult) const
{
    if (rect.height() > 0) {
        const QFontMetrics& fm = m_styleoption->fontMetrics;
        int surplus = rect.height() - fm.height();
        if ((surplus & 1) && ceilResult) {
            surplus++;
        }
        int result = rect.top() + surplus / 2 + fm.ascent();
        return result;
    }
    return 0.0;
}

void PQuickStyleItem::resolvePalette()
{
    if (QCoreApplication::testAttribute(Qt::AA_SetPalette)) {
        return;
    }

    // In theory here we should consider  m_control->property("palette")
    // Whether the client code did set a custom palette color, but we can't read and write it
    // from c++ anymore as is not a QPalette but a wrapper type we don't have access to,
    // this kind of binding will need to be done on QML side
    m_styleoption->palette = getResolvedPalette();
}

int PQuickStyleItem::topPadding() const
{
    return padding(Qt::TopEdge);
}

int PQuickStyleItem::leftPadding() const
{
    return padding(Qt::LeftEdge);
}

int PQuickStyleItem::rightPadding() const
{
    return padding(Qt::RightEdge);
}

int PQuickStyleItem::bottomPadding() const
{
    return padding(Qt::BottomEdge);
}

/*
 *   Property style
 *
 *   Returns a simplified style name.
 *
 *   QMacStyle = "mac"
 *   QWindowsXPStyle = "windowsxp"
 *   QFusionStyle = "fusion"
 */

QString PQuickStyleItem::styleName() const
{
    const QString fullName = QString::fromLatin1(PQuickStyleItem::style()->metaObject()->className());
    QStringView shortName = fullName;
    if (shortName.startsWith(QLatin1Char('q'), Qt::CaseInsensitive)) {
        shortName = shortName.sliced(1);
    }
    const QLatin1String suffix("style");
    if (shortName.endsWith(suffix, Qt::CaseInsensitive)) {
        shortName.chop(suffix.length());
    }
    return shortName.toString().toLower();
}

QString PQuickStyleItem::hitTest(int px, int py)
{
    Q_UNUSED(px);
    Q_UNUSED(py);
    return QStringLiteral("none");
}

QRect PQuickStyleItem::computeBoundingRect(const QList<QRect>& rects)
{
    const auto region = std::accumulate(rects.begin(), rects.end(), QRegion());
    return region.boundingRect();
}

int PQuickStyleItem::handleWidth() const
{
    // TODO: emulate QSplitter::handleWidth using contentWidth or a custom property?
    return style()->pixelMetric(QStyle::PM_SplitterWidth, nullptr);
}

void PQuickStyleItem::updateBaselineOffset()
{
    const qreal baseline = baselineOffset();
    if (baseline > 0) {
        setBaselineOffset(baseline);
    }
}

void PQuickStyleItem::setContentWidth(int arg)
{
    if (m_contentWidth != arg) {
        m_contentWidth = arg;
        updateSizeHint();
        Q_EMIT contentWidthChanged(arg);
    }
}

void PQuickStyleItem::setContentHeight(int arg)
{
    if (m_contentHeight != arg) {
        m_contentHeight = arg;
        updateSizeHint();
        updateBaselineOffset();
        Q_EMIT contentHeightChanged(arg);
    }
}

void PQuickStyleItem::updateSizeHint()
{
    QSize implicitSize = sizeFromContents(m_contentWidth, m_contentHeight);
    setImplicitSize(implicitSize.width(), implicitSize.height());
}

void PQuickStyleItem::updateRect()
{
    initStyleOption();
    m_styleoption->rect.setWidth(width());
    m_styleoption->rect.setHeight(height());
}

int PQuickStyleItem::pixelMetric(const QString& metric)
{
    if (metric == QLatin1String("scrollbarExtent")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_ScrollBarExtent, nullptr);
    } else if (metric == QLatin1String("defaultframewidth")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_DefaultFrameWidth, m_styleoption);
    } else if (metric == QLatin1String("taboverlap")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_TabBarTabOverlap, nullptr);
    } else if (metric == QLatin1String("tabbaseoverlap")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, m_styleoption);
    } else if (metric == QLatin1String("tabhspace")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_TabBarTabHSpace, nullptr);
    } else if (metric == QLatin1String("indicatorwidth")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_IndicatorWidth, nullptr);
    } else if (metric == QLatin1String("exclusiveindicatorwidth")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_ExclusiveIndicatorWidth, nullptr);
    } else if (metric == QLatin1String("checkboxlabelspacing")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, nullptr);
    } else if (metric == QLatin1String("radiobuttonlabelspacing")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_RadioButtonLabelSpacing, nullptr);
    } else if (metric == QLatin1String("tabvspace")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_TabBarTabVSpace, nullptr);
    } else if (metric == QLatin1String("tabbaseheight")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_TabBarBaseHeight, nullptr);
    } else if (metric == QLatin1String("tabvshift")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_TabBarTabShiftVertical, nullptr);
    } else if (metric == QLatin1String("menubarhmargin")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_MenuBarHMargin, nullptr);
    } else if (metric == QLatin1String("menubarvmargin")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_MenuBarVMargin, nullptr);
    } else if (metric == QLatin1String("menubarpanelwidth")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_MenuBarPanelWidth, nullptr);
    } else if (metric == QLatin1String("menubaritemspacing")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_MenuBarItemSpacing, nullptr);
    } else if (metric == QLatin1String("spacebelowmenubar")) {
        return PQuickStyleItem::style()->styleHint(QStyle::SH_MainWindow_SpaceBelowMenuBar, m_styleoption);
    } else if (metric == QLatin1String("menuhmargin")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_MenuHMargin, nullptr);
    } else if (metric == QLatin1String("menuvmargin")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_MenuVMargin, nullptr);
    } else if (metric == QLatin1String("menupanelwidth")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_MenuPanelWidth, nullptr);
    } else if (metric == QLatin1String("submenuoverlap")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_SubMenuOverlap, nullptr);
    } else if (metric == QLatin1String("splitterwidth")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_SplitterWidth, nullptr);
    } else if (metric == QLatin1String("scrollbarspacing")) {
        return abs(PQuickStyleItem::style()->pixelMetric(QStyle::PM_ScrollView_ScrollBarSpacing, nullptr));
    } else if (metric == QLatin1String("treeviewindentation")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_TreeViewIndentation, nullptr);
    } else if (metric == QLatin1String("layouthorizontalspacing")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing, nullptr);
    } else if (metric == QLatin1String("layoutverticalspacing")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing, nullptr);
    } else if (metric == QLatin1String("layoutleftmargin")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_LayoutLeftMargin, nullptr);
    } else if (metric == QLatin1String("layouttopmargin")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_LayoutTopMargin, nullptr);
    } else if (metric == QLatin1String("layoutrightmargin")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_LayoutRightMargin, nullptr);
    } else if (metric == QLatin1String("layoutbottommargin")) {
        return PQuickStyleItem::style()->pixelMetric(QStyle::PM_LayoutBottomMargin, nullptr);
    }
    return 0;
}

QVariant PQuickStyleItem::styleHint(const QString& metric)
{
    initStyleOption();
    if (metric == QLatin1String("comboboxpopup")) {
        return PQuickStyleItem::style()->styleHint(QStyle::SH_ComboBox_Popup, m_styleoption);
    } else if (metric == QLatin1String("highlightedTextColor")) {
        return m_styleoption->palette.highlightedText().color().name();
    } else if (metric == QLatin1String("textColor")) {
        QPalette pal = m_styleoption->palette;
        pal.setCurrentColorGroup(active() ? QPalette::Active : QPalette::Inactive);
        return pal.text().color().name();
    } else if (metric == QLatin1String("focuswidget")) {
        return PQuickStyleItem::style()->styleHint(QStyle::SH_FocusFrame_AboveWidget);
    } else if (metric == QLatin1String("tabbaralignment")) {
        int result = PQuickStyleItem::style()->styleHint(QStyle::SH_TabBar_Alignment);
        if (result == Qt::AlignCenter) {
            return QStringLiteral("center");
        }
        return QStringLiteral("left");
    } else if (metric == QLatin1String("externalScrollBars")) {
        return PQuickStyleItem::style()->styleHint(QStyle::SH_ScrollView_FrameOnlyAroundContents);
    } else if (metric == QLatin1String("scrollToClickPosition")) {
        return PQuickStyleItem::style()->styleHint(QStyle::SH_ScrollBar_LeftClickAbsolutePosition);
    } else if (metric == QLatin1String("activateItemOnSingleClick")) {
        return PQuickStyleItem::style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick);
    } else if (metric == QLatin1String("submenupopupdelay")) {
        return PQuickStyleItem::style()->styleHint(QStyle::SH_Menu_SubMenuPopupDelay, m_styleoption);
    } else if (metric == QLatin1String("wheelScrollLines")) {
        return qApp->wheelScrollLines();
    } else if (metric == QLatin1String("dialogButtonsHaveIcons")) {
        return PQuickStyleItem::style()->styleHint(QStyle::SH_DialogButtonBox_ButtonsHaveIcons);
    }
    return 0;

    // Add SH_Menu_SpaceActivatesItem
}

void PQuickStyleItem::setHints(const QVariantMap& hints)
{
    if (m_hints != hints) {
        m_hints = hints;
        initStyleOption();
        updateSizeHint();
        polish();
        if (m_styleoption->state & QStyle::State_Mini) {
            m_font.setPointSize(9.);
            Q_EMIT fontChanged();
        } else if (m_styleoption->state & QStyle::State_Small) {
            m_font.setPointSize(11.);
            Q_EMIT fontChanged();
        } else {
            Q_EMIT hintChanged();
        }
    }
}

void PQuickStyleItem::resetHints()
{
    m_hints.clear();
}

void PQuickStyleItem::setFont(const QFont& font)
{
    if (font == m_font) {
        return;
    }
    m_font = font;
    updateSizeHint();
    polish();
    Q_EMIT fontChanged();
}

void PQuickStyleItem::resetFont()
{
    setFont(qApp->font());
}

QRectF PQuickStyleItem::subControlRect(const QString& subcontrolString)
{
    Q_UNUSED(subcontrolString)
    return QRectF();
}

void PQuickStyleItem::doPaint(QPainter*) {}

QSize PQuickStyleItem::sizeFromContents(int width, int height)
{
    initStyleOption();
    return getContentSize(width, height).expandedTo(QSize(m_contentWidth, m_contentHeight));
}

QSize PQuickStyleItem::getContentSize(int, int)
{
    return {};
}

void PQuickStyleItem::paint(QPainter* painter)
{
    initStyleOption();
    if (QStyleOptionMenuItem* opt = qstyleoption_cast<QStyleOptionMenuItem*>(m_styleoption)) {
        painter->setFont(opt->font);
    } else {
        QFont font;
        if (m_styleoption->state & QStyle::State_Mini) {
            font = qApp->font("QMiniFont");
        } else if (m_styleoption->state & QStyle::State_Small) {
            font = qApp->font("QSmallFont");
        } else {
            font = QApplication::font(classNameForItem());
        }
        painter->setFont(font);
    }
    doPaint(painter);
}

qreal PQuickStyleItem::textWidth(const QString& text)
{
    QFontMetricsF fm = QFontMetricsF(m_styleoption->fontMetrics);
    return fm.boundingRect(text).width();
}

qreal PQuickStyleItem::textHeight(const QString& text)
{
    QFontMetricsF fm = QFontMetricsF(m_styleoption->fontMetrics);
    return text.isEmpty() ? fm.height() : fm.boundingRect(text).height();
}

QString PQuickStyleItem::elidedText(const QString& text, int elideMode, int width)
{
    return m_styleoption->fontMetrics.elidedText(text, Qt::TextElideMode(elideMode), width);
}

bool PQuickStyleItem::hasThemeIcon(const QString& icon) const
{
    return QIcon::hasThemeIcon(icon);
}

void PQuickStyleItem::updateFocusReason()
{
    if (m_control) {
        auto reason = m_focusReasonProperty.read(m_control).value<Qt::FocusReason>();
        if (m_focusReason != reason) {
            m_focusReason = reason;
            polish();
        }
    }
}

bool PQuickStyleItem::event(QEvent* ev)
{
    if (ev->type() == QEvent::StyleAnimationUpdate) {
        if (isVisible()) {
            ev->accept();
            polish();
        }
        return true;
    }
    return QQuickItem::event(ev);
}

void PQuickStyleItem::setTextureWidth(int w)
{
    if (m_textureWidth == w) {
        return;
    }
    m_textureWidth = w;
    Q_EMIT textureWidthChanged(m_textureWidth);
    update();
}

void PQuickStyleItem::setTextureHeight(int h)
{
    if (m_textureHeight == h) {
        return;
    }
    m_textureHeight = h;
    Q_EMIT textureHeightChanged(m_textureHeight);
    update();
}

QQuickItem* PQuickStyleItem::control() const
{
    return m_control;
}

void PQuickStyleItem::setControl(QQuickItem* control)
{
    if (control == m_control) {
        return;
    }

    if (m_control) {
        m_control->removeEventFilter(this);
        disconnect(m_control, nullptr, this, nullptr);
    }

    m_control = control;
    m_focusReasonProperty = {};
    m_focusReason = Qt::NoFocusReason;

    if (m_control) {
        // focusReasonChanged is a signal of QQuickControl class which is not exposed.
        const auto mo = m_control->metaObject();
        m_focusReasonProperty = mo->property(mo->indexOfProperty("focusReason"));
        if (m_focusReasonProperty.isValid()) {
            // wish there was something like QMetaMethod::fromSignal but for slots
            const auto slot = metaObject()->method(metaObject()->indexOfSlot("updateFocusReason()"));
            // also wish there was a way to connect QMetaMethod to arbitrary member function
            connect(m_control, m_focusReasonProperty.notifySignal(), this, slot);
            updateFocusReason();
        }

        m_control->installEventFilter(this);

        if (m_control->window()) {
            m_window = m_control->window();
            m_window->installEventFilter(this);
        }
        connect(m_control, &QQuickItem::windowChanged, this, [this](QQuickWindow* window) {
            if (m_window) {
                m_window->removeEventFilter(this);
            }
            m_window = window;
            if (m_window) {
                m_window->installEventFilter(this);
            }
        });
    }

    Q_EMIT controlChanged();
}

QSGNode* PQuickStyleItem::updatePaintNode(QSGNode* node, UpdatePaintNodeData*)
{
    if (m_image.isNull()) {
        delete node;
        return nullptr;
    }

    QSGNinePatchNode* styleNode = static_cast<QSGNinePatchNode*>(node);
    if (!styleNode) {
        styleNode = window()->createNinePatchNode();
    }

#ifdef QSG_RUNTIME_DESCRIPTION
    qsgnode_set_description(styleNode, QString::fromLatin1("%1:%2, '%3'").arg(styleName(), elementType(), text()));
#endif

    styleNode->setTexture(window()->createTextureFromImage(m_image, QQuickWindow::TextureCanUseAtlas));

    auto scale = window()->effectiveDevicePixelRatio();
    QRectF bounds = QRectF(boundingRect().topLeft(), QSizeF{ m_image.width() / scale, m_image.height() / scale });

    QPointF globalPixelPos = mapToScene(bounds.topLeft()) * scale;
    QPointF posAdjust =
        QPointF(globalPixelPos.x() - std::round(globalPixelPos.x()), globalPixelPos.y() - std::round(globalPixelPos.y())) / scale;
    bounds.moveTopLeft(bounds.topLeft() - posAdjust);

    styleNode->setBounds(bounds);
    styleNode->setDevicePixelRatio(scale);
    styleNode->setPadding(m_border.left(), m_border.top(), m_border.right(), m_border.bottom());
    styleNode->update();

    return styleNode;
}

void PQuickStyleItem::updatePolish()
{
    if (isVisible() && width() >= 1 && height() >= 1) {  // Note these are reals so 1 pixel is minimum
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 3)
        // Need to request the focus reason every time, the signal is not entirely reliable: https://bugreports.qt.io/browse/QTBUG-125725
        updateFocusReason();
#endif
        const qreal devicePixelRatio = window() ? window()->effectiveDevicePixelRatio() : qApp->devicePixelRatio();
        const QSize size =
            QSize(m_textureWidth > 0 ? m_textureWidth : width(), m_textureHeight > 0 ? m_textureHeight : height()) * devicePixelRatio;

        if (m_image.size() != size) {
            m_image = QImage(size, QImage::Format_ARGB32_Premultiplied);
        }
        m_image.setDevicePixelRatio(devicePixelRatio);
        m_image.fill(Qt::transparent);
        QPainter painter(&m_image);
        painter.setLayoutDirection(qApp->layoutDirection());
        paint(&painter);
        QQuickItem::update();
    } else if (!m_image.isNull()) {
        m_image = QImage();
        QQuickItem::update();
    }
}

bool PQuickStyleItem::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_control) {
        // Page accepts mouse events without doing anything with them (for a workaround wrt dragging from empty areas of flickables) when
        // the interaction is pure mouse, steal events from them, so a parent handler can initiate a window drag from empty areas,
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if (me->source() == Qt::MouseEventNotSynthesized && watched->inherits("QQuickPage")) {
                event->setAccepted(false);
                return true;
            }
        }
    } else if (watched == m_window.data()) {
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
            QKeyEvent* ke = static_cast<QKeyEvent*>(event);
            if (ke->key() == Qt::Key_Alt) {
                polish();
            }
        }
    } else if (watched == qGuiApp) {
        if (event->type() == QEvent::ApplicationFontChange) {
            QMetaObject::invokeMethod(this, &PQuickStyleItem::updateSizeHint, Qt::QueuedConnection);
        }
    }

    return QQuickItem::eventFilter(watched, event);
}

void PQuickStyleItem::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value)
{
    if (change == QQuickItem::ItemVisibleHasChanged || change == QQuickItem::ItemEnabledHasChanged ||
        change == QQuickItem::ItemDevicePixelRatioHasChanged) {
        polish();
    }

    QQuickItem::itemChange(change, value);
}

void PQuickStyleItem::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);

    if (!newGeometry.isEmpty() && newGeometry != oldGeometry) {
        polish();
        updateRect();

        if (newGeometry.height() != oldGeometry.height()) {
            updateBaselineOffset();
        }
    }
}

void PQuickStyleItem::styleChanged()
{
    // It should be safe to use qApp->style() unguarded here, because the signal
    // will only have been connected if qApp is a QApplication.
    Q_ASSERT(qobject_cast<QApplication*>(QCoreApplication::instance()));
    auto* style = qApp->style();
    if (!style || QCoreApplication::closingDown()) {
        return;
    }

    Q_ASSERT(style != sender());

    connect(style, &QObject::destroyed, this, &PQuickStyleItem::styleChanged);

    doResolvePalette();
    updateSizeHint();
    polish();
    Q_EMIT styleNameChanged();
}
