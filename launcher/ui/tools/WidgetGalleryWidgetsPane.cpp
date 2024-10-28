// SPDX-License-Identifier: GPL-3.0-only
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

#include "ui/tools/WidgetGalleryWidgetsPane.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCommandLinkButton>
#include <QDateTimeEdit>
#include <QDial>
#include <QDialogButtonBox>
#include <QFileSystemModel>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QShortcut>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QTimer>
#include <QToolBox>
#include <QToolButton>
#include <QTreeView>

#include "models/QtResourcesModel.h"

static inline QString className(const QObject* o)
{
    return QString::fromUtf8(o->metaObject()->className());
}
static inline void setClassNameToolTip(QWidget* w)
{
    w->setToolTip(className(w));
}

template <class Widget>
Widget* createWidget(const char* name, QWidget* parent = nullptr)
{
    auto result = new Widget(parent);
    result->setObjectName(QLatin1String(name));
    setClassNameToolTip(result);
    return result;
}

template <class Widget, class Parameter>
Widget* createWidget1(const Parameter& p1, const char* name, QWidget* parent = nullptr)
{
    auto result = new Widget(p1, parent);
    result->setObjectName(QLatin1String(name));
    setClassNameToolTip(result);
    return result;
}

WidgetGalleryWidgetsPane::WidgetGalleryWidgetsPane(QWidget* parent) : QWidget(parent), progressBar(createProgressBar())
{
    auto disableWidgetsCheckBox = createWidget1<QCheckBox>(tr("&Disable widgets"), "disableWidgetsCheckBox");

    auto buttonsGroupBox = createButtonsGroupBox();
    auto itemViewTabWidget = createItemViewTabWidget();
    auto simpleInputWidgetsGroupBox = createSimpleInputWidgetsGroupBox();
    auto textToolBox = createTextToolBox();

    connect(disableWidgetsCheckBox, &QCheckBox::toggled, buttonsGroupBox, &QWidget::setDisabled);
    connect(disableWidgetsCheckBox, &QCheckBox::toggled, textToolBox, &QWidget::setDisabled);
    connect(disableWidgetsCheckBox, &QCheckBox::toggled, itemViewTabWidget, &QWidget::setDisabled);
    connect(disableWidgetsCheckBox, &QCheckBox::toggled, simpleInputWidgetsGroupBox, &QWidget::setDisabled);

    auto topLayout = new QHBoxLayout;
    topLayout->addStretch(1);
    topLayout->addWidget(disableWidgetsCheckBox);

    auto mainLayout = new QGridLayout(this);
    mainLayout->addLayout(topLayout, 0, 0, 1, 2);
    mainLayout->addWidget(buttonsGroupBox, 1, 0);
    mainLayout->addWidget(simpleInputWidgetsGroupBox, 1, 1);
    mainLayout->addWidget(itemViewTabWidget, 2, 0);
    mainLayout->addWidget(textToolBox, 2, 1);
    mainLayout->addWidget(progressBar, 3, 0, 1, 2);

    show();
}

QGroupBox* WidgetGalleryWidgetsPane::createButtonsGroupBox()
{
    auto result = createWidget1<QGroupBox>(tr("Buttons"), "buttonsGroupBox");

    auto defaultPushButton = createWidget1<QPushButton>(tr("Default Push Button"), "defaultPushButton");
    defaultPushButton->setDefault(true);

    auto togglePushButton = createWidget1<QPushButton>(tr("Toggle Push Button"), "togglePushButton");
    togglePushButton->setCheckable(true);
    togglePushButton->setChecked(true);

    auto flatPushButton = createWidget1<QPushButton>(tr("Flat Push Button"), "flatPushButton");
    flatPushButton->setFlat(true);

    auto toolButton = createWidget<QToolButton>("toolButton");
    toolButton->setText(tr("Tool Button"));

    auto menuToolButton = createWidget<QToolButton>("menuButton");
    menuToolButton->setText(tr("Menu Button"));
    auto toolMenu = new QMenu(menuToolButton);
    menuToolButton->setPopupMode(QToolButton::InstantPopup);
    toolMenu->addAction("Option");
    toolMenu->addSeparator();
    auto action = toolMenu->addAction("Checkable Option");
    action->setCheckable(true);
    menuToolButton->setMenu(toolMenu);
    auto toolLayout = new QHBoxLayout;
    toolLayout->addWidget(toolButton);
    toolLayout->addWidget(menuToolButton);

    auto buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(defaultPushButton);
    buttonLayout->addWidget(togglePushButton);
    buttonLayout->addWidget(flatPushButton);
    buttonLayout->addLayout(toolLayout);
    buttonLayout->addStretch(1);

    auto radioButton1 = createWidget1<QRadioButton>(tr("Radio button 1"), "radioButton1");
    auto radioButton2 = createWidget1<QRadioButton>(tr("Radio button 2"), "radioButton2");
    auto radioButton3 = createWidget1<QRadioButton>(tr("Radio button 3"), "radioButton3");
    radioButton1->setChecked(true);

    auto checkBox = createWidget1<QCheckBox>(tr("Tri-state check box"), "checkBox");
    checkBox->setTristate(true);
    checkBox->setCheckState(Qt::PartiallyChecked);

    auto checkableLayout = new QVBoxLayout;
    checkableLayout->addWidget(radioButton1);
    checkableLayout->addWidget(radioButton2);
    checkableLayout->addWidget(radioButton3);
    checkableLayout->addWidget(checkBox);
    checkableLayout->addStretch(1);

    auto mainLayout = new QHBoxLayout(result);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(checkableLayout);
    mainLayout->addStretch();
    return result;
}

