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

#pragma once

#include <qtmetamacros.h>
#include <QObject>
#include <QUrl>
#include <QWidget>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlFileSelector>
#include <QtQuickWidgets/QQuickWidget>

#include <QLoggingCategory>
#include <QtLogging>

Q_DECLARE_LOGGING_CATEGORY(qmlLogCat)

class AppQmlEngine : QObject {
    Q_OBJECT
   public:
    AppQmlEngine(QObject* parent);
    ~AppQmlEngine() = default;

    QQmlComponent* load(const QUrl& url);
    QQuickWidget* newWidget(const QUrl& url, QWidget* parent = nullptr);

   private:
    QQmlEngine* m_qmlengine = nullptr;
    QQmlFileSelector* m_qmlFileSelector = nullptr;
};

class MinSizedQuickWidget : public QQuickWidget {
    Q_OBJECT
   public:
    MinSizedQuickWidget(QWidget* parent = nullptr);
    MinSizedQuickWidget(QQmlEngine* engine, QWidget* parent);
    MinSizedQuickWidget(const QUrl& source, QWidget* parent = nullptr);

   public slots:
    void minimumWidthChanged();
    void minimumHeightChanged();
   private:
    void connectMinimumSizeProerties();
};
