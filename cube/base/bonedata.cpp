#include "bonedata.h"

#include <assert.h>
BoneData::BoneData()
{
    for(int i =0 ;i<MAX_BONES ;i++)
    {
        this->IDs[i]=0;
        this->weights[i]=0;
    }
}


void BoneData::addBoneData(int id, float weight)
{
    for (int i = 0 ; i < MAX_BONES ; i++) {
        if (weights[i] == 0.0) {
            IDs[i]     = id;
            weights[i] = weight;
            return;
        }
    }

    // should never get here - more bones than we have space for
    assert(0);
}
