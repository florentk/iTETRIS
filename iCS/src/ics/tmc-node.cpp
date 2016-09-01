/****************************************************************************/
/// @file    tmc.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "tmc-node.h"
#include <stdio.h>

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
TmcNode* TmcNode::tmcMode_ = NULL;

// ===========================================================================
// static method definitions
// ===========================================================================
TmcNode*
TmcNode::GetInstance()
{
    if (!tmcMode_)
        tmcMode_ = new TmcNode;

    return tmcMode_;
}

// ===========================================================================
// member method definitions
// ===========================================================================
TmcNode::TmcNode()
{
    m_icsId = 0;
    m_type = ics_types::staType_SPECIAL;
}

}