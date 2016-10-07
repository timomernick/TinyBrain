//
//  Neuron.h
//  TinyBrain
//
//  Created by Tim Omernick on 10/7/16.
//  Copyright © 2016 Tim Omernick. All rights reserved.
//

#ifndef __NEURON_H__
#define __NEURON_H__

#include <vector>

static inline float _randomFloat()
{
    return (float)random() / (float)RAND_MAX;
}

class Neuron {
public:
    Neuron(size_t inputSize, float x, float y) : mX(x), mY(y)
    {
        mWeights.resize(inputSize);
    }
    
    void RandomizeWeights()
    {
        for (uint32_t i = 0; i < mWeights.size(); i++) {
            mWeights[i] = _randomFloat();
        }
    }
    
    std::vector<float> GetWeights() const
    {
        return mWeights;
    }
    
    void SetWeights(const std::vector<float>& newWeights)
    {
        mWeights = newWeights;
    }
    
    void GetPosition(float& x, float& y) const
    {
        x = mX;
        y = mY;
    }
    
private:
    float mX, mY;
    std::vector<float> mWeights;
};

#endif /* __NEURON_H__ */
