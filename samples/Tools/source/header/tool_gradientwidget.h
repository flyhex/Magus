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

#ifndef MAGUS_TOOL_GRADIENT_WIDGET_H
#define MAGUS_TOOL_GRADIENT_WIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>
#include "tool_gradient.h"
#include "tool_gradientmarker.h"

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

namespace Magus
{
    /****************************************************************************
    Main class for gradient editor
    ***************************************************************************/
    class QtGradientWidget : public QWidget
    {
        Q_OBJECT

        public:
            QtGradientWidget(QWidget* parent = 0);
            virtual ~QtGradientWidget(void);

            // To be used for mouse events
            bool QtGradientWidget::eventFilter(QObject* object, QEvent* event);

            // Set/get the width and the height offset (in fraction of the total widget height) of the gradient
            void setGradientDimension (qreal width, qreal heightOffsetFraction = 0.13f);
            qreal getGradientHeightOffsetFraction(void) const {return mGradientHeightOffsetFraction;};
            qreal getGradientWidth(void) const {return mGradientWidth;};

            // Returns colors, without alpha values
            QMultiMap<qreal, QColor> getColorMap(void);

            // Set colors, without alpha values; this results in adding color markers to the gradient
            void setColorMap(QMultiMap<qreal, QColor> colorMap);

            // Returns alpha values
            QMultiMap<qreal, int> getAlphaMap(void);

            // Set alpha values; this results in adding alpha markers to the gradient
            void setAlphaMap(QMultiMap<qreal, int> alphaMap);

            // Returns colors, including corresponding alpha values
            QMultiMap<qreal, QColor> exportColorAndAlpha(void);

            // Read colors from the colorMap and create a gradient
            // Note, that the alpha markers are derived from the colors in the colorMap (which contain an alpha value)
            // This means that if the colorMap was created by means of the 'exportColorAndAlpha' function, the
            // alpha markers may differ (in position and value) from the situation before the export was done.
            // If the original alpha values need to be restored, use setColorMap and setAlphaMap instead.
            void importColorAndAlpha(QMultiMap<qreal, QColor> colorMap);

        protected:
            QGraphicsItem* itemAt(const QPointF& pos);
            void enableAlphaWidgets(bool enabled);
            void enableColorWidgets(bool enabled);

            // Handle mouse events
            void mouseMoveHandler(QGraphicsSceneMouseEvent* mouseEvent);
            void mouseClickHandler(QGraphicsSceneMouseEvent* mouseEvent);

            // Create a marker for color. Argument 'mousePos' determines the mouse position
            // (where the marker is placed).
            QtGradientMarkerColor* createGradientMarkerColor(QGraphicsSceneMouseEvent* mouseEvent);

            // Create a marker for color.
            QtGradientMarkerColor* createGradientMarkerColor(qreal fraction, const QColor color = Qt::white);

            // Create a marker for alpha value. Argument 'mousePos' determines the mouse position
            // (where the marker is placed).
            QtGradientMarkerAlpha* createGradientMarkerAlpha(QGraphicsSceneMouseEvent* mouseEvent);

            // Create a marker for alpha value.
            QtGradientMarkerAlpha* createGradientMarkerAlpha(qreal fraction, qreal alpha = 255);

        private slots:
            void alphaSliderValueChanged(int value);
            void alphaEditValueChanged(QString value);
            void valueChangedRGB(void);
            void valueChangedHex(void);
            void dialogButtonClicked(void);
            void deleteAlphaMarker(void);
            void deleteColorMarker(void);
            void clearGradient(void);

        private:
            int idCounter;
            qreal mGradientHeightOffsetFraction;
            qreal mGradientWidth;
            QGraphicsView* mView;
            QGraphicsScene* mScene;
            QtGradient* mGradient;
            QSizeF mMarkerSize;
            qreal mAlphaY;
            qreal mColorY;
            qreal mMarkerMinX;
            qreal mMarkerMaxX;
            QtGradientMarker* mCurrentMarker;
            QSlider* mAlphaSlider;
            QLineEdit* mAlphaEdit;
            QColor mCurrentColorValue;
            QLineEdit* mR;
            QLineEdit* mG;
            QLineEdit* mB;
            QLineEdit* mHex;
            QLabel* mColorFill;
            QPushButton* mDialogButton;
            QPushButton* mDeleteAlphaMarkerButton;
            QPushButton* mDeleteColorMarkerButton;
            QPushButton* mClearButton;
            void updateRGBWithCurrentColor(void);
            void updateHexWithCurrentColor(void);
            void updateColorFill (void);
    };
}

#endif
