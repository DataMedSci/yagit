/********************************************************************************************
 * Copyright (C) 2023 'Yet Another Gamma Index Tool' Developers.
 * 
 * This file is part of 'Yet Another Gamma Index Tool'.
 * 
 * 'Yet Another Gamma Index Tool' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * 'Yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'Yet Another Gamma Index Tool'.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************************************/
#pragma once

#include <vector>
#include <algorithm>

#include "yagit/ImageData.hpp"
#include "yagit/GammaParameters.hpp"

#include "GammaCommon.hpp"

#include "LoadBalancingQueue.hpp"

namespace yagit{

namespace{
std::vector<std::pair<size_t, size_t>> generateCalcRanges(uint32_t nrOfRanges, size_t nrOfCalcs,
                                                          const std::vector<float>& gammaVals){
    std::vector<std::pair<size_t, size_t>> result;
    result.reserve(nrOfRanges);

    size_t nrOfCalcsPerThread = nrOfCalcs / nrOfRanges;
    uint32_t nrOfCalcsPerThreadRemainder = nrOfCalcs % nrOfRanges;
    size_t startIndex = 0;
    size_t endIndex = 0;

    if(nrOfCalcs == gammaVals.size()){  // gammaVals doesn't contain NaNs
        for(uint32_t i = 0; i < nrOfRanges; i++){
            const size_t amount = nrOfCalcsPerThread + (i < nrOfCalcsPerThreadRemainder);
            endIndex += amount;
            result.emplace_back(startIndex, endIndex);
            startIndex = endIndex;
        }
    }
    else{  // gammaVals contains NaNs
        for(uint32_t i = 0; i < nrOfRanges; i++){
            size_t counter = 0;
            const size_t amount = nrOfCalcsPerThread + (i < nrOfCalcsPerThreadRemainder);
            while(counter < amount){
                if(gammaVals[endIndex] == Inf){
                    counter++;
                    if(counter == 1){
                        startIndex = endIndex;
                    }
                }
                endIndex++;
            }
            result.emplace_back(startIndex, endIndex);
            startIndex = endIndex;
        }
    }
    return result;
}

template <typename Function, typename... Args>
std::vector<float> multithreadedGammaIndex(const ImageData& refImg, const GammaParameters& gammaParams,
                                           Function&& func, Args&&... args){
    std::vector<float> gammaVals;
    gammaVals.reserve(refImg.size());

    size_t nrOfCalcs = 0;
    const bool isLocal = gammaParams.normalization == GammaNormalization::Local;
    // preprocess gammaVals
    for(size_t i = 0; i < refImg.size(); i++){
        float doseRef = refImg.get(i);
        bool doseBelowCutoff = doseRef < gammaParams.doseCutoff;
        bool divisionByZero = isLocal && doseRef == 0;
        if(doseBelowCutoff || divisionByZero){
            gammaVals.emplace_back(NaN);
        }
        else{
            gammaVals.emplace_back(Inf);
            nrOfCalcs++;
        }
    }

    const uint32_t nrOfThreads = static_cast<uint32_t>(
        std::min(static_cast<size_t>(std::thread::hardware_concurrency()), refImg.size()));
    if(nrOfThreads > 1){  // multi-threaded
        std::vector<std::thread> threads;
        threads.reserve(nrOfThreads);

        for(const auto& range : generateCalcRanges(nrOfThreads, nrOfCalcs, gammaVals)){
            threads.emplace_back(func, std::cref(args)...,
                                 range.first, range.second, std::ref(gammaVals));
        }
        for(auto& thread : threads){
            thread.join();
        }
    }
    else{  // single-threaded
        func(args..., 0, refImg.size(), gammaVals);
    }

    return gammaVals;
}
}

namespace{
void addTasksToQueue(LoadBalancingQueue& queue, size_t size, uint32_t nrOfThreads){
    const size_t maxTaskSize = 256;
    const size_t taskSize = std::min(maxTaskSize, std::max(size / nrOfThreads, static_cast<size_t>(1)));

    for(uint32_t i = 0; i < size; i += taskSize){
        uint32_t start = i;
        uint32_t end = std::min(start + taskSize, size);
        queue.emplace(start, end);
    }
}

template <typename Function, typename... Args>
void loadBalancingMultithreadedGammaIndexInternal(Function&& func, Args&&... args, std::vector<float>& gammaVals,
                                                  LoadBalancingQueue& tasks){
    while(true){
        if(auto task = tasks.safePop(); task.has_value()){
            const auto [start, end] = *task;
            func(args..., start, end, gammaVals);
        }
        else{
            break;
        }
    }
}

template <typename Function, typename... Args>
std::vector<float> loadBalancingMultithreadedGammaIndex(size_t refImgSize, Function&& func, Args&&... args){
    std::vector<float> gammaVals(refImgSize, 0.0f);

    const uint32_t nrOfThreads = static_cast<uint32_t>(
        std::min(static_cast<size_t>(std::thread::hardware_concurrency()), refImgSize));
    
    if(nrOfThreads > 1){  // multi-threaded
        LoadBalancingQueue tasks;
        addTasksToQueue(tasks, gammaVals.size(), nrOfThreads);

        std::vector<std::thread> threads;
        threads.reserve(nrOfThreads);

        for(uint32_t i = 0; i < nrOfThreads; i++){
            threads.emplace_back(loadBalancingMultithreadedGammaIndexInternal<Function, Args...>,
                                 std::cref(func), std::cref(args)..., std::ref(gammaVals), std::ref(tasks));
        }
        for(auto& thread : threads){
            thread.join();
        }
    }
    else{  // single-threaded
        func(args..., 0, refImgSize, gammaVals);
    }

    return gammaVals;
}
}

namespace{
std::tuple<uint32_t, uint32_t> indexTo2Dindex(size_t index, const DataSize& size){
    uint32_t j = index / size.columns;
    uint32_t i = index % size.columns;
    return {j, i};
}

std::tuple<uint32_t, uint32_t, uint32_t> indexTo3Dindex(size_t index, const DataSize& size){
    uint32_t refRcSize = size.rows * size.columns;
    uint32_t k = index / refRcSize;
    uint32_t temp = index % refRcSize;
    uint32_t j = temp / size.columns;
    uint32_t i = temp % size.columns;
    return {k, j, i};
}
}

}
