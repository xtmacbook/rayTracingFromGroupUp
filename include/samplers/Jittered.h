#pragma once

#include "Sampler.h"

class Jittered : public Sampler{
public:
    Jittered(int samples = 1, int sets = 50);
    virtual Sampler* clone() const;
    virtual void generate_samples();
};