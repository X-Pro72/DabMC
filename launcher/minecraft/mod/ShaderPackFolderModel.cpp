#include "ShaderPackFolderModel.h"
#include "FileSystem.h"
#include "Version.h"

namespace {
class ShaderPackIndexMigrateTask : public Task {
    Q_OBJECT
   public:
    // Migrates metadata from sourceDir to destDir
    ShaderPackIndexMigrateTask(QDir destDir, QDir sourceDir) : m_destDir(std::move(destDir)), m_sourceDir(std::move(sourceDir)) {}

    void executeTask() override
    {
        if (!m_sourceDir.exists()) {
            qDebug() << m_sourceDir.absolutePath() << "does not exist; nothing to migrate";
            emitSucceeded();
            return;
        }

        // Ensure destination directory exists
        if (!FS::ensureFolderPathExists(m_destDir.absolutePath())) {
            emitFailed(tr("Failed to create index directory at %1").arg(m_destDir.absolutePath()));
            return;
        }

        QStringList pwFiles = m_sourceDir.entryList({ "*.pw.toml" }, QDir::Files);
        if (pwFiles.isEmpty()) {
            emitSucceeded();
            return;
        }

        bool movedAll = true;

        for (const auto& file : pwFiles) {
            QString src = m_sourceDir.filePath(file);
            QString dest = m_destDir.filePath(file);

            if (FS::move(src, dest)) {
                qDebug() << "Moved metadata" << src << "to" << dest;
            } else {
                qWarning() << "Failed to move metadata" << src << "to" << dest;
                movedAll = false;
            }
        }

        if (!movedAll) {
            emitFailed(tr("Failed to migrate some shaderpack metadata files"));
            return;
        }

        emitSucceeded();
    }

   private:
    QDir m_destDir, m_sourceDir;
};
}  // namespace

ShaderPackFolderModel::ShaderPackFolderModel(const QDir& dir, BaseInstance* instance, bool is_indexed, bool create_dir, QObject* parent)
    : ResourceFolderModel(dir, instance, is_indexed, create_dir, parent)
{
    m_column_names = QStringList({ "Enable", "Name", "Version", "Last Modified", "Provider", "Size" });
    m_column_names_translated = QStringList({ tr("Enable"), tr("Name"), tr("Version"), tr("Last Modified"), tr("Provider"), tr("Size") });
    m_column_sort_keys = { SortType::ENABLED, SortType::NAME, SortType::VERSION, SortType::DATE, SortType::PROVIDER, SortType::SIZE };
    m_column_resize_modes = { QHeaderView::Interactive, QHeaderView::Stretch, QHeaderView::Interactive,
                              QHeaderView::Interactive, QHeaderView::Interactive, QHeaderView::Interactive };
    m_columnsHideable = { false, false, true, true, true, true };
}

QVariant ShaderPackFolderModel::data(const QModelIndex& index, int role) const
{
    if (!validateIndex(index))
        return {};

    int row = index.row();
    int column = index.column();

    switch (role) {
        case Qt::DisplayRole:
            switch (column) {
                case NameColumn:
                    return m_resources[row]->name();
                case VersionColumn:
                    return at(row).version();
                case DateColumn:
                    return m_resources[row]->dateTimeChanged();
                case ProviderColumn:
                    return m_resources[row]->provider();
                case SizeColumn:
                    return m_resources[row]->sizeStr();
                default:
                    return {};
            }
        case Qt::ToolTipRole:
            if (column == NameColumn) {
                if (at(row).isSymLinkUnder(instDirPath())) {
                    return m_resources[row]->internal_id() +
                           tr("\nWarning: This resource is symbolically linked from elsewhere. Editing it will also change the original."
                              "\nCanonical Path: %1")
                               .arg(at(row).fileinfo().canonicalFilePath());
                }
                if (at(row).isMoreThanOneHardLink()) {
                    return m_resources[row]->internal_id() +
                           tr("\nWarning: This resource is hard linked elsewhere. Editing it will also change the original.");
                }
            }
            return m_resources[row]->internal_id();
        case Qt::DecorationRole: {
            if (column == NameColumn && (at(row).isSymLinkUnder(instDirPath()) || at(row).isMoreThanOneHardLink()))
                return QIcon::fromTheme("status-yellow");
            return {};
        }
        case Qt::CheckStateRole:
            if (column == ActiveColumn)
                return at(row).enabled() ? Qt::Checked : Qt::Unchecked;
            return {};
        default:
            return {};
    }
}

QVariant ShaderPackFolderModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            switch (section) {
                case ActiveColumn:
                case NameColumn:
                case VersionColumn:
                case DateColumn:
                case ProviderColumn:
                case SizeColumn:
                    return columnNames().at(section);
                default:
                    return {};
            }
        case Qt::ToolTipRole: {
            switch (section) {
                case ActiveColumn:
                    return tr("Is the shader pack enabled?");
                case NameColumn:
                    return tr("The name of the shader pack.");
                case VersionColumn:
                    return tr("The version of the shader pack.");
                case DateColumn:
                    return tr("The date and time this shader pack was last changed (or added).");
                case ProviderColumn:
                    return tr("The source provider of the shader pack.");
                case SizeColumn:
                    return tr("The size of the shader pack.");
                default:
                    return {};
            }
        }
        default:
            break;
    }

    return {};
}

int ShaderPackFolderModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : NUM_COLUMNS;
}

Task* ShaderPackFolderModel::createPreUpdateTask()
{
    // Migrate metadata from main directory to .index directory (if it exists)
    // This handles the transition from the old behavior where metadata was stored in the main folder
    return new ShaderPackIndexMigrateTask(ResourceFolderModel::indexDir(), m_dir);
}

#include "ShaderPackFolderModel.moc"
