#include "get_socket_info.hpp"
#include "srmlogit.h"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>

/**
 * get_address (IPV4)
 * @param peer [IN]
 * @return address as string
 */
std::string get_address_ipv4(sockaddr_in * peer)
{
    char buf[INET_ADDRSTRLEN]; 
    const char * res = inet_ntop(AF_INET, (const void *)&peer->sin_addr, buf, INET_ADDRSTRLEN);
    srmlogit(STORM_LOG_INFO, "get_address", "IPV4\n");
    if (0 == res) {
        std::string ret;
        return ret; 
    } else {
        std::string ret(res);
        return ret; 
    }
}

/**
 * get_address (IPV6)
 * @param soap [] the soap struct, peer [IN] the sockaddr_storage struct
 * @return address as string
 */
std::string get_address(struct soap * soap, sockaddr_storage * peer)
{
    if (AF_INET6 == peer->ss_family){ // IPV6
        sockaddr_in6 *sockaddr6 = (sockaddr_in6 *) peer;
        char buf[INET6_ADDRSTRLEN];
        const char * res = inet_ntop(AF_INET6, (const void *)&sockaddr6->sin6_addr, buf, INET6_ADDRSTRLEN);
        srmlogit(STORM_LOG_INFO, "get_address", "IPV6\n");
        if (0 == res) {
            std::string ret;
            return ret;
        } else {
            std::string ret(res);
            return ret;
        }
    } else if (AF_INET == peer->ss_family){ // IPV4
        sockaddr_in * sockaddr = (sockaddr_in *) peer;
        if (sockaddr) {
          srmlogit(STORM_LOG_INFO, "get_address", "IPV4 from IPV6\n");
          return get_address_ipv4(sockaddr);
        } else {
          std::stringstream ss;
          ss << "Address family: ";
          #define SS_ADDR(a) case a: ss << #a; break
          switch (peer->ss_family){
            SS_ADDR(AF_UNIX);
            SS_ADDR(AF_UNSPEC);
          default:
            ss << "unknown";
            break;
          }
          ss << " host: " << soap->host;
          ss << " ip: " << soap->ip;
          srmlogit(STORM_LOG_INFO, "get_address", "? from IPV6\n");
          return ss.str();
        }
    } else {
        std::stringstream ss;
        ss << "Address family: ";
#define SS_ADDR(a) case a: ss << #a; break        
        switch (peer->ss_family){
          SS_ADDR(AF_UNIX);
          SS_ADDR(AF_UNSPEC);
        default:
            ss << "unknown";
            break;
        }
        ss << " host: " << soap->host;
        ss << " ip: " << soap->ip;
        srmlogit(STORM_LOG_INFO, "get_address", "?? from IPV6\n");
        return ss.str();
    }

}

/**
 * get_ip.
 * Call appropriate overload according to type of soap->peer, which is sockaddr_storage
 * id compiling with -DWITH_IPV6 and sockaddr_in otherwise
 * @param soap [IN] the soap struct
 * @return address as string
 */
std::string get_ip(struct soap *soap)
{
    return get_address(soap,&(soap->peer));
}

