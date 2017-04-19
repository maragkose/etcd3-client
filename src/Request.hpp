#ifndef _REQUEST_HPP
#define _REQUEST_HPP

class Request {

public:

    Request(std::string request, std::string key, std::string val) :
        m_request(request),
        m_key(key), 
        m_value(val) 
    {
    }
    Request (std::string request,std::string key):
        m_request(request), 
        m_key(key) 
    {
    }
    ~Request(){} 
    
    std::string key() {
        return m_key;
    }
    std::string value() {
        return m_value;
    }

private:

    auto mapRequestOp() {
        //if(m_request == "put")  return kRequestRange
        //if(m_request == "get")  return
        //if(m_request == "del")  return
        //return 
    }
    std::string m_request;
    std::string m_key;
    std::string m_value;
};
#endif
