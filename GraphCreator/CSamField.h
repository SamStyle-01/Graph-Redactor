#ifndef CSAMFIELD_H
#define CSAMFIELD_H

#include "pch.h"

enum class Option {
    CURSOR,
    VERTEX,
    EDGE,
    ARROW,
    TEXT,
    CLEAR,
    MATRIX
};

class CSamField : public QWidget {
    void drawGrid(QPainter* painter) const;
    void drawVrxs(QPainter* painter);
    void drawEdgs(QPainter* painter);
    void drawArrows(QPainter* painter);
    void drawTxts(QPainter* painter);
    void drawSelfArrows(QPainter* painter);
    void drawSelected(QPainter* painter);
    void selectVrx(int idx);
    void selectOnlyVrx(int idx);
    void selectEdge(int idx);
    void selectArrow(int idx);
    void selectSelfEdge(int idx);
    void selectTxt(int idx);
    double scale;
    bool rightButtonPressed;
    bool leftButtonPressed;
    QPoint coords;
    QPoint oldCoords;
    Option option;
    SamGraph graph;
    int ver1;
    int ver2;
    QPoint mouseTracker;
    int verToMove;
    int txtToMove;
    bool ShiftOn;
    bool GOn;
    bool ctrlOn;
    bool writing;
    bool gridBinding;
    Text* txtInWriting;

    QVector<Edge*> selectedEdges;
    QVector<Edge*> selectedArrows;
    QVector<Vertex*> selectedVrxs;
    QVector<Text*> selectedTxts;
    QVector<SelfEdge*> selectedSelfEdges;

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
public:
    void unselectAll();
    void changeOption(Option opt);
    Option getOption() const;
    QPoint getPlace(QPoint firstCoords) const;
    QPair<Vertex*, int> getVertex(QPoint pos);
    QPair<Text*, int> getText(QPoint pos);
    void resetVers();
    explicit CSamField(QWidget *parent = nullptr);
    void setGraph(const SamGraph& graph);
    SamGraph getGraph() const;
    double getScale() const;
    void setScale(double scale);
    void setCoords(QPoint coords);
    QPoint getCoords() const;
    void resetOptions();
    void resetWriting();
};

void expandRect(QRect& rect, float factor);

#endif // CSAMFIELD_H
