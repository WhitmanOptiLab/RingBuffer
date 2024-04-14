#ifndef RINGBUFFER
#define RINGBUFFER_H
#include <iterator>
#include <vector>
#include <algorithm>

/* RingBuffer class
 * Goals: provide an indexable queue that can support timestamp bisection
 */
template<class T>
class RingBuffer {

  private:
    size_t capacity; //Will always be a power of 2
    size_t cap_min1; //Will always be one less than a power of 2
    std::vector<T> _entries;
    size_t head, tail;

    //Relies on assumption that capacity is a positive power of 2
    size_t wrap(size_t i) const {
      return i & cap_min1;
    }

    void expand() {
      std::vector<T> new_vect;
      new_vect.resize(capacity*2);
      if ( tail > head ) {
        //Single range
        std::copy(_entries.begin()+head, _entries.begin()+tail, new_vect.begin());
      } else {
        //Split range
        std::copy(_entries.begin() + head, _entries.end(), new_vect.begin());
        std::copy(_entries.begin(), _entries.begin() + tail, new_vect.begin() + (capacity - head));
      }
      tail = wrap(tail - head);
      head = 0;
      capacity = capacity*2;
      cap_min1 = capacity-1;
      _entries = std::move(new_vect);
    }

  public:

    RingBuffer() : capacity(4), cap_min1(capacity-1), _entries(capacity), 
        head(3), tail(0) { _entries.resize(capacity); }
  
    T& operator[](size_t index) {
      return _entries.at( wrap(index+head) );
    }

    size_t size() const {
      return ( wrap(tail - head) );
    }

    const T& operator[](size_t index) const {
      return _entries.at( wrap(index+head) );
    }

    void advance() {
      head = wrap(head-1);
      tail = wrap(tail-1);
    }

    void extend() {
      if (head == wrap(tail+1) ) {
        expand();
      }
      head = wrap(head-1);
    }

    void append_to_head(const T& entry) {
      extend();
      _entries.at(head) = entry;
    }
};



#endif
