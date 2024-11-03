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

#include "QtResourcesModel.h"
#include <QDirIterator>

QtResourceModel::QtResourceModel(QObject* parent) : QAbstractItemModel(parent) {}

QModelIndex QtResourceModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent)) {
        return QModelIndex();
    }

    QtResourceNode* parentNode = indexValid(parent) ? node(parent) : const_cast<QtResourceNode*>(&m_root);

    Q_ASSERT(parentNode);
    const int i = translateVisibleLocation(parentNode, row);
    if (i >= parentNode->children.size()) {
        return QModelIndex();
    }
    const QString childName = parentNode->children.keys().at(i);
    const QtResourceNode* indexNode = parentNode->children.value(childName);

    Q_ASSERT(indexNode);

    return createIndex(row, column, indexNode);
}

QModelIndex QtResourceModel::index(const QtResourceNode* node, int column) const
{
    QtResourceNode* parentNode = (node ? node->parent : nullptr);
    if (node == &m_root || !parentNode)
        return QModelIndex();
    Q_ASSERT(node);

    int visualRow = translateVisibleLocation(parentNode, parentNode->visibleLocation(node->fileName));
    return createIndex(visualRow, column, const_cast<QtResourceNode*>(node));
}

QModelIndex QtResourceModel::sibling(int row, int column, const QModelIndex& idx) const
{
    if (row == idx.row() && column < columnCount(idx.parent())) {
        return createIndex(row, column, idx.internalPointer());
    } else {
        return QAbstractItemModel::sibling(row, column, idx);
    }
}

QtResourceNode* QtResourceModel::node(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return const_cast<QtResourceNode*>(&m_root);
    }
    QtResourceNode* indexNode = static_cast<QtResourceNode*>(index.internalPointer());
    Q_ASSERT(indexNode);
    return indexNode;
}

QModelIndex QtResourceModel::parent(const QModelIndex& index) const
{
    if (!indexValid(index)) {
        return QModelIndex();
    }

    QtResourceNode* indexNode = node(index);
    Q_ASSERT(indexNode != nullptr);
    QtResourceNode* parentNode = indexNode->parent;
    if (parentNode == nullptr || parentNode == &m_root) {
        return QModelIndex();
    }

    QtResourceNode* grandParentNode = parentNode->parent;
    Q_ASSERT(grandParentNode->children.contains(parentNode->fileName));
    int visibleRow = translateVisibleLocation(
        grandParentNode, grandParentNode->visibleLocation(grandParentNode->children.value(parentNode->fileName)->fileName));
    if (visibleRow == -1) {
        return QModelIndex();
    }
    return createIndex(visibleRow, 0, parentNode);
}

bool QtResourceModel::hasChildren(const QModelIndex& parent) const
{
    if (parent.column() > 0) {
        return false;
    }
    if (!parent.isValid()) {
        return true;
    }

    const QtResourceNode* indexnode = node(parent);
    Q_ASSERT(indexnode);
    return indexnode->children.size() > 0;
}

int QtResourceModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        return m_root.children.size();
    }

    const QtResourceNode* parentNode = node(parent);
    return parentNode->children.size();
}

int QtResourceModel::columnCount(const QModelIndex& parent) const
{
    return (parent.column() > 0) ? 0 : NumColumns;
}

QVariant QtResourceModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    auto column = index.column();
    switch (role) {
        case Qt::DisplayRole:
            if (column == NameColumn) {
                return this->name(index);
            } else if (column == SizeColumn) {
                return this->size(index);
            }
            break;
        case FilePathRole:
            return filePath(index);
        case FileNameRole:
            return name(index);
        case Qt::DecorationRole:
            // TODO: actually report icons
            return QIcon();
        case Qt::TextAlignmentRole:
            if (index.column() == SizeColumn) {
                return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
            }
            break;
    }
    return QVariant();
}

QVariant QtResourceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
        case Qt::DecorationRole:
            if (section == 0) {
                QImage pixmap(16, 1, QImage::Format_ARGB32_Premultiplied);
                pixmap.fill(Qt::transparent);
                return pixmap;
            }
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignLeft;
    }

    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);

    switch (section) {
        case NameColumn:
            return tr("Name");
        case SizeColumn:
            return tr("Size");
    }
    return QVariant();
}

