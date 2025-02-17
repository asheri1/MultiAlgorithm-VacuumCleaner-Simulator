#ifndef ALGO_REGISTRATION__
#define ALGO_REGISTRATION__

#include<utility>
#include<string>

#include "../common/AlgorithmRegistrar.h"

struct AlgorithmRegistration {
    AlgorithmRegistration(const std::string& name, AlgorithmFactory algorithmFactory) {
        AlgorithmRegistrar::getAlgorithmRegistrar().registerAlgorithm(name, std::move(algorithmFactory));
    }
};

#define REGISTER_ALGORITHM(ALGO) AlgorithmRegistration \
   _##ALGO(#ALGO, []{return std::make_unique<ALGO>();})

#endif  // ALGO_REGISTRATION__
