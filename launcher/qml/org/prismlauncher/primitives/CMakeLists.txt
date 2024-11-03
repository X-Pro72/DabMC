
# SPDX-FileCopyrightText: 2024 Rachel Powers <508861+Ryex@users.noreply.github.com>
#
# SPDX-License-Identifier: GPL-3.0-only
#
#  Prism Launcher - Minecraft Launcher
#  Copyright (C) 2024 Rachel Powers <508861+Ryex@users.noreply.github.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, version 3.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

qt_add_qml_module(org_prismlauncher_primitives
    URI "org.prismlauncher.primitives"
    VERSION 1.0
    STATIC
    IMPORTS
        QtQuick
        org.prismlauncher.platform
)

target_sources(org_prismlauncher_primitives PRIVATE
    icon.cpp
    icon.h
    shadowedrectangle.cpp
    shadowedrectangle.h
    shadowedtexture.cpp
    shadowedtexture.h

    scenegraph/managedtexturenode.cpp
    scenegraph/managedtexturenode.h
    scenegraph/paintedrectangleitem.cpp
    scenegraph/paintedrectangleitem.h
    scenegraph/shadowedborderrectanglematerial.cpp
    scenegraph/shadowedborderrectanglematerial.h
    scenegraph/shadowedbordertexturematerial.cpp
    scenegraph/shadowedbordertexturematerial.h
    scenegraph/shadowedrectanglematerial.cpp
    scenegraph/shadowedrectanglematerial.h
    scenegraph/shadowedrectanglenode.cpp
    scenegraph/shadowedrectanglenode.h
    scenegraph/shadowedtexturematerial.cpp
    scenegraph/shadowedtexturematerial.h
    scenegraph/shadowedtexturenode.cpp
    scenegraph/shadowedtexturenode.h
)

qt_target_qml_sources(org_prismlauncher_primitives
    QML_FILES
        Separator.qml
        ShadowedImage.qml
)

target_include_directories(org_prismlauncher_primitives PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(org_prismlauncher_primitives 
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Quick
    org_prismlauncher_platform
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(_extra_options DEBUGINFO)
else()
    set(_extra_options PRECOMPILE OPTIMIZED)
endif()

qt_add_shaders(org_prismlauncher_primitives "shaders"
    BATCHABLE
    PREFIX "/qt/qml/org/prismlauncher/primitives/shaders"
    FILES
        shaders/shadowedrectangle.vert
        shaders/shadowedrectangle.frag
        shaders/shadowedrectangle_lowpower.frag
        shaders/shadowedborderrectangle.frag
        shaders/shadowedborderrectangle_lowpower.frag
        shaders/shadowedtexture.frag
        shaders/shadowedtexture_lowpower.frag
        shaders/shadowedbordertexture.frag
        shaders/shadowedbordertexture_lowpower.frag
    OUTPUTS
        shadowedrectangle.vert.qsb
        shadowedrectangle.frag.qsb
        shadowedrectangle_lowpower.frag.qsb
        shadowedborderrectangle.frag.qsb
        shadowedborderrectangle_lowpower.frag.qsb
        shadowedtexture.frag.qsb
        shadowedtexture_lowpower.frag.qsb
        shadowedbordertexture.frag.qsb
        shadowedbordertexture_lowpower.frag.qsb
    ${_extra_options}
    OUTPUT_TARGETS _out_targets
)


