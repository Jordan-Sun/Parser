/*
 *  graph.cpp
 *  source file for the graph class
 *  author: jordan sun
 */

#include "graph.hpp"
#include <iostream>
#include <list>

using namespace std;

bool graph::add_node(shared_ptr<node> node)
{
    // check if node already exists
    if (nodes.find(node->get_identifier()) != nodes.end())
    {
        return false;
    }
    // add node to graph
    nodes[node->get_identifier()] = node;
    return true;
}

void print_path(shared_ptr<const node> node)
{
    // recursively print the path from the source to the current node
    if (node->last)
    {
        print_path(node->last);
        cout << " -> ";
    }
    cout << node->name;
}

bool graph::add_edge(string src_name, string dest_name)
{
    // check if src and dest nodes exist
    if (nodes.find(src_name) == nodes.end() || nodes.find(dest_name) == nodes.end())
    {
        return false;
    }
    // check if there is a path from dest to src
    // note: the direction of the edge is reversed, so we are checking if there is a path from src to dest in the graph through depth first search
    shared_ptr<node> curr = nodes[src_name];
    list<shared_ptr<node>> stack;
    stack.push_front(curr);

    while (!stack.empty())
    {
        curr = stack.front();
        stack.pop_front();

        // if the current node is the source, return false
        if (curr == nodes[dest_name])
        {
            cout << "Error: cycle ";
            print_path(curr);
            // append the source node name again
            cout << " -> " << nodes[src_name]->name << " detected" << endl;
            return false;
        }

        // add the requestors of the current node to the stack
        for (auto requestor : curr->requestors)
        {
            requestor->last = curr;
            stack.push_front(requestor);
        }
    }

    // add edge to graph
    nodes[src_name]->add_requestor(nodes[dest_name]);
    return true;
}

shared_ptr<node> graph::get_node(string identifier)
{
    // check if node exists
    if (nodes.find(identifier) == nodes.end())
    {
        return nullptr;
    }
    return nodes[identifier];
}

void graph::print(ostream &os) const
{
    for (auto node : nodes)
    {
        node.second->print(os);
    }
}