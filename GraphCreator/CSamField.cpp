#include "CSamField.h"
#include "CSamFrame.h"

CSamField::CSamField(QWidget *parent) : QWidget{parent}, graph(SamGraph()) {
    scale = 1;
    option = Option::CURSOR;
    coords = QPoint(0, 0);
    rightButtonPressed = false;
    leftButtonPressed = false;
    ShiftOn = false;
    writing = false;
    GOn = true;
    ctrlOn = false;
    ver1 = -1;
    ver2 = -1;
    verToMove = -1;
    txtToMove = -1;

    txtInWriting = nullptr;
    gridBinding = false;

    mouseTracker = QPointF(0, 0);

    setCursor(Qt::CrossCursor);

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    this->setStyleSheet("background-color: rgb(253, 246, 227);");
}

void CSamField::resetOptions() {
    ver1 = -1;
    ver2 = -1;
    verToMove = -1;
    txtToMove = -1;
    txtInWriting = nullptr;
    unselectAll();
}

void CSamField::resetWriting() {
    if (txtInWriting != nullptr) {
        if (txtInWriting->text == "") txtInWriting->text = "None";
        txtInWriting = nullptr;
    }
    writing = false;
}

void CSamField::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift) {
        ShiftOn = true;
    }

    if (event->key() == Qt::Key_G && !writing) {
        GOn = !GOn;
        repaint();
    }
    if (event->key() == Qt::Key_B && !writing) {
        gridBinding = !gridBinding;
        repaint();
    }
    if (event->key() == Qt::Key_Control)
        ctrlOn = true;

    if (ctrlOn && event->key() == Qt::Key_S) {
        dynamic_cast<CSamFrame*>(parent())->saveFile();
        ctrlOn = false;
    }

    if (ctrlOn && event->key() == Qt::Key_O) {
        dynamic_cast<CSamFrame*>(parent())->openFile();
        ctrlOn = false;
    }

    if (writing) {
        if (event->key() == Qt::Key_Backspace && !txtInWriting->text.isEmpty()) {
            txtInWriting->text.chop(1);
        }
        else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            resetWriting();
            this->unselectAll();
        }
        else {
            txtInWriting->text += event->text();
        }
        repaint();
    }

    QWidget::keyPressEvent(event);
}

void CSamField::focusOutEvent(QFocusEvent *event) {
    setFocus();
    QWidget::focusOutEvent(event);
}


void CSamField::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift) {
        ShiftOn = false;
    }
    if (event->key() == Qt::Key_Control) {
        ctrlOn = false;
    }

    QWidget::keyReleaseEvent(event);
}

void CSamField::paintEvent(QPaintEvent *event) {
    QPainter painter;
    painter.begin(this);
    if (GOn)
        drawGrid(&painter);
    drawEdgs(&painter);
    drawArrows(&painter);
    drawSelfArrows(&painter);
    drawVrxs(&painter);
    drawTxts(&painter);
    drawSelected(&painter);
    painter.end();

    QWidget::paintEvent(event);
}

void CSamField::selectEdge(int idx) {
    selectedEdges.push_back(&graph.edgs[idx]);
    auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [this, idx](const Text& txt) {
        return graph.edgs[idx].textBinding == txt.idx;
    });
    selectedTxts.push_back(&(*iter));
}

void CSamField::selectArrow(int idx) {
    selectedArrows.push_back(&graph.arrs[idx]);
    auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [this, idx](const Text& txt) {
        return graph.arrs[idx].textBinding == txt.idx;
    });
    selectedTxts.push_back(&(*iter));
}

void CSamField::selectVrx(int idx) {
    selectedVrxs.push_back(&graph.verxs[idx]);

    for (int i = 0; i < (int)graph.edgs.size(); i++) {
        if (idx == graph.edgs[i].firstIdx || idx == graph.edgs[i].secondIdx) {
            selectEdge(i);
        }
    }
    for (int i = 0; i < (int)graph.arrs.size(); i++) {
        if (idx == graph.arrs[i].firstIdx || idx == graph.arrs[i].secondIdx) {
            selectArrow(i);
        }
    }
    for (int i = 0; i < (int)graph.self_arrs.size(); i++) {
        if (idx == graph.self_arrs[i].verIdx) {
            selectSelfEdge(i);
        }
    }

    auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [this, idx](const Text& txt) {
        return graph.verxs[idx].textBinding == txt.idx;
    });

    selectedTxts.push_back(&(*iter));
}

void CSamField::selectOnlyVrx(int idx) {
    selectedVrxs.push_back(&graph.verxs[idx]);

    auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [this, idx](const Text& txt) {
        return graph.verxs[idx].textBinding == txt.idx;
    });

    selectedTxts.push_back(&(*iter));
}

void CSamField::selectTxt(int idx) {
    if (graph.txts[idx].binding == -1) {
        selectedTxts.push_back(&graph.txts[idx]);
    }
    else {
        if (graph.txts[idx].notLine) {
            auto iter = std::find_if(graph.verxs.begin(), graph.verxs.end(), [this, idx](const Vertex& vrx) {
                return graph.txts[idx].binding == vrx.idx;
            });
            selectOnlyVrx(std::distance(graph.verxs.begin(), iter));
        }
        else if (graph.txts[idx].type == TypeEdge::EDGE) {
            auto iter = std::find_if(graph.edgs.begin(), graph.edgs.end(), [this, idx](const Edge& edge) {
                return graph.txts[idx].binding == edge.idx;
            });
            selectEdge(std::distance(graph.edgs.begin(), iter));
        }
        else if (graph.txts[idx].type == TypeEdge::ARROW) {
            auto iter = std::find_if(graph.arrs.begin(), graph.arrs.end(), [this, idx](const Edge& arrow) {
                return graph.txts[idx].binding == arrow.idx;
            });
            selectArrow(std::distance(graph.arrs.begin(), iter));
        }
        else if (graph.txts[idx].type == TypeEdge::SELF_ARROW) {
            auto iter = std::find_if(graph.self_arrs.begin(), graph.self_arrs.end(), [this, idx](const SelfEdge& edge) {
                return graph.txts[idx].binding == edge.idx;
            });
            selectSelfEdge(std::distance(graph.self_arrs.begin(), iter));
        }
    }
}

