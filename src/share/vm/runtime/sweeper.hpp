/*
 * Copyright (c) 1997, 2013, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_VM_RUNTIME_SWEEPER_HPP
#define SHARE_VM_RUNTIME_SWEEPER_HPP

#include "utilities/ticks.hpp"
// An NmethodSweeper is an incremental cleaner for:
//    - cleanup inline caches
//    - reclamation of unreferences zombie nmethods
//

class NMethodSweeper : public AllStatic {
  static long      _traversals;      // Stack scan count, also sweep ID.
  static nmethod*  _current;         // Current nmethod
  static int       _seen;            // Nof. nmethod we have currently processed in current pass of CodeCache
  static int       _flushed_count;   // Nof. nmethods flushed in current sweep
  static int       _zombified_count; // Nof. nmethods made zombie in current sweep
  static int       _marked_count;    // Nof. nmethods marked for reclaim in current sweep

  static volatile int      _invocations;   // No. of invocations left until we are completed with this pass
  static volatile int      _sweep_started; // Flag to control conc sweeper

  static bool      _rescan;          // Indicates that we should do a full rescan of the
                                     // of the code cache looking for work to do.
  static bool      _do_sweep;        // Flag to skip the conc sweep if no stack scan happened
  static int       _locked_seen;     // Number of locked nmethods encountered during the scan
  static int       _not_entrant_seen_on_stack; // Number of not entrant nmethod were are still on stack

  static bool      _was_full;        // remember if we did emergency unloading
  static jint      _advise_to_sweep; // flag to indicate code cache getting full
  static jlong     _last_was_full;   // timestamp of last emergency unloading
  static uint      _highest_marked;   // highest compile id dumped at last emergency unloading
  static long      _was_full_traversal;   // trav number at last emergency unloading

  // Stat counters
  static int       _number_of_flushes;            // Total of full traversals caused by full cache
  static int       _total_nof_methods_reclaimed;  // Accumulated nof methods flushed
  static Tickspan     _total_time_sweeping;          // Accumulated time sweeping
  static Tickspan     _total_time_this_sweep;        // Total time this sweep
  static Tickspan     _peak_sweep_time;              // Peak time for a full sweep
  static Tickspan     _peak_sweep_fraction_time;     // Peak time sweeping one fraction
  static Tickspan     _total_disconnect_time;        // Total time cleaning code mem
  static Tickspan     _peak_disconnect_time;         // Peak time cleaning code mem

  static void process_nmethod(nmethod *nm);

  static void log_sweep(const char* msg, const char* format = NULL, ...);

 public:
  static long traversal_count()              { return _traversals; }
  static int  number_of_flushes()            { return _number_of_flushes; }
  static int  total_nof_methods_reclaimed()  { return _total_nof_methods_reclaimed; }
  static const Tickspan total_time_sweeping()         { return _total_time_sweeping; }
  static const Tickspan peak_sweep_time()             { return _peak_sweep_time; }
  static const Tickspan peak_sweep_fraction_time()    { return _peak_sweep_fraction_time; }
  static const Tickspan total_disconnect_time()       { return _total_disconnect_time; }
  static const Tickspan peak_disconnect_time()        { return _peak_disconnect_time; }

#ifdef ASSERT
  // Keep track of sweeper activity in the ring buffer
  static void record_sweep(nmethod* nm, int line);
  static void report_events(int id, address entry);
  static void report_events();
#endif

  static void scan_stacks();      // Invoked at the end of each safepoint
  static void sweep_code_cache(); // Concurrent part of sweep job
  static void possibly_sweep();   // Compiler threads call this to sweep

  static void notify(nmethod* nm) {
    // Perform a full scan of the code cache from the beginning.  No
    // need to synchronize the setting of this flag since it only
    // changes to false at safepoint so we can never overwrite it with false.
     _rescan = true;
  }

  static void handle_full_code_cache(bool is_full); // Called by compilers who fail to allocate
  static void speculative_disconnect_nmethods(bool was_full);   // Called by vm op to deal with alloc failure

  static void set_was_full(bool state) { _was_full = state; }
  static bool was_full() { return _was_full; }
};

#endif // SHARE_VM_RUNTIME_SWEEPER_HPP
