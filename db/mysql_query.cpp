#include "srmlogit.h"
#include <errno.h>
#include <serrno.h>
#include <sys/types.h>
#include <mysqld_error.h>

#include <string>
#include <map>
#include <vector>

#include "mysql_query.hpp"

using namespace std;



static pair<MYSQL_FIELD *, MYSQL_RES *>  * _query_init(struct srm_dbfd *dbfd, string query)
{
    static const char * const func = "exec_query";
    srmlogit(STORM_LOG_DEBUG, func, "Executing query ``%s''\n",query.c_str());
    if (mysql_query(&dbfd->mysql, query.c_str())) {
        srmlogit (STORM_LOG_ERROR, func, "mysql_query error: %s. Query: ``%s''\n", mysql_error (&dbfd->mysql), query.c_str());
        throw storm_db::mysql_exception(&dbfd->mysql);
    }
    MYSQL_RES *res;
    MYSQL_FIELD *fields;
    if(0 != mysql_field_count(&dbfd->mysql)){ // The stmt has no result.
        res = mysql_store_result (&dbfd->mysql);
        if (NULL == res) { // Error getting the result of the query.
            srmlogit (STORM_LOG_ERROR, func, "mysql_store_res error: %s\n", mysql_error (&dbfd->mysql));
            throw storm_db::mysql_exception(&dbfd->mysql);
        }
        fields = mysql_fetch_fields(res);
    }else{
        fields = NULL;
        res = NULL;
    }

    return new pair<MYSQL_FIELD *, MYSQL_RES *>(fields, res);
}


namespace storm_db {

    /**
     * Exec a statement and fill a supplied map of vector containing
     * the results of the query. Keys of the map are the column ID of
     * the result. Value of the map are vector containing the
     * corresponding row value. The element @c n of the vector
     * corresponding to the "XXX" key of the map, is the value of such
     * column at the row @c n.
     *
     * @param dbfd      a pointer to a srm_dbfd struct
     * @param query     the query string
     * @param results   a map<string, vector<string> > reference.
     */
    void map_exec_query(struct srm_dbfd *dbfd, const string &query, map<string, vector<string> > &results)
    {
        pair<MYSQL_FIELD *, MYSQL_RES *> *p = _query_init(dbfd, query);
        MYSQL_FIELD *fields = p->first;
        MYSQL_RES *res = p->second;

        int num_row = mysql_num_rows(res);
        int num_fields = mysql_num_fields(res);
        if(0 < num_row){
            for(int i = 0; i<num_fields; i++) 
                results[fields[i].name].reserve(num_row);

            MYSQL_ROW row;
            while(NULL != (row = mysql_fetch_row(res)))
                for(int i = 0; i<num_fields; i++)
                    results[fields[i].name].push_back((row[i] == NULL? "" : row[i]));
        }
        delete p;
        if(NULL != p->second)
            mysql_free_result(p->second);        
    }

    /**
     * Exec a statement and return a map of vector containing the
     * results of the query. Keys of the map are the column ID of the
     * result. Value of the map are vector containing the corresponding row value. The element @c n of the vector corresponding to the "XXX" key of the map, is the value of such column at the row @c n.
     *
     * @param dbfd      a pointer to a srm_dbfd struct
     * @param query     the query string

     * @return          a pointer to a  map of @c string,vector. 
     *                  The pointer should never be null, and can be empty 
     *                  if the query has empty result (like INSERT stmt)
     *                  The client must call @c delete to free memory
     */
    map<string, vector<string> > * map_exec_query(struct srm_dbfd *dbfd, const string &query)
    {
        map<string, vector<string> > *dbmap = new map<string, vector<string> >();
        if(NULL == dbmap)
            throw bad_alloc();

        map_exec_query(dbfd, query, *dbmap);

        return dbmap;
    }

