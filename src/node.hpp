/*
 *  node.hpp
 *  header file for the node class
 *  author: jordan sun
 */

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <memory>

enum class type_t
{
    none,
    task,
    component,
    connection
};

enum class protocol_t
{
    none,
    ipcp,
    pip,
    propagation
};

class node : public std::enable_shared_from_this<node>
{
public:
    // auxiliary data
    bool ladder_flag = false;
    std::string name;
    std::shared_ptr<node> last = nullptr;
    std::set<std::shared_ptr<node>> requestors;

    virtual ~node() = default;

    // Get the name of the node
    virtual std::string get_identifier() const = 0;
    // Get the type of the node
    virtual type_t get_type() const = 0;
    // Get the protocol of the node
    virtual protocol_t get_protocol() const = 0;
    
    // Get the max priority of all requestors of this node.
    virtual size_t get_priority() const = 0;
    // Get the number of threads needed to run this node.
    virtual size_t get_thread_count() const = 0;
    // Get all threads needed to run this node, recursive helper function for get_thread_count.
    virtual void get_threads(std::set<std::shared_ptr<const node>> &threads, std::vector<std::set<std::shared_ptr<const node>>> &fixed_threads_pool, bool &require_nested_thread) const = 0;

    // Print the node
    virtual void print(std::ostream &os) const = 0;

    // Add a requestor to this node.
    void add_requestor(std::shared_ptr<node> requestor)
    {
        requestors.insert(requestor);
    }

    // Print the threads set
    static void print_threads(std::ostream &os, const std::set<std::shared_ptr<const node>> &threads, std::vector<std::set<std::shared_ptr<const node>>> &fixed_threads_pool, bool require_nested_thread)
    {
        os << "{";
        for (auto it = threads.begin(); it != threads.end(); it++)
        {
            if (it != threads.begin())
            {
                os << ", ";
            }
            os << (*it)->get_identifier();
        }
        if (fixed_threads_pool.size() > 0)
        {
            os << "{";
        }
        for (auto it = fixed_threads_pool.begin(); it != fixed_threads_pool.end(); it++)
        {
            if (it != fixed_threads_pool.begin())
            {
                os << ", {";
            }
            for (auto it2 = it->begin(); it2 != it->end(); it2++)
            {
                if (it2 != it->begin())
                {
                    os << ", ";
                }
                os << (*it2)->get_identifier();
            }
            os << "}";
        }
        os << "}";
        if (require_nested_thread)
        {
            os << " + nested thread";
        }
        os << " = ";
    }
};