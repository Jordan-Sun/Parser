/*
 *  node.hpp
 *  header file for the node class
 *  author: jordan sun
 */

#pragma once

#include <string>
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
    std::string name;
    std::shared_ptr<node> last = nullptr;
    std::set<std::shared_ptr<node>> requestors;

    virtual ~node() = default;

    // Set priority of this node.
    virtual void set_priority(size_t priority) = 0;
    // Set protocol of this node.
    virtual void set_protocol(std::string protocol) = 0;

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
    virtual std::pair<std::set<std::shared_ptr<const node>>, bool> get_threads() const = 0;
    
    // Print the node
    virtual void print(std::ostream &os) const = 0;

    // Add a requestor to this node.
    void add_requestor(std::shared_ptr<node> requestor)
    {
        requestors.insert(requestor);
    }
};