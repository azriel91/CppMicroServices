/*=============================================================================

  Library: CppMicroServices

  Copyright (c) The CppMicroServices developers. See the COPYRIGHT
  file at the top-level directory of this distribution and at
  https://github.com/saschazelzer/CppMicroServices/COPYRIGHT .

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#ifndef USSERVICEREGISTRATIONBASEPRIVATE_H
#define USSERVICEREGISTRATIONBASEPRIVATE_H

#include <atomic>

#include "usServiceInterface.h"
#include "usServiceReference.h"
#include "usServicePropertiesImpl_p.h"
#include "usThreads_p.h"

namespace us {

class BundlePrivate;
class ServiceRegistrationBase;

/**
 * \ingroup MicroServices
 */
class ServiceRegistrationBasePrivate
{

protected:

  friend class ServiceRegistrationBase;

  // The ServiceReferenceBasePrivate class holds a pointer to a
  // ServiceRegistrationBasePrivate instance and needs to manipulate
  // its reference count. This way it can keep the ServiceRegistrationBasePrivate
  // instance alive and keep returning service properties for
  // unregistered service instances.
  friend class ServiceReferenceBasePrivate;

  /**
   * Reference count for implicitly shared private implementation.
   */
  std::atomic<int> ref;

  /**
   * Service or ServiceFactory object.
   */
  InterfaceMap service;

public:

  typedef std::unordered_map<Bundle*,int> BundleToRefsMap;
  typedef std::unordered_map<Bundle*, InterfaceMap> BundleToServiceMap;
  typedef std::unordered_map<Bundle*, std::list<InterfaceMap> > BundleToServicesMap;

  ServiceRegistrationBasePrivate(const ServiceRegistrationBasePrivate&) = delete;
  ServiceRegistrationBasePrivate& operator=(const ServiceRegistrationBasePrivate&) = delete;

  /**
   * Bundles dependent on this service. Integer is used as
   * reference counter, counting number of unbalanced getService().
   */
  BundleToRefsMap dependents;

  /**
   * Object instances that a prototype factory has produced.
   */
  BundleToServicesMap prototypeServiceInstances;

  /**
   * Object instance with bundle scope that a factory may have produced.
   */
  BundleToServiceMap bundleServiceInstance;

  /**
   * Bundle registering this service.
   */
  BundlePrivate* bundle;

  /**
   * Reference object to this service registration.
   */
  ServiceReferenceBase reference;

  /**
   * Service properties.
   */
  ServicePropertiesImpl properties;

  /**
   * Is service available. I.e., if <code>true</code> then holders
   * of a ServiceReference for the service are allowed to get it.
   */
  volatile bool available;

  /**
   * Avoid recursive unregistrations. I.e., if <code>true</code> then
   * unregistration of this service has started but is not yet
   * finished.
   */
  volatile bool unregistering;

  /**
   * Lock object for synchronous event delivery.
   */
  MultiThreaded<> eventLock;

  // needs to be recursive
  MultiThreaded<MutexLockingStrategy<std::recursive_mutex>> propsLock;

  ServiceRegistrationBasePrivate(BundlePrivate* bundle, const InterfaceMap& service,
                                 const ServicePropertiesImpl& props);

  ~ServiceRegistrationBasePrivate();

  /**
   * Check if a bundle uses this service
   *
   * @param p Bundle to check
   * @return true if bundle uses this service
   */
  bool IsUsedByBundle(Bundle* m) const;

  const InterfaceMap& GetInterfaces() const;
    std::shared_ptr<void> GetService(const std::string& interfaceId) const;

};

}


#endif // USSERVICEREGISTRATIONBASEPRIVATE_H
