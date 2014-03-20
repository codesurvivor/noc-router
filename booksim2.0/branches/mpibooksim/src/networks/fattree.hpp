// $Id: fattree.hpp 938 2008-12-12 03:06:32Z dub $

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

////////////////////////////////////////////////////////////////////////
//
//  FatTree
//
////////////////////////////////////////////////////////////////////////
//
// RCS Information:
//  $Author: jbalfour $
//  $Date: 2007/06/26 22:49:23 $
//  $Id: fattree.hpp 938 2008-12-12 03:06:32Z dub $
// 
////////////////////////////////////////////////////////////////////////

#ifndef _FatTree_HPP_
#define _FatTree_HPP_

#include "network.hpp"

class FatTree : public Network {

  int _k;
  int _n;

  int _layout;
  int _channelWidth;

  void _ComputeSize( const Configuration& config );
  void _BuildNet(    const Configuration& config );

  Router*& _Router( int depth, int pos );

  int  _mapSize;
  int* _inputChannelMap;
  int* _outputChannelMap; 
  int* _latencyMap;
  short** coords_to_router;

  int _WireLatency( int depth1, int pos1, int depth2, int pos2 );

  // Parent Distance
  //   [Depth][Position][Port]
  //int _parentDistance[3][16][4];
  //int _portPreference[3][16][4];
  int ***_parentDistance, ***_portPreference;

public:

  static void RegisterRoutingFunctions ();
  FatTree( const Configuration& config );
  ~FatTree();

};

#endif