void CSamField::selectSelfEdge(int idx) {
    selectedSelfEdges.push_back(&graph.self_arrs[idx]);
    auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [this, idx](const Text& txt) {
        return (graph.self_arrs[idx].textBinding == txt.idx) && (txt.type == TypeEdge::SELF_ARROW);
    });
    selectedTxts.push_back(&(*iter));
}

void CSamField::unselectAll() {
    selectedVrxs.clear();
    selectedArrows.clear();
    selectedEdges.clear();
    selectedTxts.clear();
    selectedSelfEdges.clear();
}

void CSamField::drawSelected(QPainter* painter) {
    QPen pen(QColor(60, 120, 60));
    pen.setWidth(2);
    painter->setPen(pen);

    // Неориентированные рёбра
    for (int i = 0; i < (int)selectedEdges.size(); i++) {
        QPointF startPoint = getPlace(graph.verxs[selectedEdges[i]->firstIdx].coords) / scale;
        QPointF endPoint = getPlace(graph.verxs[selectedEdges[i]->secondIdx].coords) / scale;

        QLineF line(startPoint, endPoint);
        qreal angle = std::atan2(-line.dy(), line.dx());

        qreal startRadius = graph.verxs[selectedEdges[i]->firstIdx].radius / scale;
        qreal endRadius = graph.verxs[selectedEdges[i]->secondIdx].radius / scale;

        QPointF correctedStart = startPoint + QPointF(std::cos(angle) * startRadius, -std::sin(angle) * startRadius);
        QPointF correctedEnd = endPoint - QPointF(std::cos(angle) * endRadius, -std::sin(angle) * endRadius);

        painter->drawLine(correctedStart, correctedEnd);
    }

    // Ориентированные рёбра
    for (int i = 0; i < (int)selectedArrows.size(); i++) {
        QPointF startPoint = getPlace(graph.verxs[selectedArrows[i]->firstIdx].coords) / scale;
        QPointF endPoint = getPlace(graph.verxs[selectedArrows[i]->secondIdx].coords) / scale;

        QLineF line(startPoint, endPoint);
        qreal angle = std::atan2(-line.dy(), line.dx());

        qreal startRadius = graph.verxs[selectedArrows[i]->firstIdx].radius / scale;
        qreal endRadius = graph.verxs[selectedArrows[i]->secondIdx].radius / scale;

        QPointF correctedStart = startPoint + QPointF(std::cos(angle) * startRadius, -std::sin(angle) * startRadius);
        QPointF correctedEnd = endPoint - QPointF(std::cos(angle) * endRadius, -std::sin(angle) * endRadius);

        painter->drawLine(correctedStart, correctedEnd);

        const qreal arrowSize = 15;

        QPointF arrowP1 = correctedEnd + QPointF(std::sin(angle - 2 * M_PI / 3) * arrowSize / scale,
                                                 std::cos(angle - 2 * M_PI / 3) * arrowSize / scale);
        QPointF arrowP2 = correctedEnd + QPointF(std::sin(angle - M_PI / 3) * arrowSize / scale,
                                                 std::cos(angle - M_PI / 3) * arrowSize / scale);

        painter->drawLine(correctedEnd, arrowP1);
        painter->drawLine(correctedEnd, arrowP2);
    }
    // Рёбра в ту же вершину
    for (int i = 0; i < (int)selectedSelfEdges.size(); i++) {
        painter->drawEllipse(QRect((getPlace(QPointF(graph.verxs[selectedSelfEdges[i]->verIdx].coords.x()
                                                       - graph.verxs[selectedSelfEdges[i]->verIdx].radius / 2,
                                                   graph.verxs[selectedSelfEdges[i]->verIdx].coords.y()
                                                       - graph.verxs[selectedSelfEdges[i]->verIdx].radius / 2)) / scale).toPoint(),
                                   (getPlace(QPointF(graph.verxs[selectedSelfEdges[i]->verIdx].coords.x()
                                                       + graph.verxs[selectedSelfEdges[i]->verIdx].radius / 2,
                                                   graph.verxs[selectedSelfEdges[i]->verIdx].coords.y()
                                                       - graph.verxs[selectedSelfEdges[i]->verIdx].radius * 2)) / scale).toPoint()));
    }

    if (selectedVrxs.empty()) {
        QFontMetrics fm(QApplication::font());
        painter->setBrush(QColor(255, 255, 255));
        QPen pen(QColor(120, 60, 60));
        pen.setWidth(2);
        painter->setPen(pen);
        std::deque<Vertex*> verxs_self_sels;
        for (int i = 0; i < (int)selectedSelfEdges.size(); i++) {
            Vertex& verx = graph.verxs[selectedSelfEdges[i]->verIdx];
            verxs_self_sels.push_back(&verx);
            painter->drawEllipse(getPlace(verx.coords) / scale, int(verx.radius / scale), int(verx.radius / scale));
        }

        pen.setColor(QColor(60, 60, 180));
        pen.setWidth(1);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        for (int i = 0; i < (int)verxs_self_sels.size(); i++) {
            auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [&](const Text& txt)
                                     { return (txt.idx == verxs_self_sels[i]->textBinding) && txt.notLine; });
            QPointF textPos = getPlace(iter->coords + verxs_self_sels[i]->coords) / scale;

            QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, iter->text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            if (option == Option::TEXT) {
                painter->drawRect(boundingRect);
            }

            painter->drawText(textPos - QPointF(boundingRect.width() / 2, 0), iter->text);
        }
        pen.setColor(QColor(60, 120, 60));
        pen.setWidth(2);
        painter->setPen(pen);
    }

    // Вершины
    painter->setBrush(QColor(255, 255, 255));
    for (int i = 0; i < (int)selectedVrxs.size(); i++) {
        QPointF place = this->getPlace(selectedVrxs[i]->coords) / scale;
        int radius = static_cast<int>(selectedVrxs[i]->radius) / scale;
        painter->drawEllipse(place, radius, radius);
    }
    painter->setBrush(Qt::NoBrush);

    // Текст
    QFontMetrics fm(QApplication::font());

    for (int i = 0; i < (int)selectedTxts.size(); i++) {
        if (selectedTxts[i]->notLine) {
            auto iter = std::find_if(graph.verxs.begin(), graph.verxs.end(), [&](const Vertex& vrx){ return (vrx.idx == selectedTxts[i]->binding) && selectedTxts[i]->notLine; });
            QPointF textPos = selectedTxts[i]->binding != -1 ?
                                 getPlace(selectedTxts[i]->coords + graph.verxs[std::distance(graph.verxs.begin(), iter)].coords) / scale
                                                         : getPlace(selectedTxts[i]->coords) / scale;

            QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, selectedTxts[i]->text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            if ((option == Option::TEXT) || (option == Option::CLEAR && selectedTxts[i]->binding == -1)
                || (option == Option::CLEAR && !selectedTxts[i]->notLine))
                painter->drawRect(boundingRect);

            painter->drawText(textPos - QPoint(boundingRect.width() / 2, 0), selectedTxts[i]->text);
        }
        else if (selectedTxts[i]->type == TypeEdge::EDGE) {
            auto iter = std::find_if(graph.edgs.begin(), graph.edgs.end(), [&](const Edge& edg){ return (edg.idx == selectedTxts[i]->binding); });
            QPointF textPos = selectedTxts[i]->coords / scale + (getPlace(graph.verxs[graph.edgs[std::distance(graph.edgs.begin(), iter)].firstIdx].coords)
                                                             + getPlace(graph.verxs[graph.edgs[std::distance(graph.edgs.begin(), iter)].secondIdx].coords)) / scale / 2;


            QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, selectedTxts[i]->text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            if (option == Option::TEXT) {
                painter->drawRect(boundingRect);
            }

            painter->drawText(textPos - QPoint(boundingRect.width() / 2, 0), selectedTxts[i]->text);
        }
        else if (selectedTxts[i]->type == TypeEdge::ARROW) {
            auto iter = std::find_if(graph.arrs.begin(), graph.arrs.end(), [&](const Edge& edg){ return (edg.idx == selectedTxts[i]->binding); });
            QPointF textPos = selectedTxts[i]->coords / scale + (getPlace(graph.verxs[graph.arrs[std::distance(graph.arrs.begin(), iter)].firstIdx].coords)
                                                             + getPlace(graph.verxs[graph.arrs[std::distance(graph.arrs.begin(), iter)].secondIdx].coords)) / scale / 2;


            QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, selectedTxts[i]->text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            if (option == Option::TEXT) {
                painter->drawRect(boundingRect);
            }

            painter->drawText(textPos - QPoint(boundingRect.width() / 2, 0), selectedTxts[i]->text);
        }
        else if (selectedTxts[i]->type == TypeEdge::SELF_ARROW) {
            auto iter = std::find_if(graph.self_arrs.begin(), graph.self_arrs.end(), [&](const SelfEdge& edg){ return (edg.idx == selectedTxts[i]->binding); });
            QPointF textPos = selectedTxts[i]->coords / scale + (getPlace(graph.verxs[graph.self_arrs[std::distance(graph.self_arrs.begin(), iter)].verIdx].coords)
                                                                + getPlace(graph.verxs[graph.self_arrs[std::distance(graph.self_arrs.begin(), iter)].verIdx].coords)) / scale / 2;


            QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, selectedTxts[i]->text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            if (option == Option::TEXT) {
                painter->drawRect(boundingRect);
            }

            painter->drawText(textPos - QPoint(boundingRect.width() / 2, 0), selectedTxts[i]->text);
        }
    }
}

