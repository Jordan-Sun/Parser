/*
 *  main.cpp
 *  entry point for the program
 *  author: jordan sun
 */

#include "component.hpp"
#include "connection.hpp"
#include "graph.hpp"
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>
#include <list>

using namespace std;

// enumeration for return values
enum return_codes
{
    SUCCESS,
    INVALID_ARGS,
    FAILED_TO_OPEN_FILE
};
// array of long options
static struct option long_options[] =
    {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };

int main(int argc, char* argv[])
{
    // parsing helper variables.
    string line;
    smatch match;

    // initialize the graph.
    graph g;

    // parse the command line arguments
    string input_file_name = "";
    string output_file_name = "";
    while (true)
    {
        int option_index = 0;
        int c = getopt_long(argc, argv, "i:o:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
        case 'i':
            input_file_name = optarg;
            break;
        case 'o':
            output_file_name = optarg;
            break;

        default:
            break;
        }
    }

    // check if the input file was specified
    if (input_file_name == "")
    {
        cerr << "Error: no input file specified." << endl;
        cout << "Usage: " << argv[0] << " -i <input file> [-o <output file>]" << endl;
        return INVALID_ARGS;
    }

    /*
        Phase 1: Parse the components through simple regex.
        For each
            component [ComponentType] [Component];
                        ^- ignored      ^- name
        in the input file, create a component object and add it to the graph.
     */
    regex component_regex("component (\\w+) (\\w+);");

    // open the input file
    ifstream input_file(input_file_name);
    if (!input_file.is_open())
    {
        cerr << "Error: failed to open input file " << input_file_name << endl;
        return FAILED_TO_OPEN_FILE;
    }

    // parse the input file line by line
    while (getline(input_file, line))
    {
        // try matching the line with the component regex
        if (regex_search(line, match, component_regex))
        {
            // create a component object and add it to the graph
            string name = match[2];
            g.add_node(make_shared<component>(name));
        }
    }

    /*
        Phase 2: Parse the connections through double regex.
        For each
            connection [Rpc/Timer] [Connection]([Unparsed]);
                        ^- ignored         ^- name
        Split unparsed by comma, and for each
            from/to [Component].[Port]
                        ^- name     ^- port
        in the input file, create a connection object and add it to the graph.
        Then add an edge between each pair of source and destination component to the connection.
     */
    regex connection_regex("connection ([^ ]+) (\\w+)\\(([^)]+)\\);");
    regex port_regex("(from|to) (\w+).(\w+)");

    // reset the input file
    input_file.clear();
    input_file.seekg(0, ios::beg);

    // parse the input file line by line
    while (getline(input_file, line))
    {
        // try matching the line with the connection regex
        if (regex_search(line, match, connection_regex))
        {
            // create a connection object and add it to the graph
            string name = match[2];

            // parse the unparsed part of the connection
            string unparsed = match[3];
            // split the unparsed part by comma
            istringstream unparsed_stream(unparsed);
            // store the parsed nodes in a list
            list<string> from_nodes;
            list<string> conn_nodes;
            // parse each port
            while (getline(unparsed_stream, line, ','))
            {
                // try matching the unparsed port with the port regex
                if (regex_search(line, match, port_regex))
                {
                    // get the name and port of the component
                    string direction = match[1];
                    string component_name = match[2];
                    string port_name = match[3];

                    if (direction == "from")
                    {
                        // push the component's name to the from_nodes list
                        from_nodes.push_back(component_name);
                    }
                    else if (direction == "to")
                    {
                        // create a connection node
                        shared_ptr<node> conn = make_shared<connection>(name, component_name, port_name);
                        // add the connection node to the graph
                        g.add_node(conn);
                        // push the connection node's name to the conn_nodes list
                        conn_nodes.push_back(conn->get_identifier());
                        // add an edge from the connection to the component
                        g.add_edge(name, component_name);
                    }
                }
            }
            // add edges from each from node to each connection node
            for (string from_node : from_nodes)
            {
                for (string conn_node : conn_nodes)
                {
                    g.add_edge(from_node, conn_node);
                }
            }
        }
    }

    /*
        Phase 3: Parse the priority.
        For each
            [Component]._priority = [Priority];
            ^- name                     ^- priority
        in the input file, set the priority of the component.
     */
    regex priority_regex("(\\w+)\\._priority = (\\d+);");

    // reset the input file
    input_file.clear();
    input_file.seekg(0, ios::beg);

    // parse the input file line by line
    while (getline(input_file, line))
    {
        // try matching the line with the priority regex
        if (regex_search(line, match, priority_regex))
        {
            // get the name and priority of the component
            string name = match[1];
            istringstream priority_stream(match[2]);
            size_t priority;
            priority_stream >> priority;

            // set the priority of the component
            g.get_node(name)->set_priority(priority);
        }
    }

    /*
        Phase 4: Parse the propagation protocol.
        For each
            [Component].[Port]_priority_protocol = "[Protocol]";
            ^- name     ^- port                     ^- protocol
        in the input file, set the propagation protocol of the connection.
     */
    regex protocol_regex("(\\w+)\\.(\\w+)_priority_protocol = \"([^\"]+)\";");

    // reset the input file
    input_file.clear();
    input_file.seekg(0, ios::beg);

    // parse the input file line by line
    while (getline(input_file, line))
    {
        // try matching the line with the protocol regex
        if (regex_search(line, match, protocol_regex))
        {
            // get the name, port and protocol of the connection
            string name = match[1];
            string port = match[2];
            string protocol = match[3];

            // set the propagation protocol of the connection
            g.get_node(name)->set_protocol(protocol);
        }
    }

    // close the input file
    input_file.close();
    // print the graph
    g.print(cout);

    return SUCCESS;
}
