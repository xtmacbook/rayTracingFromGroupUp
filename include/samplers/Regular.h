#pragma once

#include "Sampler.h"

class Regular : public Sampler{
public:
    Regular(int samples = 1, int sets = 50);
    virtual Sampler* clone() const;
    virtual void generate_samples();
};