void CSamField::drawEdgs(QPainter* painter) {
    QPen pen(QColor(120, 60, 60));
    pen.setWidth(2);
    painter->setPen(pen);

    for (int i = 0; i < (int)graph.edgs.size(); i++) {
        painter->drawLine(getPlace(graph.verxs[graph.edgs[i].firstIdx].coords) / scale, getPlace(graph.verxs[graph.edgs[i].secondIdx].coords) / scale);
    }

    if (option == Option::EDGE && ver1 != -1) {
        painter->drawLine(getPlace(graph.verxs[ver1].coords) / scale, mouseTracker);
    }
}

void CSamField::drawTxts(QPainter* painter) {
    QPen pen(QColor(60, 60, 180));
    painter->setPen(pen);

    QFontMetrics fm(QApplication::font());
    std::deque<QRect> special_rects;

    for (int i = 0; i < (int)graph.txts.size(); i++) {
        if (graph.txts[i].notLine) {
            auto iter = std::find_if(graph.verxs.begin(), graph.verxs.end(), [&](const Vertex& vrx){ return (vrx.idx == graph.txts[i].binding) && graph.txts[i].notLine; });
            QPointF textPos = graph.txts[i].binding != -1 ?
                                 getPlace(graph.txts[i].coords + graph.verxs[std::distance(graph.verxs.begin(), iter)].coords) / scale
                                                         : getPlace(graph.txts[i].coords) / scale;

            QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, graph.txts[i].text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            if ((option == Option::TEXT) || (option == Option::CLEAR && graph.txts[i].binding == -1)
                || (option == Option::CLEAR && !graph.txts[i].notLine))
                painter->drawRect(boundingRect);

            painter->drawText(textPos - QPoint(boundingRect.width() / 2, 0), graph.txts[i].text);
        }
        else if (graph.txts[i].type == TypeEdge::EDGE) {
            auto iter = std::find_if(graph.edgs.begin(), graph.edgs.end(), [&](const Edge& edg){ return (edg.idx == graph.txts[i].binding); });
            QPointF textPos = graph.txts[i].coords / scale + (getPlace(graph.verxs[graph.edgs[std::distance(graph.edgs.begin(), iter)].firstIdx].coords)
                                                               + getPlace(graph.verxs[graph.edgs[std::distance(graph.edgs.begin(), iter)].secondIdx].coords)) / scale / 2;


            QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, graph.txts[i].text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            if (option == Option::TEXT) {
                painter->drawRect(boundingRect);
            }
            if (option == Option::CLEAR)
                special_rects.push_back(boundingRect);

            painter->drawText(textPos - QPoint(boundingRect.width() / 2, 0), graph.txts[i].text);
        }
        else if (graph.txts[i].type == TypeEdge::ARROW) {
            auto iter = std::find_if(graph.arrs.begin(), graph.arrs.end(), [&](const Edge& edg){ return (edg.idx == graph.txts[i].binding); });
            QPointF textPos = graph.txts[i].coords / scale + (getPlace(graph.verxs[graph.arrs[std::distance(graph.arrs.begin(), iter)].firstIdx].coords)
                                                             + getPlace(graph.verxs[graph.arrs[std::distance(graph.arrs.begin(), iter)].secondIdx].coords)) / scale / 2;


            QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, graph.txts[i].text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            if (option == Option::TEXT) {
                painter->drawRect(boundingRect);
            }
            if (option == Option::CLEAR)
                special_rects.push_back(boundingRect);

            painter->drawText(textPos - QPoint(boundingRect.width() / 2, 0), graph.txts[i].text);
        }
        else if (graph.txts[i].type == TypeEdge::SELF_ARROW) {
            auto iter = std::find_if(graph.self_arrs.begin(), graph.self_arrs.end(), [&](const SelfEdge& edg){ return (edg.idx == graph.txts[i].binding); });
            QPointF textPos = graph.txts[i].coords / scale + (getPlace(graph.verxs[graph.self_arrs[std::distance(graph.self_arrs.begin(), iter)].verIdx].coords)) / scale;


            QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, graph.txts[i].text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            if (option == Option::TEXT) {
                painter->drawRect(boundingRect);
            }
            if (option == Option::CLEAR)
                special_rects.push_back(boundingRect);

            painter->drawText(textPos - QPoint(boundingRect.width() / 2, 0), graph.txts[i].text);
        }
    }
    painter->setPen(QPen(QColor(255, 130, 130)));
    painter->setBrush(Qt::NoBrush);
    for (int i = 0; i < (int)special_rects.size(); i++) {
        painter->drawRect(special_rects[i]);
    }
}

