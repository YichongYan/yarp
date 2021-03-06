/*
 * Copyright (C) 2015 Robotics and Cognitive Sciences Department. IIT
 * Authors: Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_LOCKIMPL_H
#define YARP_OS_IMPL_LOCKIMPL_H

// Pre-declare Recursive Lock
namespace yarp {
    namespace os {
        namespace impl {
            class RecursiveLockImpl;
        }
    }
}

//Decide which implementation to use
#include <yarp/conf/system.h>

#ifdef YARP_HAS_CXX11
#  include <yarp/os/impl/CXX11LockImpl.h>
#else
#  if defined(__unix__) || defined(__APPLE__)
#    include <yarp/os/impl/POSIXLockImpl.h>
#  else
//   For everything else, there's ACE
#    ifdef YARP_HAS_ACE
#      include <yarp/os/impl/ACELockImpl.h>
#    else
#      error Cannot implement semaphores
#    endif
#  endif
#endif

#endif // YARP_OS_IMPL_LOCKIMPL_H
