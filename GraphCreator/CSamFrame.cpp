#include "CSamFrame.h"

CSamFrame::CSamFrame(QWidget *parent) : QWidget{parent} {
    this->setMinimumSize(800, 500);
    isFullScreen = false;
    matrix = nullptr;
    this->setWindowTitle("Graph Redactor");
    this->setWindowIcon(QIcon("./GraphCreator.ico"));

    Field = new CSamField(this);
    Menu = new QWidget(this);
    Menu->setMaximumWidth(100);
    Menu->setStyleSheet("background-color: #CCCCCC;");

    MainLayout = new QGridLayout(this);
    MenuLayout = new QVBoxLayout(Menu);

    area = new QScrollArea(Menu);
    area->setMaximumWidth(100);
    MainLayout->addWidget(area, 0, 0, 100, 20);
    MainLayout->addWidget(Field, 0, 20, 100, 80);

    area->setWidget(Menu);
    area->setWidgetResizable(true);
    MainLayout->addWidget(area, 0, 0, 1, 1);

    MainLayout->setContentsMargins(0, 0, 0, 0);
    MainLayout->setSpacing(0);
    MenuLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton* Cursor = new QPushButton(this);
    Cursor->setIcon(QIcon("./Cursor.png"));
    Cursor->setMinimumSize(55, 65);
    Cursor->setIconSize(QSize(55, 55));
    Cursor->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                             "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                             "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    connect(Cursor, &QPushButton::clicked, this, [this]() {
        this->changeOption(Option::CURSOR);
    });
    BtnsMenu.push_back(Cursor);

    QPushButton* AddVertex = new QPushButton(this);
    AddVertex->setIcon(QIcon("./AddVertex.png"));
    AddVertex->setMinimumSize(55, 65);
    AddVertex->setIconSize(QSize(55, 55));
    AddVertex->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                           "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                           "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    connect(AddVertex, &QPushButton::clicked, this, [this]() {
        if (Field->getOption() != Option::VERTEX)
            this->changeOption(Option::VERTEX);
        else
            this->changeOption(Option::CURSOR);
    });
    BtnsMenu.push_back(AddVertex);

    QPushButton* AddEdge = new QPushButton(this);
    AddEdge->setIcon(QIcon("./AddEdge.png"));
    AddEdge->setMinimumSize(55, 65);
    AddEdge->setIconSize(QSize(55, 55));
    AddEdge->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                            "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                            "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    connect(AddEdge, &QPushButton::clicked, this, [this]() {
        if (Field->getOption() != Option::EDGE)
            this->changeOption(Option::EDGE);
        else
            this->changeOption(Option::CURSOR);
    });
    BtnsMenu.push_back(AddEdge);

    QPushButton* AddArrow = new QPushButton(this);
    AddArrow->setIcon(QIcon("./AddArrow.png"));
    AddArrow->setMinimumSize(55, 65);
    AddArrow->setIconSize(QSize(55, 55));
    AddArrow->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                            "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                            "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    connect(AddArrow, &QPushButton::clicked, this, [this]() {
        if (Field->getOption() != Option::ARROW)
            this->changeOption(Option::ARROW);
        else
            this->changeOption(Option::CURSOR);
    });
    BtnsMenu.push_back(AddArrow);

    QPushButton* TextBtn = new QPushButton(this);
    TextBtn->setIcon(QIcon("./T.png"));
    TextBtn->setMinimumSize(55, 65);
    TextBtn->setIconSize(QSize(55, 55));
    TextBtn->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                            "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                            "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    connect(TextBtn, &QPushButton::clicked, this, [this]() {
        if (Field->getOption() != Option::TEXT)
            this->changeOption(Option::TEXT);
        else
            this->changeOption(Option::CURSOR);
    });
    BtnsMenu.push_back(TextBtn);

    QPushButton* ClearBtn = new QPushButton(this);
    ClearBtn->setIcon(QIcon("./Clear.png"));
    ClearBtn->setMinimumSize(55, 65);
    ClearBtn->setIconSize(QSize(55, 55));
    ClearBtn->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                           "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                           "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    connect(ClearBtn, &QPushButton::clicked, this, [this]() {
        if (Field->getOption() != Option::CLEAR)
            this->changeOption(Option::CLEAR);
        else
            this->changeOption(Option::CURSOR);
    });
    BtnsMenu.push_back(ClearBtn);

    QPushButton* MatrixBtn = new QPushButton(this);
    MatrixBtn->setIcon(QIcon("./Matrix.png"));
    MatrixBtn->setMinimumSize(55, 65);
    MatrixBtn->setIconSize(QSize(55, 55));
    MatrixBtn->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                            "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                            "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    connect(MatrixBtn, &QPushButton::clicked, this, [this]() {
        if (Field->getOption() != Option::MATRIX)
            this->changeOption(Option::MATRIX);
        else
            this->changeOption(Option::CURSOR);
        this->Field->unselectAll();
    });
    BtnsMenu.push_back(MatrixBtn);

    QPushButton* SaveFile = new QPushButton(this);
    SaveFile->setIcon(QIcon("./SaveFile.png"));
    SaveFile->setMinimumSize(55, 65);
    SaveFile->setIconSize(QSize(55, 55));
    SaveFile->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                            "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                            "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    connect(SaveFile, &QPushButton::clicked, this, [this]() {
        saveFile();
    });
    BtnsMenu.push_back(SaveFile);

    QPushButton* OpenFile = new QPushButton(this);
    OpenFile->setIcon(QIcon("./OpenFile.png"));
    OpenFile->setMinimumSize(55, 65);
    OpenFile->setIconSize(QSize(55, 55));
    OpenFile->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                             "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                             "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    connect(OpenFile, &QPushButton::clicked, this, [this]() {
        openFile();
    });
    BtnsMenu.push_back(OpenFile);

    for (int i = 0; i < BtnsMenu.size(); i++) {
        MenuLayout->addWidget(BtnsMenu[i]);
    }

    Menu->setLayout(MenuLayout);
    this->setLayout(MainLayout);
    changeOption(Option::CURSOR);
}

