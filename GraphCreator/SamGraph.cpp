#include "SamGraph.h"

int SamGraph::numVerxs = 0;

int SamGraph::numEdgs = 0;

int SamGraph::numArrs = 0;

int SamGraph::numTexts = 0;

int SamGraph::numSelfArrs = 0;

void Edge::setText(int textBinding) {
    this->textBinding = textBinding;
}

void SelfEdge::setText(int textBinding) {
    this->textBinding = textBinding;
}

void Vertex::setText(int textBinding) {
    this->textBinding = textBinding;
}
