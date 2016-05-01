/****************************************************************************
**
** Copyright (C) 2016
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

#ifndef MAGUS_NODE_PORT_H
#define MAGUS_NODE_PORT_H

#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include "node_constants.h"
#include "node_porttype.h"
//#include "node_node.h"
#include "node_connection.h"

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

namespace Magus
{
    /****************************************************************************
    QtPort represents a Port-class which is visualised in a QtNode.
    A QtPort can be connected to another port under certain conditions. These
    conditions are defined by mean of a Policy
    ***************************************************************************/
    class QtPort : public QGraphicsPathItem
    {
        public:
            QtPort(unsigned int portId,
                   const QString& portName,
                   QtPortType portType,
                   QColor portColour,
                   QColor connectionColour,
                   QtPortShape portShape,
                   Alignment alignment,
                   qreal zoom,
                   QGraphicsItem* parent = 0);
            virtual ~QtPort(void);

            // Scale the port
            void setZoom(qreal zoom);

            // Redraw the port
            void redraw(void);

            // Set the color of the portName
            void setNameColor(const QColor& color);

            // Returns the width when the port is not zoomed
            qreal getNormalizedWidth(void);

            // Returns the height when the port is not zoomed
            qreal getNormalizedHeight(void);

            // Set the position according to its
            void setAlignedPos(const QPointF& pos);
            void setAlignedPos(qreal x, qreal y);

            // Set a port open or closed; if the port is closed, no connection can be made or there is
            // already a connection
            void setPortOpen(bool open);

            // Create a connection on this port. The port acts as base or a target
            QtConnection* createConnection(QtConnection* targetConnection = 0);

            // Set a connection to this port; This is an existing connection.
            // If base is set to 'true', this port becomes the base port; otherwise the target port
            void setConnection(QtConnection* connection, bool base);

            // Delete the connection of this port.
            void deleteConnection(void);
            void informConnectionDeleted(void);

            // Return the connection of this port. The port can act as base- or target-port.
            QtConnection* getConnection(void);

            // Update the base connection (redraw the connection for which this port is base)
            void updateConnection(const QPointF& altTargetPos = QPointF(0.0f, 0.0f));

            // Check whether 'this' port is allow to connect to the port, given as an argument
            // The porttype of each port is compared.
            bool isConnectionAllowed(QtPort* portToConnect);

            // Returns true if the port is connected to another node
            bool isPortConnected(void);

            // Return true is this port is the base port of a connection
            bool isBasePort(void);

            // Return true is this port is the target port of a connection
            bool isTargetPort(void);

            // Hide the port and move the endpoint of the connection to the header of the node
            void collapse(void);

            // Make the port visible and restore the endpoint of the connection
            void expand(void);

            // If the port is a copy of another port, its pointer can be passed as a reference
            // Nothing is done with this value
            void setCopyOfPort(QtPort* port);

            // Getters
            unsigned int getPortId(void) {return mPortId;}
            const QString& getPortName(void) {return mPortName;}
            Alignment getAlignment(void) {return mAlignment;}
            QtPortType getPortType(void) {return mPortType;}
            QColor getPortColour(void) {return mPortColour;}
            QColor getConnectionColour(void) {return mConnectionColour;}
            QtPortShape getPortShape(void) {return mPortShape;}
            bool isPortOpen(void) {return mPortOpen;}
            QtPort* getCopyOfPort(void) {return mCopyOfPort;}

        private:
            unsigned int mPortId;
            QString mPortName;
            QtPortType mPortType;
            QColor mPortColour;
            QColor mConnectionColour;
            QtPortShape mPortShape;
            Alignment mAlignment;
            qreal mZoom;
            QGraphicsItem* mParent;
            QGraphicsTextItem* mLabel;
            QFont mFont;
            bool mPortOpen;
            QtConnection* mConnection;
            bool mConnectionIsBase;
            QPointF mOriginalPos;
            QtPort* mCopyOfPort;
    };
}

#endif
