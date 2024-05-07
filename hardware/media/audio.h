#pragma once

namespace media {

class IAudio {
public:

    static IAudio& instance();

    virtual bool initial(const int config) = 0;

    virtual bool deInitial() = 0;
};

}