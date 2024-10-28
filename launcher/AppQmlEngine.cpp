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

#include "AppQmlEngine.h"

#include <QObject>
#include <QQmlProperty>
#include <QQuickItem>

#include "LambdaHelper.h"

Q_LOGGING_CATEGORY(qmlLogCat, "qmlengine")

AppQmlEngine::AppQmlEngine(QObject* parent) : QObject(parent)
{
    m_qmlengine = new QQmlEngine(this);
    m_qmlFileSelector = new QQmlFileSelector(m_qmlengine, m_qmlengine);

    QObject::connect(m_qmlengine, &QQmlEngine::warnings, [](const QList<QQmlError>& warnings) {
        for (const QQmlError& error : warnings) {
            qDebug(qmlLogCat) << "QML Engine Error:" << error.toString();
        }
    });
}

QQmlComponent* AppQmlEngine::load(const QUrl& url)
{
    return new QQmlComponent(m_qmlengine, url);
}

QQuickWidget* AppQmlEngine::newWidget(const QUrl& url, QWidget* parent)
{
    QPointer<QQuickWidget> widget = new MinSizedQuickWidget(m_qmlengine, parent);
    QMetaObject::Connection statusConnection;
    statusConnection =
        widget->connect(widget, &QQuickWidget::statusChanged, this, [ widget, statusConnection](QQuickWidget::Status status) {
            switch (status) {
                case QQuickWidget::Loading:
                    qDebug(qmlLogCat) << "Loading QML component" << widget->source();
                    break;
                case QQuickWidget::Ready: {
                    qDebug(qmlLogCat) << "QML component Ready" << widget->source();
                    widget->disconnect(statusConnection);
                    widget->resize(widget->initialSize());
                    break;
                }
                case QQuickWidget::Error: {
                    qDebug(qmlLogCat) << "Qml Widget Error from " << widget->source();
                    for (auto error : widget->errors()) {
                        qDebug(qmlLogCat) << "Error:" << error;
                    }
                    break;
                }
                default: {
                }
            }
        });
    widget->connect(widget, &QQuickWidget::sceneGraphError, this, [widget](QQuickWindow::SceneGraphError, const QString& message) {
        qDebug(qmlLogCat) << "Qml Widget SceneGraph Error:" << widget->source() << "Error:" << message;
    });
    widget->setSource(url);
    return widget;
}

MinSizedQuickWidget::MinSizedQuickWidget(QWidget* parent) : QQuickWidget(parent)
{
    connectMinimumSizeProerties();
}
MinSizedQuickWidget::MinSizedQuickWidget(QQmlEngine* engine, QWidget* parent) : QQuickWidget(engine, parent)
{
    connectMinimumSizeProerties();
}
MinSizedQuickWidget::MinSizedQuickWidget(const QUrl& source, QWidget* parent) : QQuickWidget(source, parent)
{
    connectMinimumSizeProerties();
}
void MinSizedQuickWidget::connectMinimumSizeProerties()
{
    connect(this, &QQuickWidget::statusChanged, this, [this](QQuickWidget::Status status) {
        if (status == QQuickWidget::Ready) {
            QQuickItem* root = rootObject();
            QQmlProperty minWidth(root, "minimumWidth");
            if (minWidth.type() != QQmlProperty::Invalid) {
                minWidth.connectNotifySignal(this, SLOT(minimumWidthChanged));
            }
            QQmlProperty minHeight(root, "minimumheight");
            if (minHeight.type() != QQmlProperty::Invalid) {
                minHeight.connectNotifySignal(this, SLOT(minimumHeightChanged));
            }
        }
    });
}

void MinSizedQuickWidget::minimumWidthChanged()
{
    QQmlProperty minWidth(rootObject(), "minimumWidth");
    if (minWidth.type() != QQmlProperty::Invalid) {
        setMinimumWidth(minWidth.read().toInt());
    }
}

void MinSizedQuickWidget::minimumHeightChanged()
{
    QQmlProperty minHeight(rootObject(), "minimumheight");
    if (minHeight.type() != QQmlProperty::Invalid) {
        setMinimumHeight(minHeight.read().toInt());
    }
}
