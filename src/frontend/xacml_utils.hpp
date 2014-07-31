/* Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2010.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef XACML_UTILS_HPP
#define XACML_UTILS_HPP

#include <cassert>
#include <boost/shared_ptr.hpp>
#include "argus/pep.h"

namespace storm {
namespace authz {

template<typename T>
class XacmlDeleter
{
  bool released_;
  typedef void (*DeleteFunction)(T*);
  DeleteFunction delete_fun_;
 public:
  XacmlDeleter(DeleteFunction delete_fun)
      : released_(false), delete_fun_(delete_fun)
  {}
  void release() { released_ = true; }
  void operator()(T* p) {
    if (!released_) delete_fun_(p);
  }
 private:
};

template<typename T>
inline XacmlDeleter<T> make_deleter(void d(T*))
{
  return XacmlDeleter<T>(d);
}

template<typename T>
inline T* release_raw_pointer(boost::shared_ptr<T> p)
{
  typedef XacmlDeleter<T> Deleter;
  Deleter* deleter = boost::get_deleter<Deleter>(p);
  assert(deleter && "the deleter is not a release deleter");
  deleter->release();
  return p.get();
}

typedef boost::shared_ptr<xacml_subject_t> SubjectPtr;
typedef boost::shared_ptr<xacml_action_t> ActionPtr;
typedef boost::shared_ptr<xacml_resource_t> ResourcePtr;
typedef boost::shared_ptr<xacml_environment_t> EnvironmentPtr;
typedef boost::shared_ptr<xacml_attribute_t> AttributePtr;
typedef boost::shared_ptr<xacml_request_t> RequestPtr;
typedef boost::shared_ptr<xacml_response_t> ResponsePtr;

inline SubjectPtr make_subject()
{
  return SubjectPtr(xacml_subject_create(), make_deleter(xacml_subject_delete));
}

inline ActionPtr make_action()
{
  return ActionPtr(xacml_action_create(), make_deleter(xacml_action_delete));
}

inline ResourcePtr make_resource()
{
  return
      ResourcePtr(xacml_resource_create(), make_deleter(xacml_resource_delete));
}

inline EnvironmentPtr make_environment()
{
  return
      EnvironmentPtr(
          xacml_environment_create(),
          make_deleter(xacml_environment_delete)
      );
}

inline AttributePtr make_attribute(char const* attribute_id)
{
  return
      AttributePtr(
          xacml_attribute_create(attribute_id),
          make_deleter(xacml_attribute_delete)
      );
}

inline RequestPtr make_request()
{
  return RequestPtr(xacml_request_create(), make_deleter(xacml_request_delete));
}

}}

#endif
