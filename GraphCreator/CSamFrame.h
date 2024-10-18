#ifndef CSAMFRAME_H
#define CSAMFRAME_H

#include "pch.h"
#include "CSamField.h"
#include "CSamMatrix.h"

class CSamFrame : public QWidget {
    bool isFullScreen;
    void keyPressEvent(QKeyEvent *event) override;
    QGridLayout* MainLayout;
    QVBoxLayout* MenuLayout;
    CSamField* Field;
    QWidget* Menu;
    QScrollArea* area;
    QVector<QPushButton*> BtnsMenu;
    CSamMatrix* matrix;
public:
    void saveFile();
    void openFile();
    explicit CSamFrame(QWidget *parent = nullptr);
    void changeOption(Option opt);
};

QVector<int> fromIntJsonToVector(QJsonArray arr);
QVector<double> fromDoubleJsonToVector(QJsonArray arr);
QVector<bool> fromBoolJsonToVector(QJsonArray arr);
QVector<TypeEdge> fromEnumJsonToVector(QJsonArray arr);
QVector<QString> fromStringJsonToVector(QJsonArray arr);

#endif // CSAMFRAME_H
