#ifndef __TRACE_VECTOR_H
#define __TRACE_VECTOR_H

#include <iterator>
#include <tuple>
#include <vector>
#include "fixed_types.h"

class TraceVector
{
  public:
    TraceVector ();
    ~TraceVector () {}
    void initFromFile (const char *, UInt32);
    void advanceTrace ();
    int nextAccess (IntPtr) const;
    void print () const;
    std::vector< std::tuple<IntPtr, int, bool> >::iterator::difference_type test () const;
  private:
    typedef std::tuple<IntPtr, int, bool> trace_t;
    std::vector<trace_t> m_trace;
    std::vector<trace_t>::iterator m_trace_iterator;
    UInt32 m_log_blocksize;
    int m_offset;
};

#endif // trace_vector.h