void CSamField::drawArrows(QPainter* painter) {
    QPen pen(QColor(120, 60, 60));
    pen.setWidth(2);
    painter->setPen(pen);

    for (int i = 0; i < (int)graph.arrs.size(); i++) {
        QPointF startPoint = getPlace(graph.verxs[graph.arrs[i].firstIdx].coords) / scale;
        QPointF endPoint = getPlace(graph.verxs[graph.arrs[i].secondIdx].coords) / scale;

        QLineF line(startPoint, endPoint);
        qreal angle = std::atan2(-line.dy(), line.dx());

        qreal startRadius = graph.verxs[graph.arrs[i].firstIdx].radius / scale;
        qreal endRadius = graph.verxs[graph.arrs[i].secondIdx].radius / scale;

        QPointF correctedStart = startPoint + QPointF(std::cos(angle) * startRadius, -std::sin(angle) * startRadius);
        QPointF correctedEnd = endPoint - QPointF(std::cos(angle) * endRadius, -std::sin(angle) * endRadius);

        painter->drawLine(correctedStart, correctedEnd);

        const qreal arrowSize = 15;

        QPointF arrowP1 = correctedEnd + QPointF(std::sin(angle - 2 * M_PI / 3) * arrowSize / scale,
                                                 std::cos(angle - 2 * M_PI / 3) * arrowSize / scale);
        QPointF arrowP2 = correctedEnd + QPointF(std::sin(angle - M_PI / 3) * arrowSize / scale,
                                                 std::cos(angle - M_PI / 3) * arrowSize / scale);

        painter->drawLine(correctedEnd, arrowP1);
        painter->drawLine(correctedEnd, arrowP2);
    }

    if (option == Option::ARROW && ver1 != -1) {
        QPointF startPoint = getPlace(graph.verxs[ver1].coords) / scale;
        QPointF endPoint = mouseTracker;

        QLineF line(startPoint, endPoint);
        qreal angle = std::atan2(-line.dy(), line.dx());

        painter->drawLine(startPoint, endPoint);

        const qreal arrowSize = 15;

        QPointF arrowP1 = endPoint + QPointF(std::sin(angle - 2 * M_PI / 3) * arrowSize / scale,
                                                 std::cos(angle - 2 * M_PI / 3) * arrowSize / scale);
        QPointF arrowP2 = endPoint + QPointF(std::sin(angle - M_PI / 3) * arrowSize / scale,
                                                 std::cos(angle - M_PI / 3) * arrowSize / scale);

        painter->drawLine(endPoint, arrowP1);
        painter->drawLine(endPoint, arrowP2);
    }
}

