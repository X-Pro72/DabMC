// SPDX-License-Identifier: GPL-3.0-only
/*
 *  PolyMC - Minecraft Launcher
 *  Copyright (c) 2022 flowln <flowlnlnln@gmail.com>
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
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *      Copyright 2021-2022 kb1000
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "ModrinthPage.h"
#include "ui_ModrinthPage.h"

#include "ModrinthModel.h"

#include "BuildConfig.h"
#include "InstanceImportTask.h"
#include "Json.h"

#include "ui/widgets/ProjectItem.h"

#include <HoeDown.h>

#include <QComboBox>
#include <QKeyEvent>
#include <QPushButton>

ModrinthPage::ModrinthPage(NewInstanceDialog* dialog, QWidget* parent) : QWidget(parent), ui(new Ui::ModrinthPage), dialog(dialog)
{
    ui->setupUi(this);

    connect(ui->searchButton, &QPushButton::clicked, this, &ModrinthPage::triggerSearch);
    ui->searchEdit->installEventFilter(this);
    m_model = new Modrinth::ModpackListModel(this);
    ui->packView->setModel(m_model);

    ui->versionSelectionBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->versionSelectionBox->view()->parentWidget()->setMaximumHeight(300);

    ui->sortByBox->addItem(tr("Sort by Relevance"));
    ui->sortByBox->addItem(tr("Sort by Total Downloads"));
    ui->sortByBox->addItem(tr("Sort by Follows"));
    ui->sortByBox->addItem(tr("Sort by Newest"));
    ui->sortByBox->addItem(tr("Sort by Last Updated"));

    connect(ui->sortByBox, SIGNAL(currentIndexChanged(int)), this, SLOT(triggerSearch()));
    connect(ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &ModrinthPage::onSelectionChanged);
    connect(ui->versionSelectionBox, &QComboBox::currentTextChanged, this, &ModrinthPage::onVersionSelectionChanged);

    ui->packView->setItemDelegate(new ProjectItemDelegate(this));
    ui->packDescription->setMetaEntry(metaEntryBase());
}

ModrinthPage::~ModrinthPage()
{
    delete ui;
}

void ModrinthPage::retranslate()
{
    ui->retranslateUi(this);
}

void ModrinthPage::openedImpl()
{
    BasePage::openedImpl();
    triggerSearch();
}

bool ModrinthPage::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->searchEdit && event->type() == QEvent::KeyPress) {
        auto* keyEvent = reinterpret_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return) {
            this->triggerSearch();
            keyEvent->accept();
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

void ModrinthPage::onSelectionChanged(QModelIndex curr, QModelIndex prev)
{
    ui->versionSelectionBox->clear();

    if (!curr.isValid()) {
        if (isOpened) {
            dialog->setSuggestedPack();
        }
        return;
    }

    current = m_model->data(curr, Qt::UserRole).value<Modrinth::Modpack>();
    auto name = current.name;

    if (!current.extraInfoLoaded) {
        qCDebug(LAUNCHER_LOG) << "Loading modrinth modpack information";

        auto netJob = new NetJob(QString("Modrinth::PackInformation(%1)").arg(current.name), APPLICATION->network());
        auto response = new QByteArray();

        QString id = current.id;

        netJob->addNetAction(Net::Download::makeByteArray(QString("%1/project/%2").arg(BuildConfig.MODRINTH_PROD_URL, id), response));

        QObject::connect(netJob, &NetJob::succeeded, this, [this, response, id, curr] {
            if (id != current.id) {
                return;  // wrong request?
            }

            QJsonParseError parse_error;
            QJsonDocument doc = QJsonDocument::fromJson(*response, &parse_error);
            if (parse_error.error != QJsonParseError::NoError) {
                qCWarning(LAUNCHER_LOG) << "Error while parsing JSON response from Modrinth at " << parse_error.offset
                           << " reason: " << parse_error.errorString();
                qCWarning(LAUNCHER_LOG) << *response;
                return;
            }

            auto obj = Json::requireObject(doc);

            try {
                Modrinth::loadIndexedInfo(current, obj);
            } catch (const JSONValidationError& e) {
                qCDebug(LAUNCHER_LOG) << *response;
                qCWarning(LAUNCHER_LOG) << "Error while reading modrinth modpack version: " << e.cause();
            }

            updateUI();

            QVariant current_updated;
            current_updated.setValue(current);

            if (!m_model->setData(curr, current_updated, Qt::UserRole))
                qCWarning(LAUNCHER_LOG) << "Failed to cache extra info for the current pack!";

            suggestCurrent();
        });
        QObject::connect(netJob, &NetJob::finished, this, [response, netJob] {
            netJob->deleteLater();
            delete response;
        });
        netJob->start();
    } else
        updateUI();

    if (!current.versionsLoaded) {
        qCDebug(LAUNCHER_LOG) << "Loading modrinth modpack versions";

        auto netJob = new NetJob(QString("Modrinth::PackVersions(%1)").arg(current.name), APPLICATION->network());
        auto response = new QByteArray();

        QString id = current.id;

        netJob->addNetAction(
            Net::Download::makeByteArray(QString("%1/project/%2/version").arg(BuildConfig.MODRINTH_PROD_URL, id), response));

        QObject::connect(netJob, &NetJob::succeeded, this, [this, response, id, curr] {
            if (id != current.id) {
                return;  // wrong request?
            }

            QJsonParseError parse_error;
            QJsonDocument doc = QJsonDocument::fromJson(*response, &parse_error);
            if (parse_error.error != QJsonParseError::NoError) {
                qCWarning(LAUNCHER_LOG) << "Error while parsing JSON response from Modrinth at " << parse_error.offset
                           << " reason: " << parse_error.errorString();
                qCWarning(LAUNCHER_LOG) << *response;
                return;
            }

            try {
                Modrinth::loadIndexedVersions(current, doc);
            } catch (const JSONValidationError& e) {
                qCDebug(LAUNCHER_LOG) << *response;
                qCWarning(LAUNCHER_LOG) << "Error while reading modrinth modpack version: " << e.cause();
            }

            for (auto version : current.versions) {
                if (!version.name.contains(version.version))
                    ui->versionSelectionBox->addItem(QString("%1 — %2").arg(version.name, version.version), QVariant(version.id));
                else
                    ui->versionSelectionBox->addItem(version.name, QVariant(version.id));
            }

            QVariant current_updated;
            current_updated.setValue(current);

            if (!m_model->setData(curr, current_updated, Qt::UserRole))
                qCWarning(LAUNCHER_LOG) << "Failed to cache versions for the current pack!";

            suggestCurrent();
        });
        QObject::connect(netJob, &NetJob::finished, this, [response, netJob] {
            netJob->deleteLater();
            delete response;
        });
        netJob->start();

    } else {
        for (auto version : current.versions) {
            if (!version.name.contains(version.version))
                ui->versionSelectionBox->addItem(QString("%1 - %2").arg(version.name, version.version), QVariant(version.id));
            else
                ui->versionSelectionBox->addItem(version.name, QVariant(version.id));
        }

        suggestCurrent();
    }
}

void ModrinthPage::updateUI()
{
    QString text = "";

    if (current.extra.projectUrl.isEmpty())
        text = current.name;
    else
        text = "<a href=\"" + current.extra.projectUrl + "\">" + current.name + "</a>";

    // TODO: Implement multiple authors with links
    text += "<br>" + tr(" by ") + QString("<a href=%1>%2</a>").arg(std::get<1>(current.author).toString(), std::get<0>(current.author));

    if (current.extraInfoLoaded) {
        if (!current.extra.donate.isEmpty()) {
            text += "<br><br>" + tr("Donate information: ");
            auto donateToStr = [](Modrinth::DonationData& donate) -> QString {
                return QString("<a href=\"%1\">%2</a>").arg(donate.url, donate.platform);
            };
            QStringList donates;
            for (auto& donate : current.extra.donate) {
                donates.append(donateToStr(donate));
            }
            text += donates.join(", ");
        }

        if (!current.extra.issuesUrl.isEmpty()
         || !current.extra.sourceUrl.isEmpty()
         || !current.extra.wikiUrl.isEmpty()
         || !current.extra.discordUrl.isEmpty()) {
            text += "<br><br>" + tr("External links:") + "<br>";
        }

        if (!current.extra.issuesUrl.isEmpty())
            text += "- " + tr("Issues: <a href=%1>%1</a>").arg(current.extra.issuesUrl) + "<br>";
        if (!current.extra.wikiUrl.isEmpty())
            text += "- " + tr("Wiki: <a href=%1>%1</a>").arg(current.extra.wikiUrl) + "<br>";
        if (!current.extra.sourceUrl.isEmpty())
            text += "- " + tr("Source code: <a href=%1>%1</a>").arg(current.extra.sourceUrl) + "<br>";
        if (!current.extra.discordUrl.isEmpty())
            text += "- " + tr("Discord: <a href=%1>%1</a>").arg(current.extra.discordUrl) + "<br>";
    }

    text += "<hr>";

    HoeDown h;
    text += h.process(current.extra.body.toUtf8());

    ui->packDescription->setHtml(text + current.description);
    ui->packDescription->flush();
}

void ModrinthPage::suggestCurrent()
{
    if (!isOpened) {
        return;
    }

    if (selectedVersion.isEmpty()) {
        dialog->setSuggestedPack();
        return;
    }

    for (auto& ver : current.versions) {
        if (ver.id == selectedVersion) {
            dialog->setSuggestedPack(current.name, ver.version, new InstanceImportTask(ver.download_url, this));
            auto iconName = current.iconName;
            m_model->getLogo(iconName, current.iconUrl.toString(),
                             [this, iconName](QString logo) { dialog->setSuggestedIconFromFile(logo, iconName); });

            break;
        }
    }
}

void ModrinthPage::triggerSearch()
{
    m_model->searchWithTerm(ui->searchEdit->text(), ui->sortByBox->currentIndex());
}

void ModrinthPage::onVersionSelectionChanged(QString data)
{
    if (data.isNull() || data.isEmpty()) {
        selectedVersion = "";
        return;
    }
    selectedVersion = ui->versionSelectionBox->currentData().toString();
    suggestCurrent();
}
