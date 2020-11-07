#include "cache_set_opt.h"
#include "trace_vector.h"

TraceVector trace_vector;

// OPT: The optimal replacement policy

CacheSetOPT::CacheSetOPT (CacheBase::cache_t cache_type, UInt32 associativity, UInt32 blocksize)
: CacheSet (cache_type, associativity, blocksize)
{}

CacheSetOPT::~CacheSetOPT ()
{}

UInt32
CacheSetOPT::getReplacementIndex (CacheCntlr *cntlr)
{
  // First try to find an invalid block
  for (UInt32 i = 0; i < m_associativity; i++)
    {
      if (!m_cache_block_info_array[i]->isValid ())
        {
          return i;
        }
    }

  // Evict the block with farthest access time.
  int max_distance = 0;
  UInt32 index = m_associativity;
  for (UInt32 i = 0; i < m_associativity; i++)
    {
      // Skip the current index if not available
      if (!isValidReplacement (i))
        continue;

      IntPtr tag = m_cache_block_info_array[i]->getTag ();

      // There is no access afterwards
      int current_distance = trace_vector.nextAccess (tag);
      if (current_distance == -1)
        {
          return i;
        }
      // A block with farther access time
      else if (current_distance > max_distance)
        {
          max_distance = current_distance;
          index = i;
        }
    }

  // Check whether we found one
  if (index == m_associativity)
    {
      LOG_PRINT_ERROR ("Cannot find index.");
      return 0;
    }
  else
    {
      return index;
    }
}

void
CacheSetOPT::updateReplacementIndex (UInt32 accessed_index __attribute__ ((unused)))
{}
