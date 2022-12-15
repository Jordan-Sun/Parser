/*
 *  connection.cpp
 *  source file for the connection class
 *  author: jordan sun
 */

#include "connection.hpp"
#include <iostream>

using namespace std;

connection::connection(string name, string comp_name, string comp_port)
    : comp_name(comp_name), comp_port(comp_port)
{
    this->name = name;
}

void connection::set_priority(size_t priority)
{
    // connection does not have priority
    throw runtime_error("connection does not have priority");
}

void connection::set_protocol(std::string protocol)
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

string connection::get_identifier() const
{
    return name;
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
    bool require_nested_thread = false;

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
            threads.insert(requestor->get_threads().first.begin(), requestor->get_threads().first.end());
            require_nested_thread |= requestor->get_threads().second;
        }
        // add 1 if nested thread is required.
        return threads.size() + (require_nested_thread ? 1 : 0);
    default:
        // throw an error if protocol is not set
        throw runtime_error("protocol not set");
        break;
    }
}

pair<set<shared_ptr<const node>>, bool> connection::get_threads() const
{
    set<shared_ptr<const node>> threads;
    bool require_nested_thread = false;

    switch (protocol)
    {
    case protocol_t::ipcp:
        // only one thread is propagated, does not require nested thread (?)
        threads.insert(shared_from_this());
        // require_nested_thread = false;
        break;
    case protocol_t::pip:
        // only one thread is propagated, but require nested thread
        threads.insert(shared_from_this());
        require_nested_thread = true;
        break;
    case protocol_t::propagation:
        // propagate the sum of all requestors.
        for (auto requestor : requestors)
        {
            threads.insert(requestor->get_threads().first.begin(), requestor->get_threads().first.end());
            require_nested_thread |= requestor->get_threads().second;
        }
        break;
    default:
        // throw an error if protocol is not set
        throw runtime_error("protocol not set");
        break;
    }

    return make_pair(threads, require_nested_thread);
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