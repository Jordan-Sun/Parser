/*
 *  component.hpp
 *  header file for the component class
 *  author: jordan sun
 */

#pragma once
#include "node.hpp"

const ssize_t DEFAULT_PRIORITY = -1;

class component : public node
{
private:
    ssize_t priority = DEFAULT_PRIORITY;

public:
    component(std::string name);

    virtual ~component() = default;

    void set_priority(size_t priority);

    virtual std::string get_identifier() const override;

    virtual type_t get_type() const override;

    virtual protocol_t get_protocol() const override;

    virtual size_t get_priority() const override;

    virtual size_t get_thread_count() const override;

    virtual void get_threads(std::set<std::shared_ptr<const node>> &threads, bool &require_nested_thread) const override;

    virtual void print(std::ostream &os) const override;
};