void CSamField::drawSelfArrows(QPainter* painter) {
    QPen pen(QColor(120, 60, 60));
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    for (int i = 0; i < (int)graph.self_arrs.size(); i++) {
        painter->drawEllipse(QRect((getPlace(QPoint(graph.verxs[graph.self_arrs[i].verIdx].coords.x()
                                                       - graph.verxs[graph.self_arrs[i].verIdx].radius / 2,
                                                   graph.verxs[graph.self_arrs[i].verIdx].coords.y()
                                                        - graph.verxs[graph.self_arrs[i].verIdx].radius / 2)) / scale).toPoint(),
                                   (getPlace(QPoint(graph.verxs[graph.self_arrs[i].verIdx].coords.x()
                                                       + graph.verxs[graph.self_arrs[i].verIdx].radius / 2,
                                                   graph.verxs[graph.self_arrs[i].verIdx].coords.y()
                                                       - graph.verxs[graph.self_arrs[i].verIdx].radius * 2)) / scale).toPoint()));
    }
}

void CSamField::drawVrxs(QPainter* painter) {
    QBrush brush(QColor(255, 255, 255));
    QPen pen(QColor(120, 60, 60));
    pen.setWidth(2);
    painter->setBrush(brush);
    painter->setPen(pen);


    for (int i = 0; i < (int)graph.verxs.size(); i++) {
        QPointF place = this->getPlace(graph.verxs[i].coords) / scale;
        int radius = static_cast<int>(graph.verxs[i].radius) / scale;
        painter->drawEllipse(place, radius, radius);
    }
}

void CSamField::drawGrid(QPainter* painter) const {

    QPen pen;
    pen.setColor(QColor(135, 70, 70, 70));
    pen.setWidthF(0.8);
    pen.setStyle(Qt::DashLine);

    painter->setPen(pen);

    int numCols = width() / 30 * scale;
    int numRows = height() / 30 * scale;

    for (int i = 0; i < numCols + 4; i++) {
        int x = (30 * i + (int)round(coords.x()) % 30) / scale;
        painter->drawLine(x, 0, x, height());
    }
    for (int i = 0; i < numRows + 3; i++) {
        int y = (30 * i + (int)round(coords.y()) % 30) / scale;
        painter->drawLine(0, y, width(), y);
    }
}

QPair<Vertex*, int> CSamField::getVertex(QPointF pos) {
    for (int i = 0; i < (int)graph.verxs.size(); i++) {
        if (sqrt(pow(pos.x() * scale - coords.x() - graph.verxs[i].coords.x(), 2) + pow(pos.y() * scale - coords.y() - graph.verxs[i].coords.y(), 2)) < graph.verxs[i].radius) {
            return qMakePair(&graph.verxs[i], i);
        }
    }
    return qMakePair(nullptr, -1);
}

QPair<Text*, int> CSamField::getText(QPointF pos) {
    QFontMetrics fm(QApplication::font());

    for (int i = 0; i < (int)graph.txts.size(); i++) {
        if (graph.txts[i].notLine) {

            auto iter = std::find_if(graph.verxs.begin(), graph.verxs.end(), [&](const Vertex& vrx){ return (vrx.idx == graph.txts[i].binding) && graph.txts[i].notLine; });

            QPointF textPos = graph.txts[i].binding != -1 ?
                                 getPlace(graph.txts[i].coords + graph.verxs[std::distance(graph.verxs.begin(), iter)].coords) / scale
                                                         : getPlace(graph.txts[i].coords) / scale;

            // Получаем ограничивающий прямоугольник текста
            QRect boundingRect = fm.boundingRect(textPos.x() * scale, (textPos.y() - 10) * scale, 0, 0, Qt::AlignLeft, graph.txts[i].text);
            boundingRect.translate(-boundingRect.width() / 2, 0);
            boundingRect.adjust(0, 0, 0, -4);

            expandRect(boundingRect, scale);

            if (boundingRect.contains((pos * scale).toPoint())) {
                return qMakePair(&graph.txts[i], i);
            }
        }
        else {
            if (graph.txts[i].type == TypeEdge::EDGE) {
                auto iter = std::find_if(graph.edgs.begin(), graph.edgs.end(), [&](const Edge& edg) { return (edg.idx == graph.txts[i].binding); });
                QPointF textPos = graph.txts[i].coords + (getPlace(graph.verxs[graph.edgs[std::distance(graph.edgs.begin(), iter)].firstIdx].coords)
                                  + getPlace(graph.verxs[graph.edgs[std::distance(graph.edgs.begin(), iter)].secondIdx].coords)) / 2;

                textPos /= scale;

                QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, graph.txts[i].text);
                boundingRect.translate(-boundingRect.width() / 2, 0);
                boundingRect.adjust(0, 0, 0, -4);

                expandRect(boundingRect, scale);

                if (boundingRect.contains(pos.toPoint())) {
                    return qMakePair(&graph.txts[i], i);
                }
            }
            else if (graph.txts[i].type == TypeEdge::ARROW) {
                auto iter = std::find_if(graph.arrs.begin(), graph.arrs.end(), [&](const Edge& arr){ return (arr.idx == graph.txts[i].binding) && (graph.txts[i].type == TypeEdge::ARROW); });
                QPointF textPos = graph.txts[i].coords + (getPlace(graph.verxs[graph.arrs[std::distance(graph.arrs.begin(), iter)].firstIdx].coords)
                                                                   + getPlace(graph.verxs[graph.arrs[std::distance(graph.arrs.begin(), iter)].secondIdx].coords)) / 2;

                textPos /= scale;

                QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, graph.txts[i].text);
                boundingRect.translate(-boundingRect.width() / 2, 0);
                boundingRect.adjust(0, 0, 0, -4);

                expandRect(boundingRect, scale);

                if (boundingRect.contains(pos.toPoint())) {
                    return qMakePair(&graph.txts[i], i);
                }
            }
            else if (graph.txts[i].type == TypeEdge::SELF_ARROW) {
                auto iter = std::find_if(graph.self_arrs.begin(), graph.self_arrs.end(), [&](const SelfEdge& arr){ return (arr.idx == graph.txts[i].binding) && (graph.txts[i].type == TypeEdge::SELF_ARROW); });
                QPointF textPos = graph.txts[i].coords + (getPlace(graph.verxs[graph.self_arrs[std::distance(graph.self_arrs.begin(), iter)].verIdx].coords));

                textPos /= scale;

                QRect boundingRect = fm.boundingRect(textPos.x(), textPos.y() - 10, 0, 0, Qt::AlignLeft, graph.txts[i].text);
                boundingRect.translate(-boundingRect.width() / 2, 0);
                boundingRect.adjust(0, 0, 0, -4);

                expandRect(boundingRect, scale);

                if (boundingRect.contains(pos.toPoint())) {
                    return qMakePair(&graph.txts[i], i);
                }
            }
        }
    }
    return qMakePair(nullptr, -1);
}

