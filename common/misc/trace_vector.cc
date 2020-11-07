#include "trace_vector.h"
#include <iostream>
#include <fstream>
#include <string>
#include "utils.h"

TraceVector::TraceVector ()
: m_trace (), m_log_blocksize (0), m_offset (0)
{}

// Initialize the trace from file
void
TraceVector::initFromFile (const char *file_name, UInt32 blocksize)
{
  std::string buf;
  std::size_t pos __attribute__ ((unused));
  std::ifstream input (file_name, std::ios_base::in);
  IntPtr tag;

  // Set the block size
  m_log_blocksize = floorLog2 (blocksize);

  // Handle the first line, which should always be instruction
  std::getline (input, buf);
  // std::cout << std::stoull (buf.substr (4, 12), &pos, 16) << '\n';
  tag = std::stoull (buf.substr (4, 12), &pos, 16) >> m_log_blocksize;
  m_trace.emplace_back (tag, 1, false);
  m_trace_iterator = m_trace.begin ();

  // Read the trace file
  while (!input.eof ())
    {
      // Read a whole line
      std::getline (input, buf);

      // This line is an instruction
      if (buf.substr (0, 4).compare ("0000") == 0)
        {
          tag = std::stoull (buf.substr (4, 12), &pos, 16) >> m_log_blocksize;

          // Compare the tag with the current one
          if (std::get<0> (*m_trace_iterator) == tag && std::get<2> (*m_trace_iterator) == false)
            {
              std::get<1> (*m_trace_iterator)++;
            }
          else
            {
              m_trace.emplace_back (tag, 1, false);
              m_trace_iterator = m_trace.end () - 1;
            }
        }
      else if (buf.size () > 24 && buf.substr (20, 4).compare ("addr") == 0)
        {
          // This line is a data access
          tag = std::stoull (buf.substr (25, 12), &pos, 16) >> m_log_blocksize;

          // Compare the tag with the current one
          if (std::get<0> (*m_trace_iterator) == tag && std::get<2> (*m_trace_iterator) == true)
            {
              std::get<1> (*m_trace_iterator)++;
            }
          else
            {
              m_trace.emplace_back (tag, 1, true);
              m_trace_iterator = m_trace.end () - 1;
            }

          // Extract the second address if there is one
          if (buf.size () > 37)
            {
              tag = std::stoull (buf.substr (38, 12), &pos, 16) >> m_log_blocksize;

              // Compare the tag with the current one
              if (std::get<0> (*m_trace_iterator) == tag && std::get<2> (*m_trace_iterator) == true)
                {
                  std::get<1> (*m_trace_iterator)++;
                }
              else
                {
                  m_trace.emplace_back (tag, 1, true);
                  m_trace_iterator = m_trace.end () - 1;
                }
            }
        }
    }

  // Close the file
  input.close ();

  // Reset the iterator
  m_trace_iterator = m_trace.begin ();
}

// Advance the trace
void
TraceVector::advanceTrace ()
{
  ++m_offset;
  if (m_offset == std::get<1> (*m_trace_iterator))
    {
      // Skip all data access
      ++m_trace_iterator;
      while (m_trace_iterator != m_trace.end () && std::get<2> (*m_trace_iterator))
        ++m_trace_iterator;
      m_offset = 0;
    }
}

// Given tag, find the closest distance from current access
int
TraceVector::nextAccess (IntPtr tag) const
{
  if (std::get<0> (*m_trace_iterator) == tag)
    return 1;

  // Calculate the distance
  int distance = std::get<1> (*m_trace_iterator) - m_offset;
  std::vector<trace_t>::iterator iter = m_trace_iterator + 1;
  while (iter != m_trace.end () && std::get<0> (*iter) != tag)
    {
      distance += std::get<1> (*iter);
      ++iter;
    }

  // Return -1 if not found
  if (iter == m_trace.end ())
    {
      return -1;
    }
  else
    {
      return distance;
    }
}

void
TraceVector::print () const
{
  std::vector<trace_t>::const_iterator iter;
  for (iter = m_trace.begin (); iter != m_trace.end (); ++iter)
    std::cout << std::hex << std::get<0> (*iter) << std::dec << ' ' << std::get<1> (*iter) << ' ' << std::boolalpha << std::get<2> (*iter) << '\n';
}

std::vector< std::tuple<IntPtr, int, bool> >::iterator::difference_type
TraceVector::test () const
{
  return m_trace.end () - m_trace_iterator;
}
