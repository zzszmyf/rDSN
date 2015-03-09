/*
 * The MIT License (MIT)

 * Copyright (c) 2015 Microsoft Corporation

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
# pragma once

# include <rdsn/internal/task_queue.h>
# include <rdsn/internal/utils.h>

namespace rdsn {

class admission_controller
{
public:
    template <typename T> static admission_controller* create(task_queue* q, const char* args);

public:
    admission_controller(task_queue* q, std::vector<std::string>& sargs) : _queue(q) {}
    ~admission_controller(void) {}
    
    virtual bool is_task_accepted(task_ptr& task) = 0;
        
    task_queue* bound_queue() const { return _queue; }
    
private:
    task_queue* _queue;
};

// ----------------- inline implementation -----------------
template <typename T> 
admission_controller* admission_controller::create(task_queue* q, const char* args)
{
    std::vector<std::string> sargs;
    rdsn::utils::split_args(args, sargs, ' ');
    return new T(q, sargs);
}

} // end namespace