    void vector_exec_query(struct srm_dbfd *dbfd, const string &query, vector< map<string, string> > &results)
    {
        pair<MYSQL_FIELD *, MYSQL_RES *> *p = _query_init(dbfd, query);
        MYSQL_FIELD *fields = p->first;
        MYSQL_RES *res = p->second;
        MYSQL_ROW row;

        int num_row = mysql_num_rows(res);
        int num_fields = mysql_num_fields(res);
        if(num_row > 0){
            for(int j=0; NULL != (row = mysql_fetch_row(res)); j++){
                map<string, string> x;
                for(int i = 0; i<num_fields; i++)
                    x[fields[i].name] = (NULL != row[i]? row[i]: "");

                results.push_back(x);        
            }
        }
        if(NULL != p->second)
            mysql_free_result(p->second);
        delete p;
    }
    /**
     * Exec a statement and return a vector of map containing the
     * results of the query. The element @c n of the vector contains
     * the {@c n}-th row of the result. The element is a map ID
     * <pre><-></pre> value.
     *
     * @param dbfd      a pointer to a srm_dbfd struct
     * @param query     the query string

     * @return          a pointer to a vector of map<string,string>.  
     *                  The pointer should never be null, but can be empty.
     *                  The client must call @c delete to free memory
     */
    vector< map<string, string> > * vector_exec_query(struct srm_dbfd *dbfd, const string &query)
    {
    
        vector< map<string, string> > *dbvec = new vector< map<string, string> >();

        vector_exec_query(dbfd, query, *dbvec);

        return dbvec;
    }

    /** \fn ID_exec_query
     * Exec the statement and return the last inserted ID, if any.
     *
     * @param dbfd     a pointer to a srm_dbfd struct
     * @param query    the string qery
     *
     * @return         {@c n} the value generated for an 
     *                 AUTO_INCREMENT column by the previous INSERT 
     *                 or UPDATE statemen. In case of a multirow insert 
     *                 query, only the {@b FIRST} id will be returned.
     *
     *                 {@c 0} if the previous statement does not use 
     *                 an AUTO_INCREMENT value.
     */
    int ID_exec_query(struct srm_dbfd *dbfd, string query)
    {
        pair<MYSQL_FIELD *, MYSQL_RES *> *p = _query_init(dbfd, query);
        if(NULL != p){
            if(NULL != p->second)
                mysql_free_result(p->second);
            delete p;
        }
        return mysql_insert_id(&dbfd->mysql);
    }

} // namespace storm_db

// int main(int argc, char **argv)
// {

//     if(argc<2){
//         cerr << "Usage: "<<argv[0]<<" query"<<endl;
//         return EINVAL;
//     }
//     bool call_map=false;
//     if(argc>2)
//         if(string("-m") == argv[2])
//             call_map=true;

//     struct srm_dbfd dbfd;
    
//     (void) mysql_init(&dbfd.mysql);
//     if(!mysql_real_connect(&dbfd.mysql, "localhost", "storm", "storm", "storm_db", 0, NULL, 0)) {
//         cerr << "Connection error: "<<mysql_error(&dbfd.mysql)<<endl;
//         return ECONNABORTED;
//     }
    
//     if(call_map){
//         map<string, vector<string> > *result; 
//         try {
//             result = map_exec_query(&dbfd, argv[1]);
//         }catch (int i){
//             return 1;
//         }
//         for(map<string, vector<string> >::const_iterator i = result->begin();
//             i != result->end();
//             ++i){
//             cout << i->first<< ":  ";
//             for(vector<string>::const_iterator j = i->second.begin();
//                 j != i->second.end();
//                 j++)
//                 cout << *j <<"|";
//             cout << endl;
//         } 
//     }else{
//         vector<map<string,string> > *result;
//         try {
//             result = vector_exec_query(&dbfd, argv[1]);
//         }catch (int i){
//             return 1;
//         }
//         for(vector<map<string, string> >::const_iterator i = result->begin();
//             i != result->end();
//             ++i){
//             for(map<string, string>::const_iterator j = i->begin();
//                 j != i->end();
//                 j++)
//                     cout << j->first << " : "<<j->second <<endl;
//             cout << endl;
//         } 

//     }
//     return 0;
// }
// 
