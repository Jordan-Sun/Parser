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

    virtual void set_priority(size_t priority) override;

    virtual void set_protocol(std::string protocol) override;

    virtual std::string get_identifier() const override;

    virtual type_t get_type() const override;

    virtual protocol_t get_protocol() const override;

    virtual size_t get_priority() const override;

    virtual size_t get_thread_count() const override;

    virtual std::pair<std::set<std::shared_ptr<const node>>, bool> get_threads() const override;

    virtual void print(ostream &os) const override;
};