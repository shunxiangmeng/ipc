#pragma once

namespace media {

class IImage {
public:

    virtual bool initial(const int config) = 0;

    virtual bool deInitial() = 0;
};

}