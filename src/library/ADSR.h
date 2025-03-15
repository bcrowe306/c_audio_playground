
//
//  ADRS.h
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the ADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//

#pragma once

#include <stdio.h>
#include <string>
#include <functional>
enum envState
{
    env_idle = 0,
    env_attack,
    env_decay,
    env_sustain,
    env_release
};

class ADSR
{
public:
    ADSR(void);
    ~ADSR(void);
    float process(void);
    float process(int sampleCount);
    float getOutput(void);
    int getState(void);
    void gate(int on);

    // Set the attack rate in samples. This is the time taken to attack to the sustain level.
    // @param rate: float. Should be time represented in samples. To convert from time in seconds to samples, multiply by the sample rate.
    void setAttackRate(float rate);

    // Set the decay rate in samples. This is the time taken to decay to the
    // sustain level.
    // @param rate: float. Should be time represented in samples. To convert
    // from time in seconds to samples, multiply by the sample rate.
    void setDecayRate(float rate);

    // Set the release rate in samples. This is the time taken to release to the
    // sustain level.
    // @param rate: float. Should be time represented in samples. To convert
    // from time in seconds to samples, multiply by the sample rate.
    void setReleaseRate(float rate);

    // setSustainLevel takes a float from 0.0 to 1.0. Any value outside this range will be clamped.
    void setSustainLevel(float level);

    // This sets the curve of the attack rate. (small number such as 0.0001 to
    // 0.01 for mostly-exponential, large numbers like 100 for virtually linear)
    void setTargetRatioA(float targetRatio);

    // This sets the curve of the decay rate. (small number such as 0.0001 to
    // 0.01 for mostly-exponential, large numbers like 100 for virtually linear)
    void setTargetRatioDR(float targetRatio);

    // setAmount multiplies the output of the envelope by the amount.
    // @param amount: float from -1.0 to 1.0. Any value outside this range will be clamped.
    void setAmount(float amount);

    void setOnStateChangedCallback(std::function<void(int)> callback);

    void reset(void);

protected:
    int state;
    float output;
    float attackRate;
    float decayRate;
    float releaseRate;
    float attackCoef;
    float decayCoef;
    float releaseCoef;
    float sustainLevel;
    float targetRatioA;
    float targetRatioDR;
    float attackBase;
    float decayBase;
    float releaseBase;
    float amount = 1.0;
    std::string name;
    float calcCoef(float rate, float targetRatio);
    void set_state(int state);
    std::function<void(int)> on_state_changed;
};

inline float ADSR::process()
{
    switch (state)
    {
    case env_idle:
        break;
    case env_attack:
        output = attackBase + output * attackCoef;
        if (output >= 1.0)
        {
            output = 1.0;
            set_state(env_decay);
        }
        break;
    case env_decay:
        output = decayBase + output * decayCoef;
        if (output <= sustainLevel)
        {
            output = sustainLevel;
            set_state(env_sustain);
        }
        break;
    case env_sustain:
        break;
    case env_release:
        output = releaseBase + output * releaseCoef;
        if (output <= 0.0)
        {
            output = 0.0;
            set_state(env_idle);
        }
        break;
    }
    return output;
}

inline float ADSR::process(int sampleCount)
{
    float retVal = 0;

    if (state != env_idle)
    {
        for (int i = 0; i < sampleCount; i++)
            retVal = process();
    }

    return retVal;
}

inline void ADSR::gate(int gate)
{

    if (gate)
        set_state(env_attack);
    else if (state != env_idle)
        set_state(env_release);
}

inline int ADSR::getState()
{
    return state;
}

inline void ADSR::reset()
{
    set_state(env_idle);
    output = 0.0;
}

inline float ADSR::getOutput()
{
    return output * amount;
}
