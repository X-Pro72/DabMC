#pragma once

#include <QAbstractItemModel>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QString>
#include <QVariant>
#include <memory>
#include <vector>

class QObjectTreeItem {
   public:
    virtual QObjectTreeItem* child(int row) = 0;
    virtual int childCount() const = 0;
    virtual int columnCount() const = 0;
    virtual QVariant data(int column) const = 0;
    virtual int row() const = 0;
    virtual QObjectTreeItem* parentItem() = 0;
};

class QObjectTreeNode;

class QObjectPropertyNode : public QObjectTreeItem {
   public:
    QObjectPropertyNode(QObjectTreeNode* parentNode = nullptr) : m_parentNode(parentNode) {}

    QObjectTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    QObjectTreeItem* parentItem();

   public:
    QString m_name;
    QString m_typeName;
    QVariant m_value;

   private:
    QObjectTreeNode* m_parentNode;
};

class QObjectTreeNode : public QObjectTreeItem {
   public:
    QObjectTreeNode(QObjectTreeNode* parentNode = nullptr) : m_parentNode(parentNode) {}

    QObjectTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    QObjectTreeItem* parentItem();

   public:
    QObject* m_parent;
    QObject* m_obj;
    QString m_name;
    QString m_typeName;
    QStringList m_path;
    QSet<QObject*> m_children;
    QHash<QString, std::shared_ptr<QObjectPropertyNode>> m_properties;
    std::vector<QObjectTreeNode*> m_childrenInfo;

   private:
    QObjectTreeNode* m_parentNode;
};

struct QObjectTree {
    QObject* root;
    QHash<QObject*, std::shared_ptr<QObjectTreeNode>> nodes;
};

class QObjectTreeBuilder {
   public:
    QObjectTreeBuilder() = default;
    std::shared_ptr<QObjectTree> BuildFromNode(QObject* obj);

   private:
    void traversePathUp(QObject* obj);
    QObjectTreeNode* recordObject(QObject* obj, QObjectTreeNode* parentNode);
    QObjectTreeNode* traversePathDown(QObject* obj, QObjectTreeNode* parentNode);

   private:
    std::shared_ptr<QObjectTree> m_tree = nullptr;
};

class QObjectTreeModel : public QAbstractItemModel {
    Q_OBJECT
   public:
    Q_DISABLE_COPY_MOVE(QObjectTreeModel)

    explicit QObjectTreeModel(std::shared_ptr<QObjectTree> tree, QObject* parent = nullptr);
    ~QObjectTreeModel() override;
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& index = {}) const override;
    int columnCount(const QModelIndex& index = {}) const override;

   private:
    std::shared_ptr<QObjectTree> m_tree;
};
