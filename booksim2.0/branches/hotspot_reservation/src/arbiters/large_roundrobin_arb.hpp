// $Id: roundrobin_arb.hpp 4080 2011-10-22 23:11:32Z dub $

/*
 Copyright (c) 2007-2012, Trustees of The Leland Stanford Junior University
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this 
 list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// ----------------------------------------------------------------------
//
//  RoundRobin: Round Robin Arbiter without low cost and no hassel
//
// ----------------------------------------------------------------------

#ifndef _LARGEROUNDROBIN_HPP_
#define _LARGEROUNDROBIN_HPP_
#include "booksim.hpp"
#include <string>
#include <iostream>
#include <limits>
#include <map>
#include <assert.h>
class LargeRoundRobinArbiter{
  // Priority pointer
  bool _claimed;
  int  _pointer ;
  int  _size ;
  int  _selected ;
  int _highest_pri;
  int _best_input;
  int  _num_reqs ;
  
public:
  // Constructors
  LargeRoundRobinArbiter(const string &name, int size ) ;

  inline int NumReqs(){
    return _num_reqs;
  }

  // Print priority matrix to standard output
   void PrintState() const ;
  
  // Update priority matrix based on last aribtration result
   void UpdateState() ; 

  // Arbitrate amongst requests. Returns winning input and 
  // updates pointers to metadata when valid pointers are passed
   int Arbitrate() ;

   void AddRequest( int input, int id, int pri ) ;

   void Clear();

  static inline bool Supersedes(int input1, int pri1, int input2, int pri2, int offset, int size)
  {
    // in a round-robin scheme with the given number of positions and current 
    // offset, should a request at input1 with priority pri1 supersede a 
    // request at input2 with priority pri2?
    return ((pri1 > pri2) || 
	    ((pri1 == pri2) && 
	     (((input1 - offset + size) % size) < ((input2 - offset + size) % size))));
  }
  
} ;

#endif
