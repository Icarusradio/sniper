#ifndef CACHE_SET_OPT_H
#define CACHE_SET_OPT_H

#include "cache_set.h"

class CacheSetOPT : public CacheSet
{
   public:
      CacheSetOPT(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize);
      ~CacheSetOPT();

      UInt32 getReplacementIndex(CacheCntlr *cntlr);
      void updateReplacementIndex(UInt32 accessed_index);
};

#endif /* CACHE_SET_OPT_H */
