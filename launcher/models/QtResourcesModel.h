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
#include <qurl.h>
#include <QAbstractItemModel>
#include <QDir>
#include <QIcon>
#include <QPair>

class QtResourceNode {
   public:
    explicit QtResourceNode(const QString& filename = QString(), QtResourceNode* p = nullptr) : fileName(filename), parent(p) {}
    ~QtResourceNode()
    {
        qDeleteAll(children);
        if (info)
            delete info;
    }
    QString fileName;

    void populate(const QFileInfo& fileInfo)
    {
        if (!info)
            info = new QFileInfo(fileInfo);
        (*info) = fileInfo;
    }

    inline qint64 size() const
    {
        if (info && !info->isDir())
            return info->size();
        return 0;
    }
    inline bool isDir() const
    {
        if (info)
            return info->isDir();
        if (children.size() > 0)
            return true;
        return false;
    }
    inline QFileInfo fileInfo() const
    {
        if (info)
            return *info;
        return QFileInfo();
    }
    inline bool isFile() const
    {
        if (info)
            return info->isFile();
        return true;
    }
    inline int visibleLocation(const QString& childName) { return children.keys().indexOf(childName); }

    inline bool hasInformation() const { return info != nullptr; }

    inline bool operator<(const QtResourceNode& node) const { return fileName < node.fileName; }

    inline bool operator>(const QString& name) const { return fileName > name; }
    inline bool operator<(const QString& name) const { return fileName < name; }
    inline bool operator!=(const QString& name) const { return fileName != name; }
    inline bool operator==(const QString& name) const { return fileName == name; }

    QHash<QString, QtResourceNode*> children;
    QFileInfo* info = nullptr;
    QtResourceNode* parent;
};

class QtResourceModel : public QAbstractItemModel {
    Q_OBJECT

   public:
    enum Roles {
        FileIconRole = Qt::DecorationRole,
        FileInfoRole = Qt::UserRole + 1,
        FilePathRole = Qt::UserRole + 2,
        FileNameRole = Qt::UserRole + 3,
        FilePermissions = Qt::UserRole + 3,
    };
    enum { NameColumn, SizeColumn, NumColumns = 2 };
    ;

    explicit QtResourceModel(QObject* parent = nullptr);
    ~QtResourceModel() = default;

    QtResourceNode* node(const QModelIndex& index) const;
    QtResourceNode* node(const QString& path) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex index(const QtResourceNode* node, int column = 0) const;
    inline QModelIndex index(const QString& path, int column = 0) { return index(node(path), column); }
    QModelIndex parent(const QModelIndex& child) const override;

    QModelIndex sibling(int row, int column, const QModelIndex& idx) const override;
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    QStringList mimeTypes() const override;
    QHash<int, QByteArray> roleNames() const override;

    // model specific
    QModelIndex setRootPath(const QString& path);
    QString rootPath() const;
    QDir rootDirectory() const;

    QUrl fileUrl(const QModelIndex& index) const;
    QString filePath(const QModelIndex& index) const;

    QString name(const QModelIndex& index) const;
    qint64 size(const QModelIndex& index) const;

    inline int translateVisibleLocation(QtResourceNode* parent, int row) const
    {
        if (sortOrder != Qt::AscendingOrder) {
            return parent->children.size() - row - 1;
        }
        return row;
    }

    QtResourceNode m_root;
    int sortColumn = 0;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;

   private:
    inline bool indexValid(const QModelIndex& index) const { return (index.row() >= 0 && index.column() >= 0 && index.model() == this); }
    QtResourceNode* addNode(QtResourceNode* parentNode, const QString& fileName, const QFileInfo& info);
    QDir m_rootDir;
};
