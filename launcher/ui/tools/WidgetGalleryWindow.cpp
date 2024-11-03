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

#include "WidgetGalleryWindow.h"
#include <qquickwidget.h>

#include <QHBoxLayout>

#include "Application.h"
#include "ui/tools/WidgetGalleryWidgetsPane.h"

WidgetGalleryWindow::WidgetGalleryWindow(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    // auto icon = APPLICATION->icons()->getIcon("WidgetGallery");
    QString windowTitle = tr("Widget Gallery ");

    // Set window properties
    {
        // setWindowIcon(icon);
        setWindowTitle(windowTitle);
    }

    {
        auto hLayout = new QHBoxLayout(this);
        hLayout->setObjectName(QStringLiteral("hLayout"));

        auto widgetsPane = new WidgetGalleryWidgetsPane(this);
        hLayout->addWidget(widgetsPane);
        auto qmlWidgetsPane = APPLICATION->newQmlWidget(QUrl("qrc:/qt/qml/org/prismlauncher/ui/QmlWidgetGalleryPane.qml"));
        qmlWidgetsPane->setResizeMode(QQuickWidget::SizeRootObjectToView);
        // qmlWidgetsPane->show();
        hLayout->addWidget(qmlWidgetsPane);

        // const QUrl objTreeUrl("qrc:/qt/qml/PrismLauncher/ObjectTree.qml");

        // print out all qrc resource
        // qDebug() << "qrc:/ resources";
        // QDirIterator it(":", QDirIterator::Subdirectories);
        // while (it.hasNext()) {
        //     qDebug() << "RESOURCE qrc" << it.next();
        // }
    }
    show();
}

void WidgetGalleryWindow::closeEvent(QCloseEvent* event)
{
    emit isClosing();
    event->accept();
}
