/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef FATAL_ERROR_H
#define FATAL_ERROR_H

#include <iostream>

/**
 * \ingroup debugging
 * \brief fatal error handling
 *
 * \param msg message to output when this macro is hit.
 *
 * When this macro is hit at runtime, the user-specified 
 * error message is output and the program is halted by
 * dereferencing a null pointer. This macro is enabled 
 * unconditionally in all builds, including debug and 
 * optimized builds.
 */
#define NS_FATAL_ERROR(msg)				\
  do                                                    \
    {                                                   \
      std::cerr << msg << std::endl;			\
      int *a = 0;                                       \
      *a = 0;                                           \
    }                                                   \
  while (false)


#endif /* FATAL_ERROR_H */
