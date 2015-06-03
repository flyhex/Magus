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
#include "tool_layered_sceneviewwidget.h"

namespace Magus
{
    //****************************************************************************/
    QtLayeredSceneViewWidget::QtLayeredSceneViewWidget(const QString iconDir, QWidget* parent) : QWidget(parent)
    {
        QHBoxLayout* mainLayout = new QHBoxLayout();
        mSceneViewWidget = new QtSceneViewWidget(iconDir, this);
        mSceneViewWidget->enableDragAndDrop(false);
        mLayerWidget = new QtLayerWidget(iconDir, mSceneViewWidget, this);
        connect(mLayerWidget, SIGNAL(layerCreatedOrAdded(int,QString)), this, SLOT(handleLayerAdded(int,QString)));

        // Layout
        mainLayout->addWidget(mLayerWidget, 9);
        mainLayout->addWidget(mSceneViewWidget, 10);
        setLayout(mainLayout);
    }

    //****************************************************************************/
    QtLayeredSceneViewWidget::~QtLayeredSceneViewWidget(void)
    {
    }

    //****************************************************************************/
    void QtLayeredSceneViewWidget::setSceneViewWidgetForDragDrop(QtSceneViewWidget* sceneViewWidget)
    {
        mLayerWidget->setSceneViewWidgetForDragDrop(sceneViewWidget);
    }

    //****************************************************************************/
    QtSceneViewWidget* QtLayeredSceneViewWidget::getSceneViewWidget(void)
    {
        return mSceneViewWidget;
    }

    //****************************************************************************/
    QtLayerWidget* QtLayeredSceneViewWidget::getLayerWidget(void)
    {
        return mLayerWidget;
    }
    //****************************************************************************/
    void QtLayeredSceneViewWidget::handleLayerAdded(int layerId, const QString& name)
    {
        // If a new layer is added, set the sceneview widget default to no drag/drop
        mSceneViewWidget->enableDragAndDrop(false); // Does this with all internal sceneviews
    }

}