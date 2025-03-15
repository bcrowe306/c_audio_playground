#pragma once
#include <iostream>
#include "core/AudioNode.h"
class MuteNode : public AudioNode
{
public:
    MuteNode();
    ~MuteNode();
    float *process(float *input) override;
protected:
    bool _muted = false;
};