void CSamFrame::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл", "", "JSON файлы (*.json)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        }

        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc(QJsonDocument::fromJson(data));
        if (doc.isObject()) {
            SamGraph graph;
            QJsonObject jsonObj = doc.object();

            bool stop = false;

            // Общие переменные
            if (jsonObj.contains("numArrs")) {
                graph.numArrs = jsonObj["numArrs"].toInt();
            }
            else stop = true;
            if (jsonObj.contains("numVerxs")) {
                graph.numVerxs = jsonObj["numVerxs"].toInt();
            }
            else stop = true;
            if (jsonObj.contains("numEdgs")) {
                graph.numEdgs = jsonObj["numEdgs"].toInt();
            }
            else stop = true;
            if (jsonObj.contains("numTexts")) {
                graph.numTexts = jsonObj["numTexts"].toInt();
            }
            if (jsonObj.contains("numSelfArrs")) {
                graph.numSelfArrs = jsonObj["numSelfArrs"].toInt();
            }
            else stop = true;
            QPoint newCoords(0, 0);
            if (jsonObj.contains("coords_x")) {
                newCoords.setX(jsonObj["coords_x"].toInt());
            }
            else stop = true;
            if (jsonObj.contains("coords_y")) {
                newCoords.setY(jsonObj["coords_y"].toInt());
            }
            else stop = true;

            // Вершины
            QVector<int> verxs_coords_x;
            QVector<int> verxs_coords_y;
            if (jsonObj.contains("verxs_coords_x")) {
                verxs_coords_x = fromIntJsonToVector(jsonObj["verxs_coords_x"].toArray());
            }
            else stop = true;
            if (jsonObj.contains("verxs_coords_y")) {
                verxs_coords_y = fromIntJsonToVector(jsonObj["verxs_coords_y"].toArray());
            }
            else stop = true;

            QVector<QPoint> verxs_coords;
            for (int i = 0; i < (int)verxs_coords_x.size(); i++) {
                verxs_coords.push_back(QPoint(verxs_coords_x[i], verxs_coords_y[i]));
            }

            QVector<int> verxs_text_binding;
            if (jsonObj.contains("verxs_text_binding")) {
                verxs_text_binding = fromIntJsonToVector(jsonObj["verxs_text_binding"].toArray());
            }
            else stop = true;

            QVector<int> verxs_radius;
            if (jsonObj.contains("verxs_radius")) {
                verxs_radius = fromIntJsonToVector(jsonObj["verxs_radius"].toArray());
            }
            else stop = true;

            QVector<int> verxs_idx;
            if (jsonObj.contains("verxs_idx")) {
                verxs_idx = fromIntJsonToVector(jsonObj["verxs_idx"].toArray());
            }
            else stop = true;

            // Тексты
            QVector<int> txts_coords_x;
            QVector<int> txts_coords_y;
            if (jsonObj.contains("txts_coords_x")) {
                txts_coords_x = fromIntJsonToVector(jsonObj["txts_coords_x"].toArray());
            }
            else stop = true;
            if (jsonObj.contains("txts_coords_y")) {
                txts_coords_y = fromIntJsonToVector(jsonObj["txts_coords_y"].toArray());
            }
            else stop = true;

            QVector<QPoint> txts_coords;
            for (int i = 0; i < (int)txts_coords_x.size(); i++) {
                txts_coords.push_back(QPoint(txts_coords_x[i], txts_coords_y[i]));
            }

            QVector<int> txts_binding;
            if (jsonObj.contains("txts_binding")) {
                txts_binding = fromIntJsonToVector(jsonObj["txts_binding"].toArray());
            }
            else stop = true;

            QVector<int> txts_idx;
            if (jsonObj.contains("txts_idx")) {
                txts_idx = fromIntJsonToVector(jsonObj["txts_idx"].toArray());
            }
            else stop = true;

            QVector<QString> txts_text;
            if (jsonObj.contains("txts_text")) {
                txts_text = fromStringJsonToVector(jsonObj["txts_text"].toArray());
            }
            else stop = true;

            QVector<bool> txts_not_line;
            if (jsonObj.contains("txts_not_line")) {
                txts_not_line = fromBoolJsonToVector(jsonObj["txts_not_line"].toArray());
            }
            else stop = true;

            QVector<TypeEdge> txts_types;
            if (jsonObj.contains("txts_types")) {
                txts_types = fromEnumJsonToVector(jsonObj["txts_types"].toArray());
            }
            else stop = true;

            // Рёбра неориентированные
            QVector<int> edgs_first;
            if (jsonObj.contains("edgs_first")) {
                edgs_first = fromIntJsonToVector(jsonObj["edgs_first"].toArray());
            }
            else stop = true;

            QVector<int> edgs_second;
            if (jsonObj.contains("edgs_second")) {
                edgs_second = fromIntJsonToVector(jsonObj["edgs_second"].toArray());
            }
            else stop = true;

            QVector<int> edgs_idx;
            if (jsonObj.contains("edgs_idx")) {
                edgs_idx = fromIntJsonToVector(jsonObj["edgs_idx"].toArray());
            }
            else stop = true;

            QVector<int> edgs_text_binding;
            if (jsonObj.contains("edgs_text_binding")) {
                edgs_text_binding = fromIntJsonToVector(jsonObj["edgs_text_binding"].toArray());
            }
            else stop = true;

            // Рёбра ориентированные
            QVector<int> arrs_first;
            if (jsonObj.contains("arrs_first")) {
                arrs_first = fromIntJsonToVector(jsonObj["arrs_first"].toArray());
            }
            else stop = true;

            QVector<int> arrs_second;
            if (jsonObj.contains("arrs_second")) {
                arrs_second = fromIntJsonToVector(jsonObj["arrs_second"].toArray());
            }
            else stop = true;

            QVector<int> arrs_idx;
            if (jsonObj.contains("arrs_idx")) {
                arrs_idx = fromIntJsonToVector(jsonObj["arrs_idx"].toArray());
            }
            else stop = true;

            QVector<int> arrs_text_binding;
            if (jsonObj.contains("arrs_text_binding")) {
                arrs_text_binding = fromIntJsonToVector(jsonObj["arrs_text_binding"].toArray());
            }
            else stop = true;

            // Рёбра в те же вершины
            QVector<int> self_arrs_idxs;
            if (jsonObj.contains("self_arrs_idxs")) {
                self_arrs_idxs = fromIntJsonToVector(jsonObj["self_arrs_idxs"].toArray());
            }
            else stop = true;

            QVector<int> self_arrs_ver_idxs;
            if (jsonObj.contains("self_arrs_ver_idxs")) {
                self_arrs_ver_idxs = fromIntJsonToVector(jsonObj["self_arrs_ver_idxs"].toArray());
            }
            else stop = true;

            QVector<int> self_arrs_text_bindings;
            if (jsonObj.contains("self_arrs_text_bindings")) {
                self_arrs_text_bindings = fromIntJsonToVector(jsonObj["self_arrs_text_bindings"].toArray());
            }
            else stop = true;

            // Общие векторы объектов
            QVector<Vertex> verxs_final;
            for (int i = 0; i < (int)verxs_coords.size(); i++) {
                verxs_final.append(Vertex(verxs_coords[i], verxs_radius[i], verxs_idx[i]));
                verxs_final[i].setText(verxs_text_binding[i]);
            }

            QVector<Text> txts_final;
            for (int i = 0; i < (int)txts_text.size(); i++) {
                txts_final.append(Text(txts_text[i], txts_coords[i], txts_idx[i], txts_not_line[i], txts_binding[i], txts_types[i]));
            }

            QVector<Edge> edgs_final;
            for (int i = 0; i < (int)edgs_idx.size(); i++) {
                edgs_final.append(Edge(edgs_first[i], edgs_second[i], edgs_idx[i]));
                edgs_final[i].setText(edgs_text_binding[i]);
            }

            QVector<Edge> arrs_final;
            for (int i = 0; i < (int)arrs_idx.size(); i++) {
                arrs_final.append(Edge(arrs_first[i], arrs_second[i], arrs_idx[i]));
                arrs_final[i].setText(arrs_text_binding[i]);
            }

            QVector<SelfEdge> self_arrs_final;
            for (int i = 0; i < (int)self_arrs_idxs.size(); i++) {
                self_arrs_final.append(SelfEdge(self_arrs_ver_idxs[i], self_arrs_idxs[i]));
                self_arrs_final[i].setText(self_arrs_text_bindings[i]);
            }

            graph.verxs = verxs_final;
            graph.arrs = arrs_final;
            graph.edgs = edgs_final;
            graph.txts = txts_final;
            graph.self_arrs = self_arrs_final;

            if (!stop) {
                if (jsonObj.contains("scale")) {
                    Field->resetOptions();
                    Field->setScale(jsonObj["scale"].toDouble());
                    Field->setGraph(graph);
                    Field->setCoords(newCoords);
                }
                else {
                    QMessageBox::warning(this, "Ошибка", "Не удалось считать файл");
                }
            }
            else {
                QMessageBox::warning(this, "Ошибка", "Не удалось считать файл");
            }
            this->Field->unselectAll();
        }
    }
}

