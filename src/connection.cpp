/*
 *  connection.cpp
 *  source file for the connection class
 *  author: jordan sun
 */

#include "connection.hpp"
#include <iostream>
#include <vector>

using namespace std;

connection::connection(string name, string comp_name, string comp_port)
    : comp_name(comp_name), comp_port(comp_port)
{
    this->name = name;
}

void connection::set_protocol(string protocol)
{
    if (protocol == "fixed")
    {
        this->protocol = protocol_t::ipcp;
    }
    else if (protocol == "inherited")
    {
        this->protocol = protocol_t::pip;
    }
    else if (protocol == "propagated")
    {
        this->protocol = protocol_t::propagation;
    }
    else
    {
        throw runtime_error("invalid protocol");
    }
}

string connection::get_identifier() const
{
    return comp_name + "." + comp_port;
}

type_t connection::get_type() const
{
    return type_t::connection;
}

protocol_t connection::get_protocol() const
{
    return protocol;
}

size_t connection::get_priority() const
{
    size_t max_priority = 0;
    for (auto requestor : requestors)
    {
        max_priority = max(max_priority, requestor->get_priority());
    }
    return max_priority;
}

size_t connection::get_thread_count() const
{
    set<shared_ptr<const node>> threads;
    vector<set<shared_ptr<const node>>> fixed_threads_pool;
    bool require_nested_thread = false;
    size_t count;

    switch (protocol)
    {
    case protocol_t::ipcp:
        // fixed thread count of 1 for ipcp
        return 1;
        break;
    case protocol_t::pip:
        // fall through, same as propagation
    case protocol_t::propagation:
        // propagate the sum of all requestors.
        for (auto requestor : requestors)
        {
            get_threads(threads, fixed_threads_pool, require_nested_thread);
        }
        count = threads.size();
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
            if (!is_subset)
                count++;
        }

        // increment count if nested thread is required.
        if (require_nested_thread)
            count++;
        
        return count;
    default:
        cerr << "Error: " << name << " has no protocol set." << endl;
        return 0;
    }
}

void connection::get_threads(set<shared_ptr<const node>> &threads, vector<set<shared_ptr<const node>>> &fixed_threads_pool, bool &require_nested_thread) const
{

    if (protocol == protocol_t::propagation)
    {
        // propagate the sum of all requestors.
        for (auto requestor : requestors)
        {
            requestor->get_threads(threads, fixed_threads_pool, require_nested_thread);
        }
    }
    else if (protocol == protocol_t::none)
    {
        cerr << "Error: " << name << " has no protocol set." << endl;
    }
    else
    {
        set<shared_ptr<const node>> nested_threads;
        vector<set<shared_ptr<const node>>> nested_fixed_threads_pool;
        bool nested_require_nested_thread;
        // recursively get threads of requestors.
        for (auto requestor : requestors)
        {
            requestor->get_threads(nested_threads, nested_fixed_threads_pool, nested_require_nested_thread);
        }
        // both nested threads and threads in the fixed threads pool are condensed into a single fixed_threads.
        for (auto nested_fixed_threads : nested_fixed_threads_pool)
        {
            nested_threads.insert(nested_fixed_threads.begin(), nested_fixed_threads.end());
        }
        fixed_threads_pool.push_back(nested_threads);
        // require nested thread if protocol is pip
        // do not require nested thread if protocol is ipcp
        require_nested_thread = (protocol == protocol_t::pip);
        // nothing is contributed to threads.
    }
}

void connection::print(ostream &os) const
{
    os << "connection " << name << endl;
    os << "\tport of component: " << get_identifier() << endl;
    os << "\tprotocol: " << (protocol == protocol_t::ipcp ? "ipcp" : protocol == protocol_t::pip ? "pip" : protocol == protocol_t::propagation ? "propagation" : "none") << endl;
    os << "\trequestors: " << endl;
    for (auto requestor : requestors)
    {
        os << "\t\t" << requestor->get_identifier() << endl;
    }
    os << "\tpriority: " << get_priority() << endl;
    os << "\tnumber of threads: " << get_thread_count() << endl;
}