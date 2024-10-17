#ifndef CSAMMATRIX_H
#define CSAMMATRIX_H

#include "pch.h"

class CSamFrame;

class CSamMatrix : public QWidget {
    QVBoxLayout* layout;
    QPushButton* exit;
    QTextEdit* matrix_viewer;
    CSamFrame* frame;
public:
    explicit CSamMatrix(const SamGraph& graph, CSamFrame *parent = nullptr);
    ~CSamMatrix();
    void convertData(const SamGraph& graph);
    static int count;
};

#endif
