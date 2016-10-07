//
//  SOM.h
//  TinyBrain
//
//  Created by Tim Omernick on 10/7/16.
//  Copyright © 2016 Tim Omernick. All rights reserved.
//

#ifndef __SOM_H__
#define __SOM_H__

#include "Neuron.h"

#include <vector>
using namespace std;

// Input data is always a vector of floats.
typedef vector<float> Data;

// Label is a vector of arbitrary dimension and data type.
// It could be another float vector, or one or multiple strings, or anything else.
template <typename LabelType>
using Label = vector<LabelType>;

// A pair of data->label.  This is the basic building block of a data set.
template <typename LabelType>
using DataLabelPair = pair<Data, Label<LabelType>>;

// A data set consists of multiple data->label pairs.
template <typename LabelType>
using DataSet = vector<DataLabelPair<LabelType>>;

template<typename LabelType>
class SOM {
public:
    SOM(size_t inputSize, size_t outputSize, size_t numNeurons) : mIterationIndex(0)
    {
        size_t neuronsPerRow = (size_t)sqrtf((float)numNeurons);
        size_t numRows = (size_t)ceilf((float)numNeurons / (float)neuronsPerRow);

        for (uint32_t i = 0; i < numNeurons; i++) {
            uint32_t row = i / neuronsPerRow;
            uint32_t col = (i % neuronsPerRow);
            
            float x = (float)col / (float)neuronsPerRow;
            float y = (float)row / (float)numRows;
            
            Neuron *neuron = new Neuron(inputSize, x, y);
            mNeurons.push_back(neuron);
            
            neuron->RandomizeWeights();
        }
    }

    size_t GetNumNeurons() const
    {
        return mNeurons.size();
    }
    
    Data GetNeuronWeights(size_t neuronIndex) const
    {
        return mNeurons[neuronIndex]->GetWeights();
    }
    
    void SetNeuronWeights(uint32_t neuronIndex, const Data& data)
    {
        mNeurons[neuronIndex]->SetWeights(data);
    }

    void GetNeuronPosition(size_t neuronIndex, float& x, float& y) const
    {
        mNeurons[neuronIndex]->GetPosition(x, y);
    }
    
    float GetDistance(const Data& v0, const Data& v1)
    {
        float sumSq = 0.0f;
        size_t dimension = v0.size();
        for (uint32_t i = 0; i < dimension; i++) {
            float d = v1[i] - v0[i];
            sumSq += (d * d);
        }
        return sumSq;
    }
    
    uint32_t GetBestMatch(const Data& data)
    {
        uint32_t closestNeuronIndex = 0;
        float closestDist = FLT_MAX;
        
        size_t numNeurons = GetNumNeurons();
        for (uint32_t i = 0; i < numNeurons; i++) {
            Data neuronWeights = GetNeuronWeights(i);
            float dist = GetDistance(neuronWeights, data);
            if (dist < closestDist) {
                closestDist = dist;
                closestNeuronIndex = i;
            }
        }
        
        return closestNeuronIndex;
    }
    
    vector<pair<uint32_t, float>> GetNeighborhood(uint32_t neuronIndex, float radiusSq)
    {
        vector<pair<uint32_t, float>> neighborhood;
        
        float neuronX = 0.0f;
        float neuronY = 0.0f;
        GetNeuronPosition(neuronIndex, neuronX, neuronY);

        size_t numNeurons = GetNumNeurons();
        for (uint32_t i = 0; i < numNeurons; i++) {
            float testX = 0.0f;
            float testY = 0.0f;
            GetNeuronPosition(i, testX, testY);
            
            float dx = testX - neuronX;
            float dy = testY - neuronY;
            float distSq = (dx * dx) + (dy * dy);
            if (distSq <= radiusSq) {
                neighborhood.push_back(pair<uint32_t, float>(i, distSq));
            }
        }
        
        return neighborhood;
    }
    
    Data AdjustWeights(const Data& v0, const Data& v1, float t)
    {
        size_t dimension = v0.size();
        
        Data result;
        result.resize(dimension);
        
        for (uint32_t i = 0; i < dimension; i++) {
            result[i] = v0[i] + ((v1[i] - v0[i]) * t);
        }
        
        return result;
    }
    
    // Train the map with the given label type.
    void Train(const DataSet<LabelType>& dataSet)
    {
        const size_t maxIterations = 50000;
        float maxRadius = 0.5f;
        float neighborhoodRadius = maxRadius * exp(-(float)mIterationIndex / maxIterations);
        float neighborhoodRadiusSq = neighborhoodRadius * neighborhoodRadius;
        
        const float startLearningRate = 0.1f;
        float learningRate = startLearningRate * exp(-(float)mIterationIndex / (float)maxIterations);

        if ((mIterationIndex % 1000) == 0) {
            printf("it=%d, radius=%f, learningRate=%f\n", (int)mIterationIndex, neighborhoodRadius, learningRate);
        }

        size_t dataSetSize = dataSet.size();
        for (uint32_t i = 0; i < dataSetSize; i++) {
            uint32_t dataSetIndex = (uint32_t)random() % (uint32_t)dataSetSize;
            Data data = dataSet[dataSetIndex].first;
            
            //Label<LabelType> label = dataSet[i].second;
            
            uint32_t bestMatch = GetBestMatch(data);
            vector<pair<uint32_t, float>> neighborhood = GetNeighborhood(bestMatch, neighborhoodRadiusSq);
            
            for (const auto& neighborPair : neighborhood) {
                uint32_t neuronIndex = neighborPair.first;
                float distToBestSq = neighborPair.second;
                
                float influence = exp(-distToBestSq / (2.0f * neighborhoodRadiusSq)) * learningRate;
                
                Data oldWeights = GetNeuronWeights(neuronIndex);
                
                Data newWeights = AdjustWeights(oldWeights, data, influence);
                
                SetNeuronWeights(neuronIndex, newWeights);
            }
        }
        
        mIterationIndex++;
    }

    // Classify the input, returning the given label type.
    Label<LabelType> Map(const Data& input)
    {
    }
    
private:
    vector<Neuron*> mNeurons;
    size_t mIterationIndex;
};

#endif /* __SOM_H__ */
