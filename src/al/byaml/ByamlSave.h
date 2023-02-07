#pragma once

#include "ByamlIter.h"
#include "writer/ByamlWriter.h"
#include "al/hio/HioNode.h"

class ByamlSave : public al::HioNode {
public:
    virtual void write(al::ByamlWriter *) = 0;
    virtual void read(al::ByamlIter const &) = 0;
};