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

#include <queue>
#include <mutex>
#include <optional>

class LoadBalancingQueue{
public:
    using Task = std::pair<size_t, size_t>;

    size_t size(){
        return m_queue.size();
    }

    void push(const Task& range){
        m_queue.push(range);
    }

    void emplace(size_t start, size_t end){
        m_queue.emplace(start, end);
    }

    std::optional<Task> safePop(){
        std::scoped_lock<std::mutex> lock(m_mutex);
        if(!m_queue.empty()){
            auto res = m_queue.front();
            m_queue.pop();
            return res;
        }
        return std::nullopt;
    }

private:
    std::queue<Task> m_queue;
    std::mutex m_mutex;
};
