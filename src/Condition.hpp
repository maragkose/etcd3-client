#ifndef CONDITION_HPP
#define CONDITION_HPP

#include "rpc.grpc.pb.h"

class Condition {

public:

    Condition(std::string comparison, std::string op, std::string key, std::string val ) :
        m_comparison(comparison),
        m_key(key),
        m_value(val)
    {
        assert(comparison == "value");
    }
    Condition(std::string comparison, std::string op, std::string key, uint64_t val ) :
        m_comparison(comparison),
        m_key(key)
    {
        assert(comparison == "modify" || comparison == "version");

        if(comparison == "modify"){
            m_modifyRevision = val;
        } else 
        if(comparison == "version"){
            m_version = val;
        }    
    }
    Condition(std::string comparison, std::string key, uint64_t createRevision) :
        m_comparison(comparison),
        m_key(key),
        m_createRevision(createRevision)
    {
        assert((comparison == "create"));
    }

    ~Condition(){};
  
    void populate(auto compare){

        compare->set_key(m_key);
        compare->set_target(comparison());
        compare->set_result (operation());

        if(m_comparison == "create"){
            compare->set_create_revision(m_createRevision);
        } else
        if(m_comparison == "modify"){
            compare->set_mod_revision(m_modifyRevision);
        } else
        if(m_comparison == "value"){
            compare->set_value  (m_value);
        } else 
        if(m_comparison == "version"){
            compare->set_version(m_version);
        }    
    } 

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
    uint64_t m_revision;
    uint64_t m_version;
    uint64_t m_createRevision;
    uint64_t m_modifyRevision;
};
#endif
