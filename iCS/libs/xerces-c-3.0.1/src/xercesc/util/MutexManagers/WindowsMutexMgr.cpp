/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: WindowsMutexMgr.cpp 745390 2009-02-18 05:51:29Z borisk $
 */

#include <windows.h>

#include <xercesc/util/XMemory.hpp>
#include <xercesc/util/MutexManagers/WindowsMutexMgr.hpp>
#include <xercesc/framework/MemoryManager.hpp>

XERCES_CPP_NAMESPACE_BEGIN

// Wrap up the critical section with XMemory
//
class CSWrap: public XMemory
{
public:
  CRITICAL_SECTION cs;
};

WindowsMutexMgr::WindowsMutexMgr()
{
}


WindowsMutexMgr::~WindowsMutexMgr()
{
}


XMLMutexHandle
WindowsMutexMgr::create(MemoryManager* const manager)
{
    CSWrap* mutex = new (manager) CSWrap;
    InitializeCriticalSection(&mutex->cs);
    return mutex;
}


void
WindowsMutexMgr::destroy(XMLMutexHandle mtx, MemoryManager* const)
{
    CSWrap* mutex = (CSWrap*)mtx;
    if (mutex != 0)
    {
      ::DeleteCriticalSection(&mutex->cs);
      delete mutex;
    }
}


void
WindowsMutexMgr::lock(XMLMutexHandle mtx)
{
    CSWrap* mutex = (CSWrap*)mtx;
    ::EnterCriticalSection(&mutex->cs);
}


void
WindowsMutexMgr::unlock(XMLMutexHandle mtx)
{
    CSWrap* mutex = (CSWrap*)mtx;
    ::LeaveCriticalSection(&mutex->cs);
}


XERCES_CPP_NAMESPACE_END
