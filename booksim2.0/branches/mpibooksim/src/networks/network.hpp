// $Id: network.hpp 1102 2009-02-20 01:58:29Z qtedq $

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

#ifndef _NETWORK_HPP_
#define _NETWORK_HPP_

#include <vector>

#include "module.hpp"
#include "flit.hpp"
#include "credit.hpp"
#include "router.hpp"
#include "module.hpp"
#include "flitchannel.hpp"
#include "creditchannel.hpp"
#include "config_utils.hpp"
#include "globals.hpp"



class Network : public Module {
protected:

  int _size;
  int _shared_size;
  int _sources;
  int _dests;
  int _channels;

  int _node_count;
  int *_node_list;
  int *_router_list;
  Router **_routers;

  FlitChannel   *_inject;
  CreditChannel *_inject_cred;

  FlitChannel   *_eject;
  CreditChannel *_eject_cred;

  FlitChannel   *_chan;
  CreditChannel *_chan_cred;

  int *_chan_use;
  int _chan_use_cycles;

  virtual void _ComputeSize( const Configuration &config ) = 0;
  virtual void _BuildNet( const Configuration &config ) = 0;

  void _Alloc( );

public:
  Network( const Configuration &config );
  virtual ~Network( );

  virtual void WriteFlit( Flit *f, int source );
  virtual Flit *ReadFlit( int dest );
  virtual Flit *PeekFlit( int dest );

  virtual void    WriteCredit( Credit *c, int dest );
  virtual Credit *ReadCredit( int source );
  virtual Credit *PeekCredit( int source );

  int  NumSources( ) const;
  int  NumDests( ) const;

  virtual void InsertRandomFaults( const Configuration &config );
  void OutChannelFault( int r, int c, bool fault = true );

  virtual double Capacity( ) const;

  virtual void ReadInputs( );
  virtual void InternalStep( );
  virtual void WriteOutputs( );

  //multithreading
  virtual void ReadInputs(int t);
  virtual void InternalStep( int t);
  virtual void WriteOutputs( int t);
  virtual void DOALL(int t);
  void Display( ) const;

  int NumChannels(){return _channels;}
  FlitChannel* GetInject(){return _inject;}
  FlitChannel* GetEject(){return _eject;}
  FlitChannel* GetChannels(){return _chan;}
  Router ** GetRouters(){return _routers;}
  int NumRouters(){return _size;}

  void Divide(int id, int t);
  void GetNodes(int ** list, int* count);
};

#endif 

