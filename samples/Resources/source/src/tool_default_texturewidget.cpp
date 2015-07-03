/****************************************************************************
**
** Copyright (C) 2015
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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDir>
#include <QLabel>
#include <QImageReader>
#include <QListWidgetItem>
#include "tool_default_texturewidget.h"

namespace Magus
{
    //****************************************************************************/
    QtDefaultTextureAndText::QtDefaultTextureAndText(const QPixmap& pixmap,
                                                     const QString& name,
                                                     const QString& baseName,
                                                     const QSize& size,
                                                     QWidget* parent) : QWidget(parent)
    {
        setContentsMargins(-8, -8, -8, -8);
        setMinimumSize(size);
        setMaximumSize(size);
        QHBoxLayout* mainLayout = new QHBoxLayout;
        QVBoxLayout* textureAndNameLayout = new QVBoxLayout;

        mName = name;
        mBaseName = baseName;
        mBaseNameEdit = new QLineEdit;
        mBaseNameEdit->setText(mBaseName);
        mBaseNameEdit->setEnabled(false);

        mTextureLabel = new QLabel();
        mTextureLabel->setPixmap(pixmap);
        mTextureLabel->setScaledContents(true);

        // Layout
        textureAndNameLayout->addWidget(mTextureLabel, 1000);
        textureAndNameLayout->addWidget(mBaseNameEdit, 1);
        mainLayout->addLayout(textureAndNameLayout);
        setLayout(mainLayout);
    }

    //****************************************************************************/
    QtDefaultTextureAndText::~QtDefaultTextureAndText(void)
    {
    }

    //****************************************************************************/
    //****************************************************************************/
    //****************************************************************************/
    QtDefaultTextureWidget::QtDefaultTextureWidget(QWidget* parent) : QWidget(parent)
    {
        setWindowTitle(QString("Texture selection"));
        mNameTexture = QString("");
        mBaseNameTexture = QString("");
        mTextureSize = QSize(128, 128);
        mOriginIsFile = true;
        QHBoxLayout* mainLayout = new QHBoxLayout;
        QVBoxLayout* textureSelectionLayout = new QVBoxLayout;

        // Define selection widget (QListWidget)
        mSelectionList = new QListWidget();
        mSelectionList->setViewMode(QListView::ListMode);
        mSelectionList->setWrapping(true);
        mSelectionList->setWordWrap(true);
        mSelectionList->setSpacing(0);
        mSelectionList->setUniformItemSizes(true);
        mSelectionList->setMovement(QListView::Snap);
        mSelectionList->setFlow(QListView::LeftToRight);
        mSelectionList->setAcceptDrops(false);
        mSelectionList->setDropIndicatorShown(false);
        mSelectionList->setMouseTracking(true);

        // Layout
        textureSelectionLayout->addWidget(mSelectionList);
        mainLayout->addLayout(textureSelectionLayout);
        setLayout(mainLayout);
    }

    //****************************************************************************/
    QtDefaultTextureWidget::~QtDefaultTextureWidget(void)
    {
    }

    //****************************************************************************/
    void QtDefaultTextureWidget::addTexture(const QPixmap& pixmap, const QString name, const QString baseName)
    {
        QtDefaultTextureAndText* textureAndText = new QtDefaultTextureAndText(pixmap, name, baseName, mTextureSize, this);
        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(mTextureSize); // Must be present, otherwise the widget is not shown
        mSelectionList->addItem(item);
        mSelectionList->setItemWidget(item, textureAndText);
        connect(mSelectionList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleSelected(QListWidgetItem*)));
    }

    //****************************************************************************/
    void QtDefaultTextureWidget::setOriginIsFile(bool originIsFile)
    {
        mOriginIsFile = originIsFile;
    }

    //****************************************************************************/
    void QtDefaultTextureWidget::clearContent(void)
    {
        mSelectionList->clear();
    }

    //****************************************************************************/
    const QString& QtDefaultTextureWidget::getNameTexture(void)
    {
        return mNameTexture;
    }

    //****************************************************************************/
    const QString& QtDefaultTextureWidget::getBaseNameTexture(void)
    {
        return mBaseNameTexture;
    }

    //****************************************************************************/
    void QtDefaultTextureWidget::handleSelected(QListWidgetItem* item)
    {
        QWidget* widget = mSelectionList->itemWidget(item);
        if (widget)
        {
            QtDefaultTextureAndText* textureAndText = static_cast<QtDefaultTextureAndText*>(widget);
            emit selected(textureAndText->mName, textureAndText->mBaseName);
        }
    }

    //****************************************************************************/
    void QtDefaultTextureWidget::setTextureSize (QSize size)
    {
        mTextureSize = size;
    }

}
