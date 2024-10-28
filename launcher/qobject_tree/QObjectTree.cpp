

#include "QObjectTree.h"
#include <qcontainerfwd.h>
#include <QAbstractItemModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QtQuick/QQuickItem>
#include <memory>

// ===================================================
// =                 Utility functions               =
// ===================================================

QObject* uiParent(QObject* obj)
{
    QObject* parent = obj->parent();
    if (!parent || !parent->isWindowType()) {
        if (QQuickItem* qqItem = qobject_cast<QQuickItem*>(obj)) {
            if (QQuickItem* qqParentItem = qqItem->parentItem()) {
                return qqParentItem;
            }
        }
    }
    return parent;
}

QSet<QObject*> allUniqueChildren(QObject* obj)
{
    const auto& children = obj->children();
    QSet<QObject*> set = QSet<QObject*>(children.begin(), children.end());
    if (auto qqItem = qobject_cast<QQuickItem*>(obj)) {
        const auto& childItems = qqItem->childItems();
        set.reserve(set.size() + childItems.size());
        for (auto qqChild : childItems) {
            set.insert(qqChild);
        }
    }
    return set;
}

QString normalizeTypeName(const QString& typName)
{
    auto pos = typName.indexOf("_QMLTYPE_");
    if (pos != -1) {
        return typName.mid(pos);
    }

    pos = typName.indexOf("_QML_");
    if (pos != -1) {
        return typName.mid(pos);
    }

    return typName;
}

QString typeName(QObject* obj)
{
    if (obj) {
        const QMetaObject* meta = obj->metaObject();
        return normalizeTypeName(meta->className());
    }
    return "(null)";
}

// ===================================================
// =                 Builder Impl                    =
// ===================================================

std::shared_ptr<QObjectTree> QObjectTreeBuilder::BuildFromNode(QObject* obj)
{
    m_tree = std::make_shared<QObjectTree>();

    traversePathUp(obj);

    return m_tree;
}

QObjectTreeNode* QObjectTreeBuilder::recordObject(QObject* obj, QObjectTreeNode* parentNode)
{
    auto node = std::make_shared<QObjectTreeNode>(parentNode);
    node->m_obj = obj;
    node->m_name = obj->objectName();
    node->m_typeName = typeName(obj);
    node->m_children = allUniqueChildren(obj);
    if (parentNode) {
        node->m_parent = parentNode->m_obj;
        node->m_path.append(parentNode->m_path);
    } else {
        node->m_parent = nullptr;
    }
    node->m_path.push_back(node->m_typeName);

    const QMetaObject* meta = obj->metaObject();
    for (int i = meta->propertyOffset(); i < meta->propertyCount(); ++i) {
        auto prop = meta->property(i);
        auto propNode = std::make_shared<QObjectPropertyNode>(node.get());
        propNode->m_name = prop.name();
        propNode->m_typeName = propNode->m_typeName;
        auto val = prop.read(obj);
        if (val.canConvert<QString>()) {
            propNode->m_value = val;
        } else {
            auto jsonVal = QCborValue::fromVariant(val).toJsonValue();
            if (jsonVal.isArray()) {
                propNode->m_value = QString(QJsonDocument{ jsonVal.toArray() }.toJson());
            } else if (jsonVal.isObject()) {
                propNode->m_value = QString(QJsonDocument{ jsonVal.toObject() }.toJson());
            } else  {
                propNode->m_value = jsonVal.toVariant();
            }
        }
        node->m_properties.insert(propNode->m_name, propNode);
    }

    m_tree->nodes.insert(obj, node);
    return node.get();
}

QObjectTreeNode* QObjectTreeBuilder::traversePathDown(QObject* obj, QObjectTreeNode* parentNode)
{
    const auto children = allUniqueChildren(obj);
    auto objNode = recordObject(obj, parentNode);
    for (auto child : children) {
        objNode->m_childrenInfo.push_back(traversePathDown(child, objNode));
    }
    return objNode;
}

void QObjectTreeBuilder::traversePathUp(QObject* obj)
{
    const auto parent = uiParent(obj);

    if (parent) {
        traversePathUp(parent);
    } else {
        m_tree->root = obj;
        traversePathDown(obj, nullptr);
    }
}

// ===================================================
// =            QObjectPropertyNode Impl             =
// ===================================================

QObjectTreeItem* QObjectPropertyNode::child([[maybe_unused]] int row)
{
    return nullptr;
}

int QObjectPropertyNode::childCount() const
{
    return 0;
}

