#include "MuteNode.h"

MuteNode::MuteNode()
    {
        std::cout << "MuteNode created\n";
    }
    MuteNode::~MuteNode()
    {
        std::cout << "MuteNode destroyed\n";
    }
    float *MuteNode::process(float *input)
    {
        if (input == nullptr)
        {
            return output;
        }
        if (_muted)
        {
            output[0] = 0;
            output[1] = 0;
        }else{
            output[0] = input[0];
            output[1] = input[1];
        }
        return output;
        
    }