Qt::ItemFlags QtResourceModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (!index.isValid()) {
        return flags;
    }

    QtResourceNode* indexNode = node(index);
    // TODO Extra things?
    return flags;
}

QStringList QtResourceModel::mimeTypes() const
{
    return QStringList("text/uri-list");
}

QHash<int, QByteArray> QtResourceModel::roleNames() const
{
    auto ret = QAbstractItemModel::roleNames();
    ret.insert(FileIconRole,
               QByteArrayLiteral("fileIcon"));  // == Qt::decoration
    ret.insert(FilePathRole, QByteArrayLiteral("filePath"));
    ret.insert(FileNameRole, QByteArrayLiteral("fileName"));
    ret.insert(FilePermissions, QByteArrayLiteral("filePermissions"));
    ret.insert(FileInfoRole, QByteArrayLiteral("fileInfo"));
    return ret;
}

QString QtResourceModel::filePath(const QModelIndex& index) const
{
    QStringList path;
    QModelIndex idx = index;
    while (idx.isValid()) {
        QtResourceNode* dirNode = node(idx);
        if (dirNode) {
            path.prepend(dirNode->fileName);
        }
        idx = idx.parent();
    }
    QString fullPath = path.join(u'/');
    if (fullPath.at(0) != u':') {
        fullPath.prepend(u':');
    }
    return fullPath;
}

QUrl QtResourceModel::fileUrl(const QModelIndex& index) const
{
    return QUrl(filePath(index));
}

QString QtResourceModel::name(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QString();
    }
    QtResourceNode* indexNode = node(index);
    return indexNode->fileName;
}

qint64 QtResourceModel::size(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return 0;
    }
    return node(index)->size();
}

QString QtResourceModel::rootPath() const
{
    return m_rootDir.path();
}

QDir QtResourceModel::rootDirectory() const
{
    return QDir(m_rootDir);
}

QModelIndex QtResourceModel::setRootPath(const QString& path)
{
    QString newPath = path;

    if (newPath.isEmpty() || newPath.at(0) != u':') {
        newPath.prepend(u':');
    }

    auto newPathInfo = QFileInfo(newPath);

    m_rootDir = QDir(newPath);
    QModelIndex newRootIndex = index(m_rootDir.path());

    QDirIterator it(m_rootDir.path(), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        index(it.next());
    }

    return newRootIndex;
}

QtResourceNode* QtResourceModel::node(const QString& path) const
{
    if (path.isEmpty() || !path.startsWith(u':'))
        return const_cast<QtResourceNode*>(&m_root);
    QStringList pathElements = path.split(u'/', Qt::SkipEmptyParts);

    if (pathElements.isEmpty()) {
        return const_cast<QtResourceNode*>(&m_root);
    }
    QModelIndex index = QModelIndex();  // start with "My Computer"
    QString elementPath;
    QtResourceNode* parent = node(index);
    for (int i = 0; i < pathElements.size(); ++i) {
        QString element = pathElements.at(i);
        if (i != 0)
            elementPath.append(u'/');
        elementPath.append(element);
        bool alreadyExisted = parent->children.contains(element);
        if (alreadyExisted) {
            if ((parent->children.size() == 0) || (parent->children.value(element)->fileName != element)) {
                alreadyExisted = false;
            }
        }

        QtResourceNode* node;
        if (!alreadyExisted) {
            QFileInfo info(elementPath);
            if (!info.exists()) {
                return const_cast<QtResourceNode*>(&m_root);
            }
            QtResourceModel* m = const_cast<QtResourceModel*>(this);
            node = m->addNode(parent, element, info);
        } else {
            node = parent->children.value(element);
        }

        Q_ASSERT(node);
        parent = node;
    }

    return parent;
}

QtResourceNode* QtResourceModel::addNode(QtResourceNode* parentNode, const QString& fileName, const QFileInfo& info)
{
    QtResourceNode* node = new QtResourceNode(fileName, parentNode);
    node->populate(info);
    Q_ASSERT(!parentNode->children.contains(fileName));
    parentNode->children.insert(fileName, node);
    return node;
}