static QWidget* embedIntoHBoxLayout(QWidget* w, int margin = 5)
{
    auto result = new QWidget;
    auto layout = new QHBoxLayout(result);
    layout->setContentsMargins(margin, margin, margin, margin);
    layout->addWidget(w);
    return result;
}

QToolBox* WidgetGalleryWidgetsPane::createTextToolBox()
{
    auto result = createWidget<QToolBox>("toolBox");

    const QString plainText =
        tr("Twinkle, twinkle, little star,\n"
           "How I wonder what you are.\n"
           "Up above the world so high,\n"
           "Like a diamond in the sky.\n"
           "Twinkle, twinkle, little star,\n"
           "How I wonder what you are!\n");
    // Create centered/italic HTML rich text
    QString richText = QLatin1String("<html><head/><body><i>");
    for (const auto& line : QStringView{ plainText }.split(QLatin1Char('\n')))
        richText += QString::fromLatin1("<center>%1</center>").arg(line);
    richText += QLatin1String("</i></body></html>");

    auto textEdit = createWidget1<QTextEdit>(richText, "textEdit");
    auto plainTextEdit = createWidget1<QPlainTextEdit>(plainText, "plainTextEdit");

    systemInfoTextBrowser = createWidget<QTextBrowser>("systemInfoTextBrowser");

    result->addItem(embedIntoHBoxLayout(textEdit), tr("Text Edit"));
    result->addItem(embedIntoHBoxLayout(plainTextEdit), tr("Plain Text Edit"));
    result->addItem(embedIntoHBoxLayout(systemInfoTextBrowser), tr("Text Browser"));
    return result;
}

QTabWidget* WidgetGalleryWidgetsPane::createItemViewTabWidget()
{
    auto result = createWidget<QTabWidget>("bottomLeftTabWidget");
    result->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);

    auto treeView = createWidget<QTreeView>("treeView");
    auto fileSystemModel = new QFileSystemModel(treeView);
    fileSystemModel->setRootPath("");
    treeView->setModel(fileSystemModel);

    auto resourceTreeView = createWidget<QTreeView>("treeView");
    auto resourceModel = new QtResourceModel(resourceTreeView);
    resourceModel->setRootPath(":");
    resourceTreeView->setModel(resourceModel);
    resourceTreeView->expand(resourceModel->index(":"));

    auto listModel = new QStandardItemModel(0, 1, result);
    listModel->appendRow(
        new QStandardItem(QIcon(QLatin1String(":/qt-project.org/styles/commonstyle/images/diropen-128.png")), tr("Directory")));
    listModel->appendRow(
        new QStandardItem(QIcon(QLatin1String(":/qt-project.org/styles/commonstyle/images/computer-32.png")), tr("Computer")));

    auto listView = createWidget<QListView>("listView");
    listView->setModel(listModel);

    auto iconModeListView = createWidget<QListView>("iconModeListView");
    iconModeListView->setViewMode(QListView::IconMode);
    iconModeListView->setModel(listModel);

    result->addTab(embedIntoHBoxLayout(resourceTreeView), tr("&Resource Tree View"));
    result->addTab(embedIntoHBoxLayout(treeView), tr("&Tree View"));
    result->addTab(embedIntoHBoxLayout(listView), tr("&List"));
    result->addTab(embedIntoHBoxLayout(iconModeListView), tr("&Icon Mode List"));
    return result;
}

QGroupBox* WidgetGalleryWidgetsPane::createSimpleInputWidgetsGroupBox()
{
    auto result = createWidget1<QGroupBox>(tr("Simple Input Widgets"), "bottomRightGroupBox");
    result->setCheckable(true);
    result->setChecked(true);

    auto lineEdit = createWidget1<QLineEdit>("s3cRe7", "lineEdit");
    lineEdit->setClearButtonEnabled(true);
    lineEdit->setEchoMode(QLineEdit::Password);

    auto spinBox = createWidget<QSpinBox>("spinBox", result);
    spinBox->setValue(50);

    auto slider = createWidget<QSlider>("slider", result);
    slider->setOrientation(Qt::Horizontal);
    slider->setValue(40);

    auto scrollBar = createWidget<QScrollBar>("scrollBar", result);
    scrollBar->setOrientation(Qt::Horizontal);
    setClassNameToolTip(scrollBar);
    scrollBar->setValue(60);


    auto layout = new QGridLayout(result);
    layout->addWidget(lineEdit, 0, 0, 1, 2);
    layout->addWidget(spinBox, 1, 0, 1, 2);
    layout->addWidget(slider, 2, 0);
    layout->addWidget(scrollBar, 3, 0);
    layout->setRowStretch(4, 1);
    return result;
}

QProgressBar* WidgetGalleryWidgetsPane::createProgressBar()
{
    auto result = createWidget<QProgressBar>("progressBar");
    result->setRange(0, 10000);
    result->setValue(0);

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WidgetGalleryWidgetsPane::advanceProgressBar);
    timer->start(1000);
    return result;
}

void WidgetGalleryWidgetsPane::advanceProgressBar()
{
    int curVal = progressBar->value();
    int maxVal = progressBar->maximum();
    progressBar->setValue(curVal + (maxVal - curVal) / 100);
}
