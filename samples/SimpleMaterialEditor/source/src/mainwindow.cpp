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
#include <QString>
#include <QFile>
#include <QMenuBar>
#include "mainwindow.h"


//****************************************************************************/
MainWindow::MainWindow(void) : mIsClosing(false)
{
    // Create the Ogre Manager
    mOgreManager = new OgreManager();
    
	// Perform standard functions
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();


    // Set the title
    setWindowTitle(QString("SimpleMaterialEditor"));

    // Set the stylesheet of the application
    QFile File(QString("dark.qss"));
    File.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(File.readAll());
    setStyleSheet(styleSheet);
	showMaximized();
    setEnabled(true);
}

//****************************************************************************/
MainWindow::~MainWindow(void)
{
    delete mOgreManager;
}

//****************************************************************************/
void MainWindow::closeEvent(QCloseEvent* event)
{
    mIsClosing = true;
}

//****************************************************************************/
void MainWindow::createActions(void)
{
    mNewMenuAction = new QAction(QString("New"), this);
    connect(mNewMenuAction, SIGNAL(triggered()), this, SLOT(doNewMenuAction()));
    mQuitMenuAction = new QAction(QString("Quit"), this);
    connect(mQuitMenuAction, SIGNAL(triggered()), this, SLOT(doQuitMenuAction()));
    mResetWindowLayoutMenuAction = new QAction(QString("Reset Window Layout"), this);
    connect(mResetWindowLayoutMenuAction, SIGNAL(triggered()), this, SLOT(doResetWindowLayoutMenuAction()));

}

//****************************************************************************/
void MainWindow::createMenus(void)
{
    mFileMenu = menuBar()->addMenu(QString("File"));
    mFileMenu->addAction(mNewMenuAction);
    mFileMenu->addAction(mQuitMenuAction);
    mWindowMenu = menuBar()->addMenu(QString("Window"));
    mWindowMenu->addAction(mResetWindowLayoutMenuAction);

}

//****************************************************************************/
void MainWindow::createToolBars(void)
{

}

//****************************************************************************/
void MainWindow::createStatusBar(void)
{

}

//****************************************************************************/
void MainWindow::createDockWindows(void)
{
    mRendererDockWidget = new RendererDockWidget("Renderer", this);
    addDockWidget(Qt::LeftDockWidgetArea, mRendererDockWidget);
    mEditorDockWidget = new EditorDockWidget("Editor", this);
    addDockWidget(Qt::LeftDockWidgetArea, mEditorDockWidget);
    mAssetDockWidget = new AssetDockWidget("Asset", this);
    addDockWidget(Qt::RightDockWidgetArea, mAssetDockWidget);
}

//****************************************************************************/
void MainWindow::doNewMenuAction(void)
{
    // Restart again
    mEditorDockWidget->start();
    mAssetDockWidget->start();
}

//****************************************************************************/
void MainWindow::doQuitMenuAction(void)
{
    close();
}

//****************************************************************************/
void MainWindow::doResetWindowLayoutMenuAction(void)
{
    mRendererDockWidget->show();
    addDockWidget(Qt::LeftDockWidgetArea, mRendererDockWidget);
    mEditorDockWidget->show();
    addDockWidget(Qt::LeftDockWidgetArea, mEditorDockWidget);
    mAssetDockWidget->show();
    addDockWidget(Qt::RightDockWidgetArea, mAssetDockWidget);
}


//****************************************************************************/
void MainWindow::update(void)
{
    if (mOgreManager)
        mOgreManager->renderOgreWidgetsOneFrame();
}