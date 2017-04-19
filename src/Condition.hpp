#ifndef CONDITION_HPP
#define CONDITION_HPP

#include "rpc.grpc.pb.h"

class Condition {

public:

    Condition(std::string comparison, std::string op, std::string key, std::string val ) :
        m_comparison(comparison),
        m_operation(op),
        m_key(key),
        m_value(val)
    {
    }
    Condition(std::string comparison, std::string op, std::string key) :
        m_comparison(comparison),
        m_operation(op),
        m_key(key)
    {
        assert((comparison == "create"));
    }

    ~Condition(){};
   
private:
 
    auto mapComparisonTarget() {
        if(m_comparison == "create")  return etcdserverpb::Compare_CompareTarget_CREATE; 
        if(m_comparison == "modify")  return etcdserverpb::Compare_CompareTarget_MOD; 
        if(m_comparison == "version") return etcdserverpb::Compare_CompareTarget_VERSION; 
        if(m_comparison == "value")   return etcdserverpb::Compare_CompareTarget_VALUE;
        return etcdserverpb::Compare_CompareTarget_CREATE;
    }

    auto mapOperation() {
        if(m_operation == "=")  return etcdserverpb::Compare_CompareResult_EQUAL; 
        if(m_operation == "<")  return etcdserverpb::Compare_CompareResult_LESS;
        if(m_operation == ">")  return etcdserverpb::Compare_CompareResult_GREATER;
        return etcdserverpb::Compare_CompareResult_EQUAL;
    }

public:    
    
    auto comparison() { 
        return mapComparisonTarget();
    }
    auto operation() {
        return mapOperation(); 
    } 
    std::string key() {
        return m_key;
    }
    std::string value() {
        return m_value;
    }

private:

    std::string m_comparison;
    std::string m_operation;
    std::string m_key;
    std::string m_value;


};

#endif
