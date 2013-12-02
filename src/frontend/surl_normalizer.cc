#include "surl_normalizer.h"
#include <boost/regex.hpp>
#include "storm_exception.hpp"

static const std::string SRM_URL_REGEXP = "^srm://[A-Za-z0-9\\.\\-]+(:\\d{1,4})?/+(srm/managerv2\\?SFN=)?[A-Za-z0-9\\._\\-/]*$";

inline bool srm_url_valid(std::string const& srm_url){
    static boost::regex const e(SRM_URL_REGEXP);
    return boost::regex_match(srm_url,e);
}

std::string
remove_double_slashes(std::string const& surl){
    static boost::regex const e("([^:])//+");
    return boost::regex_replace(surl,e,"$1/", boost::match_default | boost::format_all);
}

std::string
storm::normalize_surl(std::string surl){

    if (!srm_url_valid(surl)){
        throw invalid_surl("invalid surl: "+surl);
    }

    static std::string const QUERY_PATTERN="/srm/managerv2?SFN=";
    static size_t const QUERY_PATTERN_SIZE = QUERY_PATTERN.size();

    if (surl.empty())
        return surl;

    surl = remove_double_slashes(surl);

    size_t const pos = surl.find(QUERY_PATTERN);

    if (pos == std::string::npos)
        return surl;

    return surl.erase(pos, QUERY_PATTERN_SIZE);
}

char*
storm_normalize_surl(const char* surl){
    try{
        if (surl == 0)
            return 0;

        std::string s(storm::normalize_surl(std::string(surl)));
        char * retval = (char*) malloc(s.size());
        s.copy(retval,s.size());
        retval[s.size()]='\0';
        return retval;

    }catch(storm::invalid_surl& e){
        return 0;
    }
}