void CSamFrame::saveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить файл", "", "JSON файлы (*.json)");
    if (!fileName.isEmpty()) {
        QJsonObject saveDoc;
        saveDoc["scale"] = Field->getScale();
        saveDoc["coords_x"] = Field->getCoords().x();
        saveDoc["coords_y"] = Field->getCoords().y();
        saveDoc["numVerxs"] = Field->getGraph().numVerxs;
        saveDoc["numEdgs"] = Field->getGraph().numEdgs;
        saveDoc["numArrs"] = Field->getGraph().numArrs;
        saveDoc["numTexts"] = Field->getGraph().numTexts;
        saveDoc["numSelfArrs"] = Field->getGraph().numSelfArrs;

        QJsonArray verxs_coords_x;
        for (int i = 0; i < (int)Field->getGraph().verxs.size(); i++) {
            verxs_coords_x.append(Field->getGraph().verxs[i].coords.x());
        }
        saveDoc["verxs_coords_x"] = verxs_coords_x;

        QJsonArray verxs_coords_y;
        for (int i = 0; i < (int)Field->getGraph().verxs.size(); i++) {
            verxs_coords_y.append(Field->getGraph().verxs[i].coords.y());
        }
        saveDoc["verxs_coords_y"] = verxs_coords_y;

        QJsonArray verxs_radius;
        for (int i = 0; i < (int)Field->getGraph().verxs.size(); i++) {
            verxs_radius.append(Field->getGraph().verxs[i].radius);
        }
        saveDoc["verxs_radius"] = verxs_radius;

        QJsonArray verxs_idx;
        for (int i = 0; i < (int)Field->getGraph().verxs.size(); i++) {
            verxs_idx.append(Field->getGraph().verxs[i].idx);
        }
        saveDoc["verxs_idx"] = verxs_idx;

        QJsonArray verxs_text_binding;
        for (int i = 0; i < (int)Field->getGraph().verxs.size(); i++) {
            verxs_text_binding.append(Field->getGraph().verxs[i].textBinding);
        }
        saveDoc["verxs_text_binding"] = verxs_text_binding;

        QJsonArray edgs_first;
        for (int i = 0; i < (int)Field->getGraph().edgs.size(); i++) {
            edgs_first.append(Field->getGraph().edgs[i].firstIdx);
        }
        saveDoc["edgs_first"] = edgs_first;

        QJsonArray edgs_second;
        for (int i = 0; i < (int)Field->getGraph().edgs.size(); i++) {
            edgs_second.append(Field->getGraph().edgs[i].secondIdx);
        }
        saveDoc["edgs_second"] = edgs_second;

        QJsonArray edgs_idx;
        for (int i = 0; i < (int)Field->getGraph().edgs.size(); i++) {
            edgs_idx.append(Field->getGraph().edgs[i].idx);
        }
        saveDoc["edgs_idx"] = edgs_idx;

        QJsonArray edgs_text_binding;
        for (int i = 0; i < (int)Field->getGraph().edgs.size(); i++) {
            edgs_text_binding.append(Field->getGraph().edgs[i].textBinding);
        }
        saveDoc["edgs_text_binding"] = edgs_text_binding;

        QJsonArray arrs_first;
        for (int i = 0; i < (int)Field->getGraph().arrs.size(); i++) {
            arrs_first.append(Field->getGraph().arrs[i].firstIdx);
        }
        saveDoc["arrs_first"] = arrs_first;

        QJsonArray arrs_second;
        for (int i = 0; i < (int)Field->getGraph().arrs.size(); i++) {
            arrs_second.append(Field->getGraph().arrs[i].secondIdx);
        }
        saveDoc["arrs_second"] = arrs_second;

        QJsonArray arrs_idx;
        for (int i = 0; i < (int)Field->getGraph().arrs.size(); i++) {
            arrs_idx.append(Field->getGraph().arrs[i].idx);
        }
        saveDoc["arrs_idx"] = arrs_idx;

        QJsonArray arrs_text_binding;
        for (int i = 0; i < (int)Field->getGraph().arrs.size(); i++) {
            arrs_text_binding.append(Field->getGraph().arrs[i].textBinding);
        }
        saveDoc["arrs_text_binding"] = arrs_text_binding;

        QJsonArray self_arrs_idxs;
        for (int i = 0; i < (int)Field->getGraph().self_arrs.size(); i++) {
            self_arrs_idxs.append(Field->getGraph().self_arrs[i].idx);
        }
        saveDoc["self_arrs_idxs"] = self_arrs_idxs;

        QJsonArray self_arrs_ver_idxs;
        for (int i = 0; i < (int)Field->getGraph().self_arrs.size(); i++) {
            self_arrs_ver_idxs.append(Field->getGraph().self_arrs[i].verIdx);
        }
        saveDoc["self_arrs_ver_idxs"] = self_arrs_ver_idxs;

        QJsonArray self_arrs_text_bindings;
        for (int i = 0; i < (int)Field->getGraph().self_arrs.size(); i++) {
            self_arrs_text_bindings.append(Field->getGraph().self_arrs[i].textBinding);
        }
        saveDoc["self_arrs_text_bindings"] = self_arrs_text_bindings;

        QJsonArray txts_text;
        for (int i = 0; i < (int)Field->getGraph().txts.size(); i++) {
            txts_text.append(Field->getGraph().txts[i].text);
        }
        saveDoc["txts_text"] = txts_text;

        QJsonArray txts_coords_x;
        for (int i = 0; i < (int)Field->getGraph().txts.size(); i++) {
            txts_coords_x.append(Field->getGraph().txts[i].coords.x());
        }
        saveDoc["txts_coords_x"] = txts_coords_x;

        QJsonArray txts_coords_y;
        for (int i = 0; i < (int)Field->getGraph().txts.size(); i++) {
            txts_coords_y.append(Field->getGraph().txts[i].coords.y());
        }
        saveDoc["txts_coords_y"] = txts_coords_y;

        QJsonArray txts_idx;
        for (int i = 0; i < (int)Field->getGraph().txts.size(); i++) {
            txts_idx.append(Field->getGraph().txts[i].idx);
        }
        saveDoc["txts_idx"] = txts_idx;

        QJsonArray txts_binding;
        for (int i = 0; i < (int)Field->getGraph().txts.size(); i++) {
            txts_binding.append(Field->getGraph().txts[i].binding);
        }
        saveDoc["txts_binding"] = txts_binding;

        QJsonArray txts_not_line;
        for (int i = 0; i < (int)Field->getGraph().txts.size(); i++) {
            txts_not_line.append(Field->getGraph().txts[i].notLine);
        }
        saveDoc["txts_not_line"] = txts_not_line;

        QJsonArray txts_types;
        for (int i = 0; i < (int)Field->getGraph().txts.size(); i++) {
            txts_types.append(Field->getGraph().txts[i].type == TypeEdge::NONE ? "None"
                              : Field->getGraph().txts[i].type == TypeEdge::EDGE ? "Edge"
                              : Field->getGraph().txts[i].type == TypeEdge::ARROW ? "Arrow" : "SelfArrow");
        }
        saveDoc["txts_types"] = txts_types;

        QJsonDocument finalDoc(saveDoc);

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи");
        }
        else {
            Field->resetOptions();
            file.write(finalDoc.toJson());
            file.close();
            QMessageBox::information(this, "Успешно", "Файл был сохранён");
        }
        this->Field->unselectAll();
    }
}

