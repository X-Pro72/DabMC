
// SPDX-FileCopyrightText: 2022 Rachel Powers <508861+Ryex@users.noreply.github.com>
//
// SPDX-License-Identifier: GPL-3.0-only

/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 */

#include "ShaderPack.h"
#include "FileSystem.h"
#include "Version.h"
#include <QRegularExpression>

ShaderPack::ShaderPack(QObject* parent) : Resource(parent) {}

ShaderPack::ShaderPack(QFileInfo file_info) : Resource(file_info)
{
    // For shaders, make internal_id unique based on full absolute path
    // This allows multiple versions of the same shader (with same filename) to coexist
    m_internal_id = FS::NormalizePath(file_info.absoluteFilePath());
}

void ShaderPack::setFile(QFileInfo file_info)
{
    // Call parent's setFile first to do the normal parsing
    Resource::setFile(file_info);
    
    // For shaders, make internal_id unique based on full absolute path
    // This allows multiple versions of the same shader (with same filename) to coexist
    m_internal_id = FS::NormalizePath(file_info.absoluteFilePath());
}

void ShaderPack::setPackFormat(ShaderPackFormat new_format)
{
    QMutexLocker locker(&m_data_lock);

    m_pack_format = new_format;
}

auto ShaderPack::version() const -> QString
{
    if (metadata() && !metadata()->version_number.isEmpty()) {
        return metadata()->version_number;
    }
    
    // Try to extract version from filename as fallback
    // Common patterns: "shader-v1.2.3.zip", "shader-1.2.3.zip", "shader_v1.2.3.zip"
    QString filename = m_file_info.completeBaseName(); // filename without extension
    if (filename.isEmpty()) {
        return {};
    }
    
    // Look for version patterns in filename
    // Pattern: v followed by numbers and dots (e.g., v1.2.3, v2.0)
    QRegularExpression versionPattern(R"(v?(\d+\.\d+(?:\.\d+)?(?:\.\d+)?))", QRegularExpression::CaseInsensitiveOption);
    auto match = versionPattern.match(filename);
    if (match.hasMatch()) {
        return match.captured(1); // Return the version number without 'v' prefix
    }
    
    // If no version found and no metadata, return empty (will show as blank in UI)
    return {};
}

int ShaderPack::compare(const Resource& other, SortType type) const
{
    auto cast_other = dynamic_cast<ShaderPack const*>(&other);
    if (!cast_other)
        return Resource::compare(other, type);

    switch (type) {
        default:
        case SortType::ENABLED:
        case SortType::NAME:
        case SortType::DATE:
        case SortType::SIZE:
        case SortType::PROVIDER:
            return Resource::compare(other, type);
        case SortType::VERSION: {
            auto this_ver = Version(version());
            auto other_ver = Version(cast_other->version());
            if (this_ver > other_ver)
                return 1;
            if (this_ver < other_ver)
                return -1;
            break;
        }
    }
    return 0;
}

bool ShaderPack::valid() const
{
    return m_pack_format != ShaderPackFormat::INVALID;
}