int QObjectPropertyNode::columnCount() const
{
    return 3;
}

QVariant QObjectPropertyNode::data(int column) const
{
    switch (column) {
        case 0:
            return m_name;
        case 1:
            return m_typeName;
        case 2:
            return m_value;
    }
    return {};
}

int QObjectPropertyNode::row() const
{
    if (m_parentNode == nullptr) {
        return 0;
    }

    const auto it = std::find_if(m_parentNode->m_properties.cbegin(), m_parentNode->m_properties.cend(),
                                 [this](const std::shared_ptr<QObjectPropertyNode>& prop) { return prop.get() == this; });
    if (it != m_parentNode->m_properties.cend()) {
        return std::distance(m_parentNode->m_properties.cbegin(), it);
    }
    Q_ASSERT(false);  // failed to find property in it's parent? Inconceivable!
    return -1;
}

QObjectTreeItem* QObjectPropertyNode::parentItem()
{
    return m_parentNode;
}
// ===================================================
// =              QObjectTreeNode Impl               =
// ===================================================

QObjectTreeItem* QObjectTreeNode::child(int row)
{
    if (row >= 0 && row < childCount()) {
        if (row < m_properties.size()) {
            auto propKeys = m_properties.keys();
            auto key = propKeys.at(row);
            return m_properties.value(key).get();
        }
        auto childN = row - m_properties.size();
        return m_childrenInfo.at(childN);
    }
    return nullptr;
}

int QObjectTreeNode::childCount() const
{
    return m_properties.size() + m_childrenInfo.size();
}

int QObjectTreeNode::columnCount() const
{
    return 3;
}

QVariant QObjectTreeNode::data(int column) const
{
    switch (column) {
        case 0:
            return m_name;
        case 1:
            return m_typeName;
        case 2:
            return m_path.join(" > ");
    }
    return {};
}

int QObjectTreeNode::row() const
{
    if (m_parentNode == nullptr) {
        return 0;
    }

    const auto it = std::find_if(m_parentNode->m_childrenInfo.cbegin(), m_parentNode->m_childrenInfo.cend(),
                                 [this](const QObjectTreeNode* prop) { return prop == this; });
    if (it != m_parentNode->m_childrenInfo.cend()) {
        return std::distance(m_parentNode->m_childrenInfo.cbegin(), it);
    }
    Q_ASSERT(false);  // failed to find node in it's parent? Inconceivable!
    return -1;
}

QObjectTreeItem* QObjectTreeNode::parentItem()
{
    return m_parentNode;
}

// ===================================================
// =                   Model Impl                    =
// ===================================================

QObjectTreeModel::QObjectTreeModel(std::shared_ptr<QObjectTree> tree, QObject* parent) : QAbstractItemModel(parent), m_tree(tree) {}

QObjectTreeModel::~QObjectTreeModel() = default;

QModelIndex QObjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    QObjectTreeItem* node =
        parent.isValid() ? static_cast<QObjectTreeItem*>(parent.internalPointer()) : m_tree->nodes.value(m_tree->root).get();

    if (auto* child = node->child(row)) {
        return createIndex(row, column, child);
    }
    return {};
}

QModelIndex QObjectTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return {};
    }

    if (!index.internalPointer()) {
        return {};
    }

    auto child = static_cast<QObjectTreeItem*>(index.internalPointer());
    auto parent = child->parentItem();
    if (!parent) {
        return QModelIndex{};
    }

    return parent != m_tree->nodes.value(m_tree->root).get() ? createIndex(parent->row(), 0, parent) : QModelIndex{};
}

int QObjectTreeModel::rowCount(const QModelIndex& index) const
{
    if (index.column() > 0) {
        return 0;
    }
    const auto parent =
        index.isValid() ? static_cast<const QObjectTreeItem*>(index.internalPointer()) : m_tree->nodes.value(m_tree->root).get();

    return parent->childCount();
}

int QObjectTreeModel::columnCount(const QModelIndex& index) const
{
    if (index.isValid()) {
        return static_cast<QObjectTreeItem*>(index.internalPointer())->columnCount();
    }
    return m_tree->nodes.value(m_tree->root)->columnCount();
}

QVariant QObjectTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }

    const auto* item = static_cast<const QObjectTreeItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags QObjectTreeModel::flags(const QModelIndex& index) const
{
    return index.isValid() ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}

QVariant QObjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return QObject::tr("Name");
            case 1:
                return QObject::tr("Type");
            case 2:
                return QObject::tr("Path / Value");
        }
    }
    return {};
}