void expandRect(QRect& rect, float factor) {
    int newWidth = rect.width() * factor;
    int newHeight = rect.height() * factor;

    int dx = (newWidth - rect.width()) / 2;
    int dy = (newHeight - rect.height()) / 2;

    rect.adjust(-dx, -dy, dx, dy);
}

void CSamField::wheelEvent(QWheelEvent *event) {
    if (ShiftOn) {
        double prev_scale = 1;
        if (event->angleDelta().y() < 0) {
            if (scale < 3) {
                prev_scale = scale;
                scale *= 1.05;
                QPointF dCam = QPoint(width() / 2, height() / 2) * (1 / prev_scale - 1 / scale) * 2;
                coords += dCam.toPoint();
            }
        }
        else {
            if (scale > 1) {
                prev_scale = scale;
                scale /= 1.05;
                QPointF dCam = QPoint(width() / 2, height() / 2) * (1 / prev_scale - 1 / scale) * 2;
                coords += dCam.toPoint();
            }
        }
    }
    else {
        QPair<Vertex*, int> ver = getVertex(mouseTracker);
        if (ver.first != nullptr) {
            if (event->angleDelta().y() < 0 && ver.first->radius > 17) {
                ver.first->radius *= 0.90;  // Уменьшаем радиус на более значительное значение
            }
            else if (event->angleDelta().y() > 0 && ver.first->radius < 50) {
                ver.first->radius *= 1.10;  // Увеличиваем радиус на большее значение
            }

        }
    }
    mouseTracker = event->position().toPoint();
    repaint();
    QWidget::wheelEvent(event);
}

