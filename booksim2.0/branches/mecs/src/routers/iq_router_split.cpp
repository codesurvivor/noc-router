// $Id: iq_router_split.cpp -1   $

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

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>

#include "globals.hpp"
#include "random_utils.hpp"
#include "iq_router_split.hpp"

IQRouterSplit::IQRouterSplit( const Configuration& config,
		    Module *parent, const string & name, int id,
		    int inputs, int outputs )
  : IQRouterBase( config, parent, name, id, inputs, outputs )
{
  string alloc_type;
  string arb_type;
  int iters;

  // check constraints
  if(_routing_delay != 0)
    Error("This router architecture requires lookahead routing!");
  
  // Allocate the allocators
  config.GetStr( "sw_allocator", alloc_type );
  config.GetStr( "sw_alloc_arb_type", arb_type );
  iters = config.GetInt("sw_alloc_iters");
  if(iters == 0) iters = config.GetInt("alloc_iters");
  _sw_allocator = Allocator::NewAllocator( this, "sw_allocator",
					   alloc_type,
					   _inputs*_input_speedup, 
					   _outputs*_output_speedup,
					   iters, arb_type );
  
  _vc_rr_offset.resize(_inputs*_vcs);
  _sw_rr_offset.resize(_inputs*_input_speedup);
  
  _use_fast_path.resize(_inputs*_vcs, true);
}

IQRouterSplit::~IQRouterSplit( )
{
  delete _sw_allocator;
}

