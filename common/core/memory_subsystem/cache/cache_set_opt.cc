#include "cache_set_opt.h"

// OPT: The optimal replacement policy

CacheSetOPT::CacheSetOPT(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize) :
   CacheSet(cache_type, associativity, blocksize)
{
   m_replacement_index = m_associativity - 1;
}

CacheSetOPT::~CacheSetOPT()
{}

UInt32
CacheSetOPT::getReplacementIndex(CacheCntlr *cntlr)
{
   UInt32 curr_replacement_index = m_replacement_index;
   m_replacement_index = (m_replacement_index == 0) ? (m_associativity-1) : (m_replacement_index-1);

   if (!isValidReplacement(m_replacement_index))
      return getReplacementIndex(cntlr);
   else
      return curr_replacement_index;
}

void
CacheSetOPT::updateReplacementIndex(UInt32 accessed_index)
{
   return;
}