void CSamFrame::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F11) {
        if (isFullScreen) {
            showNormal();
        } else {
            showFullScreen();
        }
        isFullScreen = !isFullScreen;
    }
    QWidget::keyPressEvent(event);
}

void CSamFrame::changeOption(Option opt) {
    if (opt != Option::MATRIX && CSamMatrix::count)
        Field->setEnabled(true);
    if (CSamMatrix::count)
        delete matrix;
    this->Field->unselectAll();
    this->Field->resetWriting();

    Field->changeOption(opt);
    Field->resetVers();
    if (opt == Option::VERTEX) {
        for (int i = 0; i < (int)BtnsMenu.size(); i++)
            BtnsMenu[i]->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                                       "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                                       "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
        BtnsMenu[1]->setStyleSheet("QPushButton { border: none; background-color: rgba(100, 200, 170, 180); border-radius: 15px; }"
                                 "QPushButton:hover { background-color: rgba(135, 235, 205, 180); }"
                                 "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    }
    else if (opt == Option::EDGE) {
        for (int i = 0; i < (int)BtnsMenu.size(); i++)
            BtnsMenu[i]->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                                       "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                                       "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
        BtnsMenu[2]->setStyleSheet("QPushButton { border: none; background-color: rgba(100, 200, 170, 180); border-radius: 15px; }"
                                   "QPushButton:hover { background-color: rgba(135, 235, 205, 180); }"
                                   "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    }
    else if (opt == Option::ARROW) {
        for (int i = 0; i < (int)BtnsMenu.size(); i++)
            BtnsMenu[i]->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                                       "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                                       "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
        BtnsMenu[3]->setStyleSheet("QPushButton { border: none; background-color: rgba(100, 200, 170, 180); border-radius: 15px; }"
                                   "QPushButton:hover { background-color: rgba(135, 235, 205, 180); }"
                                   "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    }
    else if (opt == Option::TEXT) {
        for (int i = 0; i < (int)BtnsMenu.size(); i++)
            BtnsMenu[i]->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                                       "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                                       "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
        BtnsMenu[4]->setStyleSheet("QPushButton { border: none; background-color: rgba(100, 200, 170, 180); border-radius: 15px; }"
                                   "QPushButton:hover { background-color: rgba(135, 235, 205, 180); }"
                                   "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    }
    else if (opt == Option::CLEAR) {
        for (int i = 0; i < (int)BtnsMenu.size(); i++)
            BtnsMenu[i]->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                                       "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                                       "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
        BtnsMenu[5]->setStyleSheet("QPushButton { border: none; background-color: rgba(100, 200, 170, 180); border-radius: 15px; }"
                                   "QPushButton:hover { background-color: rgba(135, 235, 205, 180); }"
                                   "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    }
    else if (opt == Option::MATRIX) {
        if (!CSamMatrix::count) {
            matrix = new CSamMatrix(Field->getGraph(), this);
            matrix->show();
            this->MainLayout->addWidget(matrix, 0, 40, 30, 40);
        }
        else delete matrix;
        Field->setDisabled(true);
        for (int i = 0; i < (int)BtnsMenu.size(); i++)
            BtnsMenu[i]->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                                       "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                                       "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
        BtnsMenu[6]->setStyleSheet("QPushButton { border: none; background-color: rgba(100, 200, 170, 180); border-radius: 15px; }"
                                   "QPushButton:hover { background-color: rgba(135, 235, 205, 180); }"
                                   "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    }
    else {
        for (int i = 0; i < (int)BtnsMenu.size(); i++)
            BtnsMenu[i]->setStyleSheet("QPushButton { border: none; background-color: none; border-radius: 15px; }"
                                   "QPushButton:hover { background-color: rgba(150, 200, 251, 180); }"
                                   "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
        BtnsMenu[0]->setStyleSheet("QPushButton { border: none; background-color: rgba(100, 200, 170, 180); border-radius: 15px; }"
                                   "QPushButton:hover { background-color: rgba(135, 235, 205, 180); }"
                                   "QPushButton:disabled { background-color: rgba(81, 200, 251, 180); }");
    }
    Field->repaint();
    Field->setFocus();
}

QVector<int> fromIntJsonToVector(QJsonArray arr) {
    QVector<int> result;
    for (int i = 0; i < (int)arr.size(); i++) {
        result.push_back(arr[i].toInt());
    }
    return result;
}

QVector<bool> fromBoolJsonToVector(QJsonArray arr) {
    QVector<bool> result;
    for (int i = 0; i < (int)arr.size(); i++) {
        result.push_back(arr[i].toBool());
    }
    return result;
}

QVector<QString> fromStringJsonToVector(QJsonArray arr) {
    QVector<QString> result;
    for (int i = 0; i < (int)arr.size(); i++) {
        result.push_back(arr[i].toString());
    }
    return result;
}

QVector<TypeEdge> fromEnumJsonToVector(QJsonArray arr) {
    QVector<TypeEdge> result;
    for (int i = 0; i < (int)arr.size(); i++) {
        result.push_back(arr[i] == "None" ? TypeEdge::NONE : arr[i] == "Edge" ? TypeEdge::EDGE : arr[i] == "Arrow" ? TypeEdge::ARROW : TypeEdge::SELF_ARROW);
    }
    return result;
}
