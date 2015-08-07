/****************************************************************************
**
** Copyright (C) 2014
**
** This file is generated by the Magus toolkit
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#ifndef MAGUS_TOOL_RESOURCES_ASSETS_H
#define MAGUS_TOOL_RESOURCES_ASSETS_H

#include <QtWidgets>
#include "constants.h"
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QToolBar>
#include <QTabWidget>
#include "tool_audiowidget.h"
#include "tool_generic_assetwidget.h"
#include "tool_default_texturewidget.h"

QT_BEGIN_NAMESPACE
class QDockWidget;
QT_END_NAMESPACE

namespace Magus
{
    static const QString TOOL_RESOURCE_ICON_AUDIO = QString("audio.png");
    static const QString TOOL_RESOURCE_ICON_MATERIAL = QString("material.png");
    static const QString TOOL_RESOURCE_ICON_MESH = QString("softbody.png");
    static const QString TOOL_RESOURCE_ICON_SCRIPT = QString("cog.png");
    static const QString TOOL_RESOURCE_ICON_TEXTURE = QString("texture.png");
    static const QString TOOL_GENERIC_ASSETWIDGET_ICON_MATERIAL = QString("asset_material.png");
    static const QString TOOL_GENERIC_ASSETWIDGET_ICON_MESH = QString("asset_mesh.png");
    static const QString TOOL_GENERIC_ASSETWIDGET_ICON_SCRIPT = QString("asset_script.png");

    /****************************************************************************
    This class represents a generic QtAssetsDockWidget
    ***************************************************************************/
    class QtAssetsDockWidget : public QDockWidget
    {
        Q_OBJECT

        public:
            QtAssetsDockWidget(const QString& iconDir, const QString& title, QMainWindow* parent, Qt::WindowFlags flags = 0);
            ~QtAssetsDockWidget(void);
            void createActions(void);
            void createMenus(void);
            void createToolBars(void);

            // Add a tab to this widget. The tab represents an asset (mesh, texture, etc.). The widget is a widget that corresponds wuth
            // the asset type.
            void addTab(QWidget* widget, const QIcon& icon, const QString name);

            // Add an asset to a particular asset widget (eg. audio, texture, etc.)
            void addAsset(int toplevelId, int parentId, int resourceId, const QString& name, const QString& baseName);

            // Delete an asset from a particular asset widget (eg. audio, texture, etc.)
            void deleteAsset(int toplevelId, const QString& name);

            // Select the appropriate tab, based on the given arguments
            void selectTab(int toplevelId, int parentId, int resourceId, const QString& name, const QString& baseName);

            // Apply filtering to the underlying widgets (mAudioSelection, mTextureSelection, ...)
            void filter(const QString& pattern);

            // Reset the filtering in the underlying widgets (mAudioSelection, mTextureSelection, ...)
            void resetFilter(void);

        signals:
            void tabChanged(int index);

        private slots:
            void handleTabChanged(int index);

        private:
            QMainWindow* mParent;
            QMainWindow* mInnerMain;
            QTabWidget* mTabWidget;
            QString mIconDir;
            QtAudioWidget* mAudioSelection;
            QtGenericAssetWidget* mMaterialSelection;
            QtGenericAssetWidget* mMeshSelection;
            QtGenericAssetWidget* mScriptSelection;
            QtDefaultTextureWidget* mTextureSelection;
    };
}

#endif

