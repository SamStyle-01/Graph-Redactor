#ifndef SAMGRAPH_H
#define SAMGRAPH_H

enum class TypeEdge {
    NONE,
    EDGE,
    ARROW,
    SELF_ARROW
};

struct Text;

struct Vertex {
    Vertex() : coords(QPoint(0, 0)), radius(0), idx(-1), textBinding(-1) {}
    Vertex(QPoint coords, int radius, int idx) : coords(coords), radius(radius), idx(idx), textBinding(-1) {}
    void setText(int textBinding);

    QPoint coords;
    int radius;
    int idx;
    int textBinding;
};

struct Edge {
    Edge(int ver1idx, int ver2idx, int index) : firstIdx(ver1idx), secondIdx(ver2idx), idx(index) {}
    void setText(int textBinding);
    int firstIdx;
    int secondIdx;
    int idx;
    int textBinding;
};

struct SelfEdge {
    SelfEdge(int ver_idx, int index) : verIdx(ver_idx), idx(index) {}
    void setText(int textBinding);
    int verIdx;
    int idx;
    int textBinding;
};

struct Text {
    Text(QString text, QPoint coords, int idx, bool notLine, int binding = -1, TypeEdge type = TypeEdge::NONE)
        : text(text), coords(coords), idx(idx), binding(binding), notLine(notLine), type(type) {}
    QString text;
    QPoint coords;
    int idx;
    int binding;
    bool notLine;
    TypeEdge type;
};

struct SamGraph {
    SamGraph() : verxs(QVector<Vertex>()) {}
    QVector<Vertex> verxs;
    QVector<Edge> edgs;
    QVector<Edge> arrs;
    QVector<Text> txts;
    QVector<SelfEdge> self_arrs;
    static int numVerxs;
    static int numEdgs;
    static int numArrs;
    static int numTexts;
    static int numSelfArrs;
};

#endif
