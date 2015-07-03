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

// Include
#include "mainwindow.h"
#include "Dock1_dockwidget.h"
#include "tool_resourcetree_widget.h"

//****************************************************************************/
Dock1DockWidget::Dock1DockWidget(QString title, MainWindow* parent, Qt::WindowFlags flags) : 
	QDockWidget (title, parent, flags), 
	mParent(parent)
{
    Magus::QtResourceTreeWidget* resourceTreeWidget = new Magus::QtResourceTreeWidget(QString("../common/icons/"));

    int LEVEL_X000_AUDIO = 1;
    int LEVEL_X000_MESH = 2;
    int LEVEL_X000_TEXTURE = 3;
    //int LEVEL_0X00_MESH = 10 * LEVEL_X000_MESH;
    //int LEVEL_00X0_MESH = 100 * LEVEL_X000_MESH;
    //int RESOURCE_MESH_BASE = 1000 * LEVEL_X000_MESH;
    //int RESOURCE_TEXTURE_BASE = 1000 * LEVEL_X000_TEXTURE;

    // Initialize resourceTreeWidget:
    // - The contexmenu is enabled (default)
    // - Add toplevel groups to the resourceTreeWidget up front
    // - Disable the contextmenu-item to add/create a new toplevel group
    // - Don't register the resource (with resourceTreeWidget->registerResource)
    //      => icons of subgroups cannot be selected from the contextmenu, but are inherited from toplevel group
    // - Toplevel groups cannot be deleted
    // - Subgroups and assets are not added up front
    //      => Add them by means of the contextmenu
    resourceTreeWidget->addResource (LEVEL_X000_AUDIO, 0, QString("Audio"), QString("audio.png"));
    resourceTreeWidget->addResource (LEVEL_X000_MESH, 0, QString("Mesh"), QString("softbody.png"));
    resourceTreeWidget->addResource (LEVEL_X000_TEXTURE, 0, QString("Texture"), QString("texture_bold.png"));
    resourceTreeWidget->setCreateTopLevelGroupContextMenuItemEnabled(false);
    resourceTreeWidget->setDeleteTopLevelGroupEnabled(false);

    //resourceTreeWidget->registerResource(LEVEL_X000_AUDIO, QString("Audio"), QString("audio.png"));
    //resourceTreeWidget->registerResource(LEVEL_X000_MESH, QString("Mesh"), QString("softbody.png"));
    //resourceTreeWidget->registerResource(LEVEL_X000_TEXTURE, QString("Texture"), QString("texture_bold.png"));

    // Sublevel groups
    //resourceTreeWidget->addResource (LEVEL_0X00_MESH, LEVEL_X000_MESH, QString("SubMesh1"), QString("softbody.png"));
    //resourceTreeWidget->addResource (LEVEL_00X0_MESH, LEVEL_0X00_MESH, QString("SubMesh2"), QString("softbody.png"));

    // Assets
    //resourceTreeWidget->addResource (RESOURCE_MESH_BASE + 1, LEVEL_00X0_MESH, QString("SM1"), QString(""));
    //resourceTreeWidget->addResource (RESOURCE_MESH_BASE + 2, LEVEL_00X0_MESH, QString("SM2"), QString(""));
    //resourceTreeWidget->addResource (RESOURCE_TEXTURE_BASE + 1, LEVEL_X000_TEXTURE, QString("T1"), QString(""));
    //resourceTreeWidget->addResource (RESOURCE_TEXTURE_BASE + 2, LEVEL_X000_TEXTURE, QString("T1"), QString(""));
    //resourceTreeWidget->addResource (RESOURCE_TEXTURE_BASE + 3, LEVEL_X000_TEXTURE, QString("T2"), QString(""));

    //resourceTreeWidget->deleteResource(LEVEL_X000_MESH); // TEST

    mInnerMain = new QMainWindow();
    mInnerMain->setCentralWidget(resourceTreeWidget);
    setWidget(mInnerMain);
    QWidget* oldTitleBar = titleBarWidget();
    setTitleBarWidget(new QWidget());
    delete oldTitleBar;
    //setWindowFlags(Qt::WindowTitleHint | Qt::FramelessWindowHint);

    // Perform standard functions
    createActions();
    createMenus();
    createToolBars();
}

//****************************************************************************/
Dock1DockWidget::~Dock1DockWidget(void)
{
}

//****************************************************************************/
void Dock1DockWidget::createActions(void)
{

}

//****************************************************************************/
void Dock1DockWidget::createMenus(void)
{

}

//****************************************************************************/
void Dock1DockWidget::createToolBars(void)
{

}



