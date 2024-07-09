#ifndef COMPOUND_RB_H
#define COMPOUND_RB_H

#include "ringbuffer.hpp"
#include <vector>
#include <cassert>

/* CompoundRingBuffer<V,T,t0>
 * A data structure to store a collection of variables at various 
 * time points, such that each variable has its own "expiration date"
 * of how long data for that variable needs to be maintained in the 
 * ring buffer.
 * V - data type for individual variables (must be uniform)
 * T - data type for the timestamp value (default double)
 * t0 - the data value defining the timestamp of the initial state (default 0)
 */
template<class V, class T = double>
class CompoundRingBuffer {
 private:
  
  std::vector< RingBuffer<V> > _stateVars;
  std::vector<T> _maxDelays;

 public:
  RingBuffer<T> _timestamp;
  RingBuffer<T> _h;
  //Create a CRB to track N state variables, defining their maximum delays
  // and initial values with provided vectors whose lengths must be N
  CompoundRingBuffer(size_t n, T t0, T h0, std::vector<T>&& max_delays, 
                    const std::vector<V>& init_vals): 
      _timestamp(), _h(), _stateVars(n), _maxDelays(max_delays) { 
    //Add two copies of the initial entry to keep update logic simpler
    _timestamp[0] = t0;
    _timestamp.append_to_head(t0);
    _h[0] = h0;
    _h.append_to_head(h0);
    for (size_t index = 0; index < n; index++) {
      init_vals.at(index)[0] = init_vals[index];
      init_vals.at(index).append_to_head(init_vals[index]);
    }
  }
  CompoundRingBuffer(size_t n, T t0, T h0, const std::vector<T>& max_delays, 
                    const std::vector<V>& init_vals): 
      _timestamp(), _h(), _stateVars(n), _maxDelays(max_delays) { 
    assert(max_delays.size() >= n && init_vals.size() >= n);
    //Add two copies of the initial entry to keep update logic simpler
    _timestamp[0] = t0 - 1;
    _timestamp.append_to_head(t0);
    _h[0] = h0;
    _h.append_to_head(h0);
    for (size_t index = 0; index < n; index++) {
      _stateVars.at(index)[0] = init_vals[index];
      _stateVars.at(index).append_to_head(init_vals[index]);
    }
  }
  CompoundRingBuffer() {}

  void update(T t, T h, const std::vector<V>& vals) {
    bool append_any = false;
    assert(vals.size() >= _stateVars.size());

    for (size_t i = 0; i < _stateVars.size(); i++) {
      //If the timestamp of the new last item would still be more than 
      // maxDelay in the past for this state variable given the timestamp 
      // of the newest data entry, advance. Otherwise, extend.
      if ( _timestamp[_timestamp.size()-1] < t - _maxDelays[i]) {
        _stateVars[i].advance();
      } else {
        _stateVars[i].extend();
        append_any = true;
      }
      _stateVars[i][0] = vals[i];
    }
    if (append_any) {
      _timestamp.extend();
      _h.extend();
    } else {
      _timestamp.advance();
      _h.advance();
    }
    _timestamp[0] = t;
    _h[0] = h;
  }

  size_t bisect(T target) const {
    //Remember that values are sorted in "reverse" order
    //Start, end inclusive range
    size_t start = 0;
    size_t end = _timestamp.size()-1;
    while (start < end) {
      size_t mid = (start + end) / 2;
      if ( _timestamp[mid] < target ) {
        end = mid;
      } else {
        start = mid+1;
      }
    }

    // reduce to most recent sample if _timestamp[start] == _timstamp[start-1] (might happen for initial case)
    while ((_timestamp[start] != 0) && (_timestamp[start] == _timestamp[start-1])) {
      start--;
    }
    return start;
  }

  const RingBuffer<V>& operator[](size_t y_i) const {
    return _stateVars[y_i];
  }

};

#endif