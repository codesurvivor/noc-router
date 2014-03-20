// $Id: MECSCreditCombiner.hpp 887 2008-12-04 23:02:18Z dub $

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

#ifndef _MECSCREDITCOMBINER_HPP_
#define _MECSCREDITCOMBINER_HPP_

#include "MECSCreditForwarder.hpp"
#include "creditchannel.hpp"

class MECSCreditCombiner :public Module{

protected:
  int direction;
  int router;
  //select which credit channel input round robin style
  int round_robin;
  //the channel inputs
  vector<MECSCreditForwarder*> inputs;
  //output into the router
  CreditChannel* cred_out;
  
  
public:
  MECSCreditCombiner(Module* parent, string name, int dir, int r);
  void AddOutChannel(CreditChannel* cred){
    cred_out = cred;
  }
  void AddForwarder(MECSCreditForwarder* forwarder){
    inputs.push_back(forwarder);
  }
  void ReadInputs();
};
#endif
