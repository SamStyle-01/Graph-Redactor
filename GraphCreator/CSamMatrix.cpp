#include "CSamMatrix.h"
#include "CSamFrame.h"

CSamMatrix::CSamMatrix(const SamGraph& graph, CSamFrame *parent) : QWidget{parent} {
    this->setWindowIcon(QIcon("./GraphCreator.ico"));

    frame = parent;
    layout = new QVBoxLayout(this);
    exit = new QPushButton("Ок", this);
    exit->setFont(QFont("Times New Roman", 16));
    exit->setStyleSheet("QPushButton { background-color: rgb(255, 255, 255); border: 2px solid #CC9999;}"
                        "QPushButton:hover { background-color: rgb(150, 200, 251); }"
                        "QPushButton:disabled { background-color: rgb(81, 200, 251); }");
    connect(exit, &QPushButton::clicked, this, [this]() {
        frame->changeOption(Option::CURSOR);
    });
    matrix_viewer = new QTextEdit(this);
    matrix_viewer->setLineWrapMode(QTextEdit::NoWrap);
    matrix_viewer->setStyleSheet("border: 2px solid #CC9999; background-color: rgb(255, 255, 255);");
    matrix_viewer->setFont(QFont("Times New Roman", 12));
    matrix_viewer->setReadOnly(true);
    layout->addWidget(matrix_viewer);
    layout->addWidget(exit);
    this->setLayout(layout);

    this->setMinimumSize(300, 300);
    this->setMaximumHeight(500);

    this->convertData(graph);
    count++;
}

CSamMatrix::~CSamMatrix() {
    count--;
}

int CSamMatrix::count = 0;

void CSamMatrix::convertData(const SamGraph& graph) {
    QString result;

    QVector<QPair<Text, int>> verxs;
    int num = 0;
    for (int i = 0; i < (int)graph.txts.size(); i++) {
        if (graph.txts[i].notLine && graph.txts[i].binding != -1) {
            verxs.append(qMakePair(graph.txts[i], i));
            QString text = graph.txts[i].text;
            text = text.remove(' ');
            if (text.isEmpty()) {
                verxs[num].first.text = "V" + QString::number(num + 1);
            }
            num++;
        }
    }

    QVector<int> verxs_idxs;
    for (int i = 0; i < (int)verxs.size(); i++) {
        auto iter = std::find_if(graph.verxs.begin(), graph.verxs.end(), [&verxs, i](const Vertex& vrx) {
            return (verxs[i].first.binding == vrx.idx);
        });
        verxs_idxs.push_back(std::distance(graph.verxs.begin(), iter));
    }

    if (!verxs.empty()) result += "N, ";
    for (int i = 0; i < (int)verxs.size(); i++) {
        result += verxs[i].first.text + ", ";
    }
    result += "\n";

    QString** matrix = new QString*[verxs.size()];
    for (int i = 0; i < (int)verxs.size(); i++) {
        matrix[i] = new QString[verxs.size()];
        for (int j = 0; j < (int)verxs.size(); j++)
            matrix[i][j] = "◯";
    }

    for (int i = 0; i < (int)graph.edgs.size(); i++) {
        auto iter1 = std::find(verxs_idxs.begin(), verxs_idxs.end(), graph.edgs[i].firstIdx);
        auto iter2 = std::find(verxs_idxs.begin(), verxs_idxs.end(), graph.edgs[i].secondIdx);
        auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [&graph, i](const Text& txt) {
            return (txt.binding == graph.edgs[i].idx) && !txt.notLine && (txt.type == TypeEdge::EDGE);
        });
        QString text = iter->text;
        text = text.remove(' ');
        if (!text.isEmpty()) {
            matrix[*iter1][*iter2] = iter->text;
            matrix[*iter2][*iter1] = iter->text;
        }
        else {
            matrix[*iter1][*iter2] = "∃";
            matrix[*iter2][*iter1] = "∃";
        }
    }

    for (int i = 0; i < (int)graph.arrs.size(); i++) {
        auto iter1 = std::find(verxs_idxs.begin(), verxs_idxs.end(), graph.arrs[i].firstIdx);
        auto iter2 = std::find(verxs_idxs.begin(), verxs_idxs.end(), graph.arrs[i].secondIdx);
        auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [&graph, i](const Text& txt) {
            return (txt.binding == graph.arrs[i].idx) && !txt.notLine && (txt.type == TypeEdge::ARROW);
        });
        QString text = iter->text;
        text = text.remove(' ');
        if (!text.isEmpty())
            matrix[*iter1][*iter2] = iter->text;
        else
            matrix[*iter1][*iter2] = "∃";
    }

    for (int i = 0; i < (int)graph.self_arrs.size(); i++) {
        auto iter1 = std::find(verxs_idxs.begin(), verxs_idxs.end(), graph.self_arrs[i].verIdx);
        auto iter = std::find_if(graph.txts.begin(), graph.txts.end(), [&graph, i](const Text& txt) {
            return (txt.binding == graph.self_arrs[i].idx) && !txt.notLine && (txt.type == TypeEdge::SELF_ARROW);
        });
        QString text = iter->text;
        text = text.remove(' ');
        if (!text.isEmpty()) {
            matrix[*iter1][*iter1] = iter->text;
        }
        else {
            matrix[*iter1][*iter1] = "∃";
        }
    }

    for (int i = 0; i < (int)verxs.size(); i++) {
        QString row;
        for (int j = 0; j < (int)verxs.size(); j++)
            row += matrix[i][j] + ", ";

        result += verxs[i].first.text + ", " + row + "\n";
    }

    matrix_viewer->setText(result);

    for (int i = 0; i < (int)verxs.size(); i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}
