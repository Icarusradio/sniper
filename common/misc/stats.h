#pragma once

#include "simulator.h"
#include "itostr.h"

#include <strings.h>
#include <db.h>

class StatsMetricBase
{
   public:
      String objectName;
      UInt32 index;
      String metricName;
      StatsMetricBase(String _objectName, UInt32 _index, String _metricName) :
         objectName(_objectName), index(_index), metricName(_metricName)
      {}
      virtual UInt64 recordMetric() = 0;
      virtual bool isDefault() { return false; } // Return true when value hasn't changed from its initialization value
};

template <class T> UInt64 makeStatsValue(T t);

template <class T> class StatsMetric : public StatsMetricBase
{
   public:
      T *metric;
      StatsMetric(String _objectName, UInt32 _index, String _metricName, T *_metric) :
         StatsMetricBase(_objectName, _index, _metricName), metric(_metric)
      {}
      virtual UInt64 recordMetric()
      {
         return makeStatsValue<T>(*metric);
      }
      virtual bool isDefault()
      {
         return recordMetric() == 0;
      }
};

typedef UInt64 (*StatsCallback)(String objectName, UInt32 index, String metricName, void *arg);
class StatsMetricCallback : public StatsMetricBase
{
   public:
      StatsCallback func;
      void *arg;
      StatsMetricCallback(String _objectName, UInt32 _index, String _metricName, StatsCallback _func, void *_arg) :
         StatsMetricBase(_objectName, _index, _metricName), func(_func), arg(_arg)
      {}
      virtual UInt64 recordMetric()
      {
         return func(objectName, index, metricName, arg);
      }
};


class StatsManager
{
   public:
      StatsManager();
      ~StatsManager();
      void init();
      void recordStats(String prefix);
      void registerMetric(StatsMetricBase *metric);
      StatsMetricBase *getMetricObject(String objectName, UInt32 index, String metricName);

   private:
      UInt64 m_keyid;
      UInt64 m_prefixnum;
      DB *m_db;
      // Use std::string here because String (__versa_string) does not provide a hash function for STL containers with gcc < 4.6
      typedef std::unordered_map<UInt64, StatsMetricBase *> StatsIndexList;
      typedef std::pair<UInt64, StatsIndexList> StatsMetricWithKey;
      typedef std::unordered_map<std::string, StatsMetricWithKey> StatsMetricList;
      typedef std::unordered_map<std::string, StatsMetricList> StatsObjectList;
      StatsObjectList m_objects;

      void recordStatsBase();
      void db_write(std::string key, std::string data);
};

template <class T> void registerStatsMetric(String objectName, UInt32 index, String metricName, T *metric)
{
   Sim()->getStatsManager()->registerMetric(new StatsMetric<T>(objectName, index, metricName, metric));
}


class StatHist {
  private:
    static const int HIST_MAX = 20;
    unsigned long n, s, s2, min, max;
    unsigned long hist[HIST_MAX];
    char dummy[64];
  public:
    StatHist() : n(0), s(0), s2(0), min(0), max(0) { bzero(hist, sizeof(hist)); }
    StatHist & operator += (StatHist & stat);
    void update(unsigned long v);
    void print();
};
