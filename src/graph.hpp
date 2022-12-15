/*
 *  graph.hpp
 *  header file for the graph class
 *  author: jordan sun
 */

#pragma once

#include "node.hpp"
#include <map>

class graph
{
private:
    // maps node identifiers to nodes, "node.port" for connections
    std::map<std::string, std::shared_ptr<node>> nodes;
public:
    graph() = default;
    ~graph() = default;

    // add a node to the graph, return true if successful
    bool add_node(std::shared_ptr<node> node);
    // add an edge to the graph, return true if successful
    bool add_edge(std::string src_name, std::string dest_name);
    // get a node from the graph
    std::shared_ptr<node> get_node(std::string identifier);
    // print the graph
    void print(std::ostream &os) const;
};