#pragma once

namespace PushBoxes {

class Block {
    public:
    virtual bool isMoveable();
    virtual bool isReplaceable();
    virtual char blockChar() = 0;
};

}  // namespace PushBoxes