void CSamField::mousePressEvent(QMouseEvent *event) {
    writing = false;
    if (txtInWriting != nullptr && txtInWriting->text == "") {
        txtInWriting->text = "None";
    }
    if(event->buttons() == Qt::RightButton) {
        oldCoords = event->pos();
        rightButtonPressed = true;
    }
    else if(event->buttons() == Qt::LeftButton) {
        unselectAll();
        if (option == Option::VERTEX) {
            if (this->getVertex(event->pos()).first == nullptr) {
                graph.verxs.push_back(Vertex((event->pos() * scale - coords), 20, graph.numVerxs));
                graph.txts.push_back(Text(QString::number(graph.numVerxs++ + 1), QPoint(0, 3), graph.numTexts++, true, graph.verxs[graph.verxs.size() - 1].idx));
                graph.verxs.back().setText(graph.txts.back().idx);
            }
        }
        else if (option == Option::EDGE) {
            if (this->getVertex(event->pos()).first != nullptr && ver1 == -1)
                ver1 = this->getVertex(event->pos()).second;
            else if (this->getVertex(event->pos()).first != nullptr && ver1 != -1) {
                ver2 = this->getVertex(event->pos()).second;
                bool addIt = true;

                for (int i = 0; i < graph.edgs.size(); i++) {
                    if ((graph.edgs[i].firstIdx == ver1 && graph.edgs[i].secondIdx == ver2)
                        || (graph.edgs[i].secondIdx == ver1 && graph.edgs[i].firstIdx == ver2))
                        addIt = false;
                }

                for (int i = 0; i < graph.arrs.size(); i++) {
                    if ((graph.arrs[i].firstIdx == ver1 && graph.arrs[i].secondIdx == ver2)
                        || (graph.arrs[i].secondIdx == ver1 && graph.arrs[i].firstIdx == ver2))
                        addIt = false;
                }

                if (ver1 == ver2) {
                    bool addIt = true;
                    for (int i = 0; i < graph.self_arrs.size(); i++) {
                        if (graph.self_arrs[i].verIdx == ver1)
                            addIt = false;
                    }
                    if (addIt) {
                        graph.self_arrs.push_back(SelfEdge(ver1, graph.numSelfArrs));
                        graph.txts.push_back(Text("   ", QPoint(0, -graph.verxs[ver1].radius * 2.1), graph.numTexts++,
                                                  false, graph.numSelfArrs++, TypeEdge::SELF_ARROW));
                        graph.self_arrs.back().setText(graph.txts.back().idx);
                    }
                }
                else if (addIt) {
                    graph.edgs.push_back(Edge(ver1, ver2, graph.numEdgs));
                    graph.txts.push_back(Text("   ", QPoint(0, 0), graph.numTexts++, false, graph.numEdgs++, TypeEdge::EDGE));
                    graph.edgs.back().setText(graph.txts.back().idx);
                }
                ver1 = -1;
                ver2 = -1;
            }
            else {
                ver1 = -1;
                ver2 = -1;
            }
        }
        else if (option == Option::ARROW) {
            if (this->getVertex(event->pos()).first != nullptr && ver1 == -1)
                ver1 = this->getVertex(event->pos()).second;
            else if (this->getVertex(event->pos()).first != nullptr && ver1 != -1) {
                ver2 = this->getVertex(event->pos()).second;
                bool addIt = true;

                for (int i = 0; i < graph.edgs.size(); i++) {
                    if ((graph.edgs[i].firstIdx == ver1 && graph.edgs[i].secondIdx == ver2)
                        || (graph.edgs[i].secondIdx == ver1 && graph.edgs[i].firstIdx == ver2))
                        addIt = false;
                }
                for (int i = 0; i < graph.arrs.size(); i++) {
                    if (graph.arrs[i].firstIdx == ver1 && graph.arrs[i].secondIdx == ver2)
                        addIt = false;
                }

                if (ver1 == ver2) {
                    bool addIt = true;
                    for (int i = 0; i < graph.self_arrs.size(); i++) {
                        if (graph.self_arrs[i].verIdx == ver1)
                            addIt = false;
                    }
                    if (addIt) {
                        graph.self_arrs.push_back(SelfEdge(ver1, graph.numSelfArrs));
                        graph.txts.push_back(Text("   ", QPoint(0, -graph.verxs[ver1].radius * 2.1), graph.numTexts++,
                                                  false, graph.numSelfArrs++, TypeEdge::SELF_ARROW));
                        graph.self_arrs.back().setText(graph.txts.back().idx);
                    }
                }
                else if (addIt) {
                    graph.arrs.push_back(Edge(ver1, ver2, graph.numArrs));
                    graph.txts.push_back(Text("   ", QPoint(0, 0), graph.numTexts++, false, graph.numArrs++, TypeEdge::ARROW));
                    graph.arrs.back().setText(graph.txts.back().idx);
                }
                ver1 = -1;
                ver2 = -1;
            }
            else {
                ver1 = -1;
                ver2 = -1;
            }
        }
        else if (option == Option::CURSOR) {
            if (verToMove != -1) {
                verToMove = -1;
                unselectAll();
            }
            else if (getVertex(event->pos()).first != nullptr) {
                verToMove = getVertex(event->pos()).second;
                selectVrx(verToMove);
                if (gridBinding) {
                    graph.verxs[verToMove].coords = QPointF(substraD(graph.verxs[verToMove].coords.x() - graph.verxs[verToMove].coords.x(), 5),
                                                            substraD(graph.verxs[verToMove].coords.y() - graph.verxs[verToMove].coords.y(), 5));
                }
            }
        }
        else if (option == Option::TEXT) {
            if (txtToMove != -1) {
                txtToMove = -1;
            }
            else if (getText(event->pos()).first != nullptr) {
                txtToMove = getText(event->pos()).second;
                selectTxt(txtToMove);
            }
        }
        else if (option == Option::CLEAR) {
            QPair<Vertex*, int> vertex = getVertex(event->pos());
            QPair<Text*, int> txt = getText(event->pos());

            if (ShiftOn) {
                graph.edgs.clear();
                graph.arrs.clear();
                graph.verxs.clear();
                graph.txts.clear();
                graph.self_arrs.clear();
                graph.numVerxs = 0;
            }
            else if (vertex.first != nullptr) {
                int vertexIdx = vertex.second;

                for (int i = graph.edgs.size() - 1; i >= 0; i--) {
                    if (graph.edgs[i].firstIdx == vertexIdx || graph.edgs[i].secondIdx == vertexIdx) {
                        auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [&](const Text& txt){ return (txt.idx == graph.edgs[i].textBinding) && (txt.type == TypeEdge::EDGE); });
                        graph.txts.removeAt(std::distance(graph.txts.begin(), iter));
                        graph.edgs.removeAt(i);
                    }
                }

                for (int i = graph.arrs.size() - 1; i >= 0; i--) {
                    if (graph.arrs[i].firstIdx == vertexIdx || graph.arrs[i].secondIdx == vertexIdx) {
                        auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [&](const Text& txt){ return (txt.idx == graph.arrs[i].textBinding) && (txt.type == TypeEdge::ARROW); });
                        graph.txts.removeAt(std::distance(graph.txts.begin(), iter));
                        graph.arrs.removeAt(i);
                    }
                }

                for (int i = graph.self_arrs.size() - 1; i >= 0; i--) {
                    if (graph.self_arrs[i].verIdx == vertexIdx) {
                        auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [&](const Text& txt){ return (txt.idx == graph.self_arrs[i].textBinding) && (txt.type == TypeEdge::SELF_ARROW); });
                        graph.txts.removeAt(std::distance(graph.txts.begin(), iter));
                        graph.self_arrs.removeAt(i);
                    }
                }

                for (int i = graph.txts.size() - 1; i >= 0; i--) {
                    if ((graph.txts[i].binding == graph.verxs[vertexIdx].idx) && graph.txts[i].notLine) {
                        graph.txts.removeAt(i);
                    }
                }

                for (int i = 0; i < (int)graph.edgs.size(); i++) {
                    if (graph.edgs[i].firstIdx >= vertexIdx) graph.edgs[i].firstIdx--;
                    if (graph.edgs[i].secondIdx >= vertexIdx) graph.edgs[i].secondIdx--;
                }

                for (int i = 0; i < (int)graph.arrs.size(); i++) {
                    if (graph.arrs[i].firstIdx >= vertexIdx) graph.arrs[i].firstIdx--;
                    if (graph.arrs[i].secondIdx >= vertexIdx) graph.arrs[i].secondIdx--;
                }

                for (int i = 0; i < (int)graph.self_arrs.size(); i++) {
                    if (graph.self_arrs[i].verIdx >= vertexIdx) graph.self_arrs[i].verIdx--;
                }

                graph.verxs.removeAt(vertexIdx);
            }
            else if (txt.second != -1) {
                if (graph.txts[txt.second].type == TypeEdge::EDGE) {
                    auto iter = std::find_if(graph.edgs.begin(), graph.edgs.end(), [&](const Edge& edg){ return graph.txts[txt.second].binding == edg.idx; });
                    graph.edgs.removeAt(std::distance(graph.edgs.begin(), iter));
                }
                else if (graph.txts[txt.second].type == TypeEdge::ARROW) {
                    auto iter = std::find_if(graph.arrs.begin(), graph.arrs.end(), [&](const Edge& arr){ return graph.txts[txt.second].binding == arr.idx; });
                    graph.arrs.removeAt(std::distance(graph.arrs.begin(), iter));
                }
                else if (graph.txts[txt.second].type == TypeEdge::SELF_ARROW) {
                    auto iter = std::find_if(graph.self_arrs.begin(), graph.self_arrs.end(), [&](const SelfEdge& edg){ return graph.txts[txt.second].binding == edg.idx; });
                    graph.self_arrs.removeAt(std::distance(graph.self_arrs.begin(), iter));
                }
                graph.txts.removeAt(txt.second);

            }
        }
    }
    repaint();
    QWidget::mousePressEvent(event);
}

double substraD(double i, int j) {
    int sub = (int)floor(i);
    return sub % j + i - (double)j;
}

void CSamField::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        rightButtonPressed = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void CSamField::mouseMoveEvent(QMouseEvent *event) {
    mouseTracker = event->pos();
    if (rightButtonPressed) {
        QPointF delta = (event->pos() - oldCoords);
        coords += delta * scale;
        oldCoords = event->pos();
    }
    else if (option == Option::CURSOR && verToMove != -1) {
        QPointF delta = event->pos() * scale - graph.verxs[verToMove].coords - coords;
        graph.verxs[verToMove].coords += delta;
        oldCoords = event->pos();
        if (gridBinding) {
            graph.verxs[verToMove].coords = QPointF(substraD(graph.verxs[verToMove].coords.x() - graph.verxs[verToMove].coords.x(), 20),
                                                    substraD(graph.verxs[verToMove].coords.y() - graph.verxs[verToMove].coords.y(), 20));
        }
    }
    else if (option == Option::TEXT && txtToMove != -1) {
        // Текущая позиция курсора в координатах виджета
        QPointF cursorPosInWidget = event->pos();

        // Вычисляем смещение текста
        if (graph.txts[txtToMove].type == TypeEdge::NONE) {
            auto iter = std::find_if(graph.verxs.begin(), graph.verxs.end(), [&](const Vertex& vrx){ return (graph.txts[txtToMove].binding == vrx.idx) && graph.txts[txtToMove].notLine; });

            QPointF delta;
            graph.txts[txtToMove].binding != -1 ? delta = cursorPosInWidget * scale - coords - graph.txts[txtToMove].coords
                                                        - graph.verxs[std::distance(graph.verxs.begin(), iter)].coords
                                                        : delta = cursorPosInWidget * scale - coords - graph.txts[txtToMove].coords;

            // Перемещаем текст
            graph.txts[txtToMove].coords += delta;
        }
        else if (graph.txts[txtToMove].type == TypeEdge::EDGE) {
            auto iter = std::find_if(graph.edgs.begin(), graph.edgs.end(), [&](const Edge& edg){ return (graph.txts[txtToMove].binding == edg.idx); });

            QPointF delta = cursorPosInWidget * scale - graph.txts[txtToMove].coords -
                (getPlace(graph.verxs[graph.edgs[std::distance(graph.edgs.begin(), iter)].firstIdx].coords)
                + getPlace(graph.verxs[graph.edgs[std::distance(graph.edgs.begin(), iter)].secondIdx].coords)) / 2;

            // Перемещаем текст
            graph.txts[txtToMove].coords += delta;
        }
        else if (graph.txts[txtToMove].type == TypeEdge::ARROW) {
            auto iter = std::find_if(graph.arrs.begin(), graph.arrs.end(), [&](const Edge& arr){ return (graph.txts[txtToMove].binding == arr.idx); });

            QPointF delta = cursorPosInWidget * scale - graph.txts[txtToMove].coords -
                    (getPlace(graph.verxs[graph.arrs[std::distance(graph.arrs.begin(), iter)].firstIdx].coords)
                     + getPlace(graph.verxs[graph.arrs[std::distance(graph.arrs.begin(), iter)].secondIdx].coords)) / 2;

            // Перемещаем текст
            graph.txts[txtToMove].coords += delta;
        }
        else if (graph.txts[txtToMove].type == TypeEdge::SELF_ARROW) {
            auto iter = std::find_if(graph.self_arrs.begin(), graph.self_arrs.end(), [&](const SelfEdge& arr){ return (graph.txts[txtToMove].binding == arr.idx);});

            QPointF delta = cursorPosInWidget * scale - graph.txts[txtToMove].coords -
                           (getPlace(graph.verxs[graph.self_arrs[std::distance(graph.self_arrs.begin(), iter)].verIdx].coords));

            // Перемещаем текст
            graph.txts[txtToMove].coords += delta;
        }

        // Обновляем старую позицию курсора
        oldCoords = event->pos();
    }

    repaint();
    QWidget::mouseMoveEvent(event);
}

void CSamField::mouseDoubleClickEvent(QMouseEvent *event) {
    if (option == Option::TEXT) {
        txtToMove = -1;
        QPair<Text*, int> txt = getText(event->pos());
        if (txt.second == -1) {
            graph.txts.push_back(Text("None", event->pos() * scale - coords, graph.numTexts++, true));
        }
        else {
            txtInWriting = txt.first;
            txtInWriting->text = "";
            writing = true;
        }
        repaint();
    }
}

void CSamField::changeOption(Option opt) {
    option = opt;
}

Option CSamField::getOption() const {
    return option;
}

QPointF CSamField::getPlace(QPointF firstCoords) const {
    return firstCoords + coords;
}

void CSamField::resetVers() {
    ver1 = -1;
    ver2 = -1;
    verToMove = -1;
    txtToMove = -1;
}

void CSamField::setGraph(const SamGraph& graph) {
    this->graph = graph;
}

SamGraph CSamField::getGraph() const {
    return this->graph;
}

double CSamField::getScale() const {
    return this->scale;
}

QPointF CSamField::getCoords() const {
    return this->coords;
}

void CSamField::setCoords(QPointF coords) {
    this->coords = coords;
}

void CSamField::setScale(double scale) {
    this->scale = scale;
}