void IQRouterSplit::_Alloc( )
{
  bool watched = false;
  int fast_path_vcs[_inputs];
  _sw_allocator->Clear( );
  
  for(int input = 0; input < _inputs; ++input) {
    
    fast_path_vcs[input] = -1;
    
    for(int s = 0; s < _input_speedup; ++s) {
      int expanded_input  = s*_inputs + input;
      
      // Arbitrate (round-robin) between multiple requesting VCs at the same 
      // input (handles the case when multiple VC's are requesting the same 
      // output port)
      int vc = _sw_rr_offset[expanded_input];
      
      for(int v = 0; v < _vcs; ++v) {
	
	// This continue acounts for the interleaving of VCs when input speedup 
	// is used.
	// dub: Essentially, this skips loop iterations corresponding to those 
	// VCs not in the current speedup set. The skipped iterations will be 
	// handled in a different iteration of the enclosing loop over 's'.
	// dub: Furthermore, we skip this iteration if the current VC has only a
	// single, newly arrived flit.
	if(((vc % _input_speedup) != s) || _use_fast_path[input*_vcs+vc]) {
	  vc = (vc + 1) % _vcs;
	  continue;
	}
	
	VC * cur_vc = _vc[input][vc];
	
	VC::eVCState vc_state = cur_vc->GetState();
	
	if(cur_vc->FrontFlit() && cur_vc->FrontFlit()->watch) 
	  *gWatchOut << GetSimTime() << " | " << FullName() << " | "
		      << "Saw flit " << cur_vc->FrontFlit()->id
		      << " in slow path." << endl;
	
	if(!cur_vc->Empty()) {
	  
	  Flit * f = cur_vc->FrontFlit();
	  assert(f);
	  
	  if(((vc_state != VC::vc_alloc) && (vc_state != VC::active)) ||
	     (cur_vc->GetStateTime() < _sw_alloc_delay)) {
	    if(f->watch)
	      *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			  << "VC " << vc << " at input " << input 
			  << " is not ready for slow-path allocation (flit: " << f->id 
			  << ", state: " << VC::VCSTATE[vc_state] 
			  << ", state time: " << cur_vc->GetStateTime() 
			  << ")." << endl;
	    vc = (vc + 1) % _vcs;
	    continue;
	  }	    
	  
	  if(f->watch)
	    *gWatchOut << GetSimTime() << " | " << FullName() << " | "
			<< "VC " << vc << " at input " << input
			<< " is requesting slow-path allocation (flit: " << f->id 
			<< ", state: " << VC::VCSTATE[vc_state]
			<< ")." << endl;
	  
	  const OutputSet * route_set = cur_vc->GetRouteSet();
	  
	  int output = _vc_rr_offset[input*_vcs+vc];
	  
	  for(int output_index = 0; output_index < _outputs; ++output_index) {
	    
	    // in active state, we only care about our assigned output port
	    if(vc_state == VC::active) {
	      output = cur_vc->GetOutputPort();
	    }
	    
	    // When input_speedup > 1, the virtual channel buffers are 
	    // interleaved to create multiple input ports to the switch.
	    // Similarily, the output ports are interleaved based on their 
	    // originating input when output_speedup > 1.
	    
	    assert(expanded_input == (vc%_input_speedup)*_inputs+input);
	    int expanded_output = (input%_output_speedup)*_outputs + output;
	    
	    if((_switch_hold_in[expanded_input] == -1) && 
	       (_switch_hold_out[expanded_output] == -1)) {
	      
	      BufferState * dest_vc = _next_vcs[output];
	      
	      bool do_request = false;
	      int in_priority;
	      
	      // check if any suitable VCs are available and determine the 
	      // highest priority for this port
	      int vc_cnt = route_set->NumVCs(output);
	      assert(!((vc_state == VC::active) && (vc_cnt == 0)));
	      
	      for(int vc_index = 0; vc_index < vc_cnt; ++vc_index) {
		int vc_prio;
		int out_vc = route_set->GetVC(output, vc_index, &vc_prio);
		
		if((vc_state == VC::vc_alloc) &&
		   !dest_vc->IsAvailableFor(out_vc)) {
		  if(f->watch)
		    *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
				<< "VC " << out_vc << " at output "
				<< output << " is busy." << endl;
		  continue;
		} else if((vc_state == VC::active) && 
			  (out_vc != cur_vc->GetOutputVC())) {
		  continue;
		}
		
		if(dest_vc->IsFullFor(out_vc)) {
		  if(f->watch)
		    *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
				<< "VC " << out_vc << " at output "
				<< output << " has no buffers available." << endl;
		  continue;
		}
		
		if(f->watch)
		  *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			      << "VC " << out_vc << " at output "
			      << output << " is available." << endl;
		if(!do_request || (vc_prio > in_priority)) {
		  do_request = true;
		  in_priority = vc_prio;
		}
	      }
	      
	      if(do_request) {
		
		if(f->watch) {
		  *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			      << "VC " << vc << " at input "
			      << input << " requests output " << output 
			      << " (flit: " << f->id
			      << ", exp. input: " << expanded_input
			      << ", exp. output: " << expanded_output
			      << ")." << endl;
		  watched = true;
		}
		
		// We could have requested this same input-output pair in a 
		// previous iteration; only replace the previous request if the 
		// current request has a higher priority (this is default 
		// behavior of the allocators). Switch allocation priorities 
		// are strictly determined by the packet priorities.
		
		_sw_allocator->AddRequest(expanded_input, expanded_output, vc, 
					  in_priority, cur_vc->GetPriority());
		
	      }
	    }

	    // in active state, we only care about our assigned output port
	    if(vc_state == VC::active) {
	      break;
	    }
	    
	    output = (output + 1) % _outputs;
	  }
	}
	vc = (vc + 1) % _vcs;
      }
    }
    
    // dub: handle fast-path flits separately so we know all switch requests 
    // from other VCs that are on the regular path have been issued already
    for(int vc = 0; vc < _vcs; vc++) {
      
      if(_use_fast_path[input*_vcs+vc]) {
	
	VC * cur_vc = _vc[input][vc];
	
	if(cur_vc->Empty()) {
	  continue;
	}
	
	Flit * f = cur_vc->FrontFlit();
	assert(f);
	
	if(f->watch)
	  *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
		      << "Saw flit " << f->id
		      << " in fast path." << endl;
	
	VC::eVCState vc_state = cur_vc->GetState();
	
	if(((vc_state != VC::vc_alloc) && (vc_state != VC::active)) ||
	   (cur_vc->GetStateTime() < _sw_alloc_delay)) {
	  if(f->watch)
	    *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			<< "VC " << vc << " at input " << input 
			<< " is not ready for fast-path allocation (flit: " << f->id 
			<< ", state: " << VC::VCSTATE[vc_state] 
			<< ", state time: " << cur_vc->GetStateTime() 
			<< ")." << endl;
	  continue;
	}	    
	
	if(f->watch)
	  *gWatchOut << GetSimTime() << " | " << FullName() << " | "
		      << "VC " << vc << " at input " << input
		      << " is requesting fast-path allocation (flit: " << f->id 
		      << ", state: " << VC::VCSTATE[vc_state]
		      << ")." << endl;
	
	if(fast_path_vcs[input] >= 0)
	  cout << "XXX" << endl
	       << FullName() << endl
	       << "VC: " << vc << ", input: " << input << ", flit: " << f->id
	       << ", fast VC: " << fast_path_vcs[input] << ", fast flit: "
	       << _vc[input][fast_path_vcs[input]]->FrontFlit()->id << endl
	       << "XXX" << endl;
	assert(fast_path_vcs[input] < 0);
	
	fast_path_vcs[input] = vc;
	
	const OutputSet * route_set = cur_vc->GetRouteSet();
	int expanded_input = (vc%_input_speedup)*_inputs+input;
	
	for(int output = 0; output < _outputs; ++output) {
	  
	  // dub: if we're done with VC allocation, we already know our output
	  if(vc_state == VC::active) {
	    output = cur_vc->GetOutputPort();
	  }
	  
	  BufferState * dest_vc = _next_vcs[output];
	  
	  int expanded_output = (input%_output_speedup)*_outputs + output;
	  
	  if(_sw_allocator->ReadRequest(expanded_input, expanded_output) >= 0) {
	    if(f->watch)
	      *gWatchOut << GetSimTime() << " | " << FullName() << " | "
			  << "Crossbar slot is already in use by slow path "
			  << "(exp. input: " << expanded_input
			  << ", exp. output: " << expanded_output
			  << ")." << endl;
	    if(vc_state == VC::active) {
	      break;
	    } else {
	      continue;
	    }
	  }
	  
	  bool do_request = false;
	  int in_priority;
	  
	  int vc_cnt = route_set->NumVCs(output);
	  assert((vc_state != VC::active) || (vc_cnt > 0));
	  
	  for(int vc_index = 0; vc_index < vc_cnt; ++vc_index) {
	    int vc_prio;
	    int out_vc = route_set->GetVC(output, vc_index, &vc_prio);
	    
	    if((vc_state == VC::vc_alloc) && 
	       !dest_vc->IsAvailableFor(out_vc)) {
	      if(f->watch)
		*gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			    << "VC " << out_vc << " at output "
			    << output << " is busy." << endl;
	      continue;
	    } else if((vc_state == VC::active) && 
		      (out_vc != cur_vc->GetOutputVC())) {
	      continue;
	    }
	    
	    if(dest_vc->IsFullFor(out_vc)) {
	      if(f->watch)
		*gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			    << "VC " << out_vc << " at output "
			    << output << " has no buffers available." << endl;
	      continue;
	    }
	    
	    if(f->watch)
	      *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			  << "VC " << out_vc << " at output "
			  << output << " is available." << endl;
	    if(!do_request || (vc_prio > in_priority)) {
	      do_request = true;
	      in_priority = vc_prio;
	    }	    
	  }
	  
	  if(do_request) {
	    
	    if(f->watch)
	      *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			  << "VC " << vc << " at input "
			  << input << " requests output " << output 
			  << " (flit: " << f->id
			  << ", exp. input: " << expanded_input
			  << ", exp. output: " << expanded_output
			  << ")." << endl;
	    
	    // We could have requested this same input-output pair in a 
	    // previous iteration; only replace the previous request if the 
	    // current request has a higher priority (this is default 
	    // behavior of the allocators). Switch allocation priorities 
	    // are strictly determined by the packet priorities.
	    
	    _sw_allocator->AddRequest(expanded_input, expanded_output, vc, 
				      in_priority, cur_vc->GetPriority());
	    
	  }
	  
	  if(vc_state == VC::active) {
	    break;
	  }
	}
      }
    }
  }
  
  if(watched) {
    *gWatchOut << GetSimTime() << " | " << _sw_allocator->FullName() << " | ";
    _sw_allocator->PrintRequests(gWatchOut);
  }
  
  _sw_allocator->Allocate();
  
  if(watched) {
    *gWatchOut << GetSimTime() << " | " << _sw_allocator->FullName() << " | "
		<< "Grants = [ ";
    for(int input = 0; input < _inputs; ++input)
      for(int s = 0; s < _input_speedup; ++s) {
	int expanded_input = s * _inputs + input;
	int expanded_output = _sw_allocator->OutputAssigned(expanded_input);
	if(expanded_output > -1) {
	  int output = expanded_output % _outputs;
	  int vc = _sw_allocator->ReadRequest(expanded_input, expanded_output);
	  *gWatchOut << input << " -> " << output << " (vc:" << vc << ")  ";
	}
      }
    *gWatchOut << "]." << endl;
  }
  
  // Winning flits cross the switch

  _crossbar_pipe->WriteAll(NULL);

  //////////////////////////////
  // Switch Power Modelling
  //  - Record Total Cycles
  //
  switchMonitor.cycle() ;

  for(int input = 0; input < _inputs; ++input) {
    
    Credit * c = NULL;
    
    for(int s = 0; s < _input_speedup; ++s) {
      
      int expanded_input = s*_inputs + input;
      int expanded_output;
      VC * cur_vc;
      int vc;
      int fvc = fast_path_vcs[input];
      
      if(_switch_hold_in[expanded_input] != -1) {
	assert(_switch_hold_in[expanded_input] >= 0);
	expanded_output = _switch_hold_in[expanded_input];
	vc = _switch_hold_vc[expanded_input];
	cur_vc = _vc[input][vc];
	
	if(cur_vc->Empty()) { // Cancel held match if VC is empty
	  expanded_output = -1;
	}
      } else {
	expanded_output = _sw_allocator->OutputAssigned(expanded_input);
	if(expanded_output >= 0) {
	  vc = _sw_allocator->ReadRequest(expanded_input, expanded_output);
	  cur_vc = _vc[input][vc];
	} else {
	  vc = -1;
	  cur_vc = NULL;
	}
      }
      
      if(expanded_output >= 0) {
	
	int output = expanded_output % _outputs;
	
	Flit * f = cur_vc->FrontFlit();
	assert(f);
	
	if(vc == fvc) {
	  if(f->watch)
	    *gWatchOut << GetSimTime() << " | " << FullName() << " | "
			<< "Fast-path allocation successful for VC " << vc
			<< " at input " << input << " (flit: " << f->id
			<< ")." << endl;
	} else {
	  if(f->watch)
	    *gWatchOut << GetSimTime() << " | " << FullName() << " | "
			<< "Slow-path allocation successful for VC " << vc
			<< " at input " << input << " (flit: " << f->id
			<< ")." << endl;
	  if(fvc >= 0) {
	    assert(_use_fast_path[input*_vcs+fvc]);
	    VC * fast_vc = _vc[input][fvc];
	    assert(fast_vc->FrontFlit());
	    if(fast_vc->FrontFlit()->watch)
	      cout << GetSimTime() << " | " << FullName() << " | "
		   << "Disabling fast-path allocation for VC " << fvc
		   << " at input " << input << "." << endl;
	    _use_fast_path[input*_vcs+fvc] = false;
	  }
	}
	
	BufferState * dest_vc = _next_vcs[output];
	
	switch(cur_vc->GetState()) {
	  
	case VC::vc_alloc:
	  {
	    const OutputSet * route_set = cur_vc->GetRouteSet();
	    int sel_prio = -1;
	    int sel_vc = -1;
	    int vc_cnt = route_set->NumVCs(output);
	    
	    for(int vc_index = 0; vc_index < vc_cnt; ++vc_index) {
	      int out_prio;
	      int out_vc = route_set->GetVC(output, vc_index, &out_prio);
	      if(!dest_vc->IsAvailableFor(out_vc)) {
		if(f->watch)
		  *gWatchOut << GetSimTime() << " | " << FullName() << " | "
			      << "VC " << out_vc << " at output "
			      << output << " is busy." << endl;
		continue;
	      }
	      if(dest_vc->IsFullFor(out_vc)) {
		if(f->watch)
		  *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			      << "VC " << out_vc << " at output "
			      << output << " has no buffers available." << endl;
		continue;
	      }
	      
	      if(f->watch)
		*gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			    << "VC " << out_vc << " at output "
			    << output << " is available." << endl;
	      if(out_prio > sel_prio) {
		sel_vc = out_vc;
		sel_prio = out_prio;
	      }
	    }
	    
	    if(sel_vc < 0) {
	      cout << "XXX" << endl
		   << "Flit " << f->id
		   << ", VC " << vc
		   << ", input " << input << ":" << endl
		   << "None of " << vc_cnt << " VCs at output " << output 
		   << " were suitable and available." << endl 
		   << "XXX" << endl;
	    }
	    // we should only get to this point if some VC requested 
	    // allocation
	    assert(sel_vc > -1);
	    
	    // dub: this is taken care of later on
	    //cur_vc->SetState(VC::active);
	    cur_vc->SetOutput(output, sel_vc);
	    dest_vc->TakeBuffer(sel_vc);
	    
	    _vc_rr_offset[input*_vcs+vc] = (output + 1) % _outputs;
	    
	    if(f->watch)
	      *gWatchOut << GetSimTime() << " | " << FullName() << " | "
			  << "VC " << sel_vc << " at output " << output
			  << " granted to VC " << vc << " at input " << input
			  << " (flit: " << f->id << ")." << endl;
	  }
	  // NOTE: from here, we just fall through to the code for VC::active!
	  
	case VC::active:
	  
	  if(_hold_switch_for_packet) {
	    _switch_hold_in[expanded_input] = expanded_output;
	    _switch_hold_vc[expanded_input] = vc;
	    _switch_hold_out[expanded_output] = expanded_input;
	  }
	  
	  //assert(cur_vc->GetState() == VC::active);
	  assert(!cur_vc->Empty());
	  assert(cur_vc->GetOutputPort() == output);
	  
	  dest_vc = _next_vcs[output];
	  
	  assert(!dest_vc->IsFullFor(cur_vc->GetOutputVC()));
	  
	  // Forward flit to crossbar and send credit back
	  f = cur_vc->RemoveFlit();
	  
	  if(f->watch)
	    *gWatchOut << GetSimTime() << " | " << FullName() << " | " 
			<< "Output " << output
			<< " granted to VC " << vc << " at input " << input
			<< " (flit: " << f->id
			<< ", exp. input: " << expanded_input
			<< ", exp. output: " << expanded_output
			<< ")." << endl;
	  
	  f->hops++;
	  
	  //
	  // Switch Power Modelling
	  //
	  switchMonitor.traversal(input, output, f);
	  bufferMonitor.read(input, f);
	  
	  if(f->watch)
	    *gWatchOut << GetSimTime() << " | " << FullName() << " | "
			<< "Forwarding flit " << f->id << " through crossbar "
			<< "(exp. input: " << expanded_input
			<< ", exp. output: " << expanded_output
			<< ")." << endl;
	  
	  if (c == NULL) {
	    c = _NewCredit(_vcs);
	  }
	  
	  assert(vc == f->vc);
	  
	  c->vc[c->vc_cnt] = vc;
	  c->vc_cnt++;
	  c->dest_router = f->from_router;
	  f->vc = cur_vc->GetOutputVC();
	  dest_vc->SendingFlit(f);
	  
	  _crossbar_pipe->Write(f, expanded_output);
	  
	  if(f->tail) {
	    if(cur_vc->Empty()) {
	      cur_vc->SetState(VC::idle);
	    } else {
	      cur_vc->Route(_rf, this, cur_vc->FrontFlit(), input);
	      cur_vc->SetState(VC::vc_alloc);
	    }
	    _switch_hold_in[expanded_input] = -1;
	    _switch_hold_vc[expanded_input] = -1;
	    _switch_hold_out[expanded_output] = -1;
	  } else {
	    // reset state timer for next flit
	    cur_vc->SetState(VC::active);
	  }	  
	  
	  if(!_use_fast_path[input*_vcs+vc]) {
	    _sw_rr_offset[expanded_input] = (vc + 1) % _vcs;
	  }
	  
	  if(cur_vc->Empty() && !_use_fast_path[input*_vcs+vc]) {
	    if(f->watch)
	      *gWatchOut << GetSimTime() << " | " << FullName() << " | "
			  << "Enabling fast-path allocation for VC " << vc
			  << " at input " << input << "." << endl;
	    _use_fast_path[input*_vcs+vc] = true;
	  }
	  
	} 
      } else if((fvc >= 0) && ((fvc % _input_speedup) == s)) {
	
	assert(_use_fast_path[input*_vcs+fvc]);
	
	VC * fast_vc = _vc[input][fvc];
	Flit * f = fast_vc->FrontFlit();
	assert(f);
	
	if(f->watch)
	  *gWatchOut << GetSimTime() << " | " << FullName() << " | "
		      << "Disabling fast-path allocation for VC " << fvc
		      << " at input " << input << "." << endl;
	_use_fast_path[input*_vcs+fvc] = false;
      }
    }    
    _credit_pipe->Write(c, input);
  }
}
