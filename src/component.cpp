/*
 *  component.cpp
 *  source file for the component class
 *  author: jordan sun
 */

#include "component.hpp"
#include <iostream>
#include <vector>

using namespace std;

component::component(string name)
{
    this->name = name;
}

void component::set_priority(size_t priority)
{
    this->priority = priority;
}

string component::get_identifier() const
{
    return name;
}

type_t component::get_type() const
{
    // if requestors is empty, return task
    if (requestors.empty())
    {
        return type_t::task;
    }
    // otherwise, return component
    return type_t::component;
}

protocol_t component::get_protocol() const 
{
    // component does not have protocol
    return protocol_t::none;
}

size_t component::get_priority() const 
{
    // if requestors is empty, return this priority.
    size_t num_requestors = requestors.size();
    if (num_requestors == 0)
    {
        return priority;
    }

    // otherwise, propagate the max priority of all requestors.
    size_t max_priority = 0;
    // issue a warning if there are more than one requestors.
    if (num_requestors > 1)
    {
        cerr << "Warning: component " << name << " has more than one input port." << endl;
    }
    for (auto requestor : requestors)
    {
        max_priority = max(max_priority, requestor->get_priority());
    }
    return max_priority;
}

size_t component::get_thread_count() const 
{
    // if requestors is empty, return 1.
    if (requestors.empty())
    {
        return 1;
    }
    // otherwise, propagate the sum of all requestors.
    set<shared_ptr<const node>> threads;
    vector<set<shared_ptr<const node>>> fixed_threads_pool;
    bool require_nested_thread = false;
    for (auto requestor : requestors)
    {
        requestor->get_threads(threads, fixed_threads_pool, require_nested_thread);
    }

    print_threads(cout, threads, fixed_threads_pool, require_nested_thread);

    size_t count = threads.size();
    for (auto fixed_threads : fixed_threads_pool)
    {
        // check if fixed threads is a subset of threads.
        bool is_subset = true;
        for (auto thread : fixed_threads)
        {
            if (threads.find(thread) == threads.end())
            {
                is_subset = false;
                break;
            }
        }
        // if fixed threads is not a subset of threads, increment count by 1.
        if (!is_subset) count++;
    }

    // increment count if nested thread is required.
    if (require_nested_thread) count++;

    return count;
}

void component::get_threads(set<shared_ptr<const node>> &threads, vector<set<shared_ptr<const node>>> &fixed_threads_pool, bool &require_nested_thread) const
{
    size_t num_requestors = requestors.size();

    // if requestors is empty, return this thread.
    if (num_requestors == 0)
    {
        threads.insert(shared_from_this());
    }
    // otherwise, return all threads of all requestors.
    else
    {
        // issue a warning if there are more than one requestors.
        if (num_requestors > 1)
        {
            cerr << "Warning: component " << name << " has more than one input port." << endl;
        }
        for (auto requestor : requestors)
        {
            requestor->get_threads(threads, fixed_threads_pool,require_nested_thread);
        }
    }
}

void component::print(ostream &os) const
{
    os << "component " << name << endl;
    os << "\ttype: " << (get_type() == type_t::task ? "task" : "component") << endl;
    os << "\trequestors: " << endl;
    for (auto requestor : requestors)
    {
        os << "\t\t" << requestor->get_identifier() << endl;
    }
    os << "\tpriority: " << get_priority() + (get_type() == type_t::task ? 0 : ladder_flag) << endl;
    os << "\tnumber of threads: " << get_thread_count() << endl;
}