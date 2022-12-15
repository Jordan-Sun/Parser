/*
 *  component.cpp
 *  source file for the component class
 *  author: jordan sun
 */

#include "component.hpp"
#include <iostream>

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
        cerr << "Warning: component " << name << " has more than one requestor." << endl;
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
    bool require_nested_thread = false;
    for (auto requestor : requestors)
    {
        requestor->get_threads(threads, require_nested_thread);
    }
    // add 1 if nested thread is required.
    return threads.size() + (require_nested_thread ? 1 : 0);
}

void component::get_threads(std::set<std::shared_ptr<const node>> &threads, bool &require_nested_thread) const
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
            requestor->get_threads(threads, require_nested_thread);
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
    os << "\tpriority: " << get_priority() << endl;
    os << "\tnumber of threads: " << get_thread_count() << endl;
}