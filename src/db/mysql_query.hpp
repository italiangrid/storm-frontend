#ifndef __MYSQL_QUERY_HPP
#define __MYSQL_QUERY_HPP

#include <map>
#include <string>
#include <vector>

#include <exception>
#include <stdexcept>

#include <mysql/mysql.h>
#include "storm_functions.h"

namespace storm_db {

    class mysql_exception : public std::exception {
    public:
        mysql_exception(MYSQL* mysql) {
            _errno = mysql_errno(mysql);
            errmsg = mysql_error(mysql); 
        };
        ~mysql_exception() throw() {};
        const char* what() const throw() { return errmsg.c_str(); };
        
        int get_mysql_errno() const { return _errno;};
        std::string get_mysql_error() const { return errmsg; };
        
    private:
        int _errno;
        std::string errmsg;
    };


    void map_exec_query(struct srm_dbfd *dbfd, const std::string &query, std::map<std::string, std::vector<std::string> >& results);
    std::map<std::string, std::vector<std::string> > * map_exec_query(struct srm_dbfd *dbfd, std::string query);

    void vector_exec_query(struct srm_dbfd *dbfd, const std::string &query, std::vector< std::map<std::string, std::string> >& results);
    std::vector< std::map<std::string, std::string> > * vector_exec_query(struct srm_dbfd *dbfd, std::string query);

    int ID_exec_query(struct srm_dbfd *dbfd, std::string query);
}
#endif // __MYSQL_QUERY_HPP
