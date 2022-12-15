/*
 *  connection.hpp
 *  header file for the connection class
 *  author: jordan sun
 */

#pragma once
#include "node.hpp"

class connection : public node
{
private:
    protocol_t protocol = protocol_t::none;
    std::string comp_name;
    std::string comp_port;

public:
    connection(std::string name, std::string comp_name, std::string comp_port);

    virtual ~connection() = default;

    void set_protocol(std::string protocol);

    virtual std::string get_identifier() const override;

    virtual type_t get_type() const override;

    virtual protocol_t get_protocol() const override;

    virtual size_t get_priority() const override;

    virtual size_t get_thread_count() const override;

    virtual void get_threads(std::set<std::shared_ptr<const node>> &threads, bool &require_nested_thread) const override;

    virtual void print(std::ostream &os) const override;
};