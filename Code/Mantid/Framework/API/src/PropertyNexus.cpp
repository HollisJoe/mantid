#include "MantidAPI/PropertyNexus.h"
#include "MantidKernel/System.h"
#include "MantidNexusCPP/NeXusException.hpp"
#include "MantidNexusCPP/NeXusFile.hpp"
#include "MantidKernel/DateAndTime.h"
#include "MantidKernel/TimeSeriesProperty.h"
#include "MantidKernel/PropertyWithValue.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include "MantidKernel/Strings.h"
#include <napi.h>

using namespace Mantid::Kernel;
using namespace ::NeXus;
using boost::algorithm::is_any_of;
using boost::algorithm::split;

namespace Mantid
{
namespace API
{

namespace PropertyNexus
{


  //----------------------------------------------------------------------------------------------
  /** Helper method to create a property
   *
   * @param file :: nexus file handle
   * @param name :: name of the property being created
   * @param times :: vector of times, empty = single property with value
   * @return Property *
   */
  template <typename NumT>
  Property * makeProperty(::NeXus::File * file, const std::string & name,  std::vector<Kernel::DateAndTime> & times)
  {

    std::vector<NumT> values;
    file->getData(values);
    if (times.empty())
      return new PropertyWithValue<NumT>(name, values[0]);
    else
    {
      TimeSeriesProperty<NumT> * prop = new TimeSeriesProperty<NumT>(name);
      prop->addValues(times, values);
      return prop;
    }
  }


  /** Make a string/vector\<string\> property */
  Property * makeStringProperty(::NeXus::File * file, const std::string & name,  std::vector<Kernel::DateAndTime> & times)
  {
    std::vector<std::string> values;
    if (times.empty())
    {
      std::string bigString = file->getStrData();
      return new PropertyWithValue<std::string>(name, bigString);
    }
    else
    {
      if (file->getInfo().dims.size() != 2)
        throw std::runtime_error("NXlog loading failed on field " + name + ". Expected rank 2.");
      int numStrings = file->getInfo().dims[0];
      int span = file->getInfo().dims[1];
      char * data = new char[numStrings*span];
      file->getData(data);
      values.reserve(size_t(numStrings));
      for (int i=0; i<numStrings; i++)
        values.push_back( std::string(data + i*span) );

      TimeSeriesProperty<std::string> * prop = new TimeSeriesProperty<std::string>(name);
      //boost::split(values, bigString, boost::is_any_of("\n"));
      prop->addValues(times, values);
      return prop;
    }
  }


  //----------------------------------------------------------------------------------------------
  /** Opens a NXlog group in a nexus file and
   * creates the correct Property object from it.
   *
   * @param file :: NXS file handle
   * @param group :: name of NXlog group to open
   * @return Property pointer
   */
  Property * loadProperty(::NeXus::File * file, const std::string & group)
  {
    file->openGroup(group, "NXlog");

    // Times in second offsets
    std::vector<double> timeSec;
    std::string startStr = "";

    // Get the entries so that you can check if the "time" field is present
    std::map<std::string, std::string> entries = file->getEntries();
    if (entries.find("time") != entries.end())
    {
      file->openData("time");
      file->getData(timeSec);
      // Optionally get a start
      try
      { file->getAttr("start", startStr); }
      catch (::NeXus::Exception &) {}
      file->closeData();
    }

    std::vector<Kernel::DateAndTime> times;
    if (!timeSec.empty())
    {
      // Use a default start time
      if (startStr.empty())
        startStr = "2000-01-01T00:00:00";
      //Convert time in seconds to DateAndTime
      DateAndTime start(startStr);
      times.reserve(timeSec.size());
      for (size_t i=0; i<timeSec.size(); i++)
        times.push_back( start + timeSec[i] );
    }

    file->openData("value");
    Property * retVal = NULL;
    switch (file->getInfo().type)
    {
    case ::NeXus::FLOAT32:
      retVal = makeProperty<float>(file, group, times);
      break;
    case ::NeXus::FLOAT64:
      retVal = makeProperty<double>(file, group, times);
      break;
    case ::NeXus::INT32:
      retVal = makeProperty<int32_t>(file, group, times);
      break;
    case ::NeXus::UINT32:
      retVal = makeProperty<uint32_t>(file, group, times);
      break;
    case ::NeXus::INT64:
      retVal = makeProperty<int64_t>(file, group, times);
      break;
    case ::NeXus::UINT64:
      retVal = makeProperty<uint64_t>(file, group, times);
      break;
    case ::NeXus::CHAR:
      retVal = makeStringProperty(file, group, times);
      break;
    case ::NeXus::INT8:
    case ::NeXus::UINT8:
    case ::NeXus::INT16:
    case ::NeXus::UINT16:
      retVal = NULL;
      break;
    }

    file->closeData();
    file->closeGroup();
    return retVal;
  }


  //----------------------------------------------------------------------------------------------
  /** Helper function to save a PropertyWithValue<> */
  template <typename NumT>
  void savePropertyWithValue(::NeXus::File * file, PropertyWithValue<NumT> * prop)
  {
    file->makeGroup(prop->name(), "NXlog", 1);
    file->writeData("value", (*prop)() );
    file->closeGroup();
  }

  //----------------------------------------------------------------------------------------------
  /** Helper function to save a PropertyWithValue<> */
  void savePropertyWithValueString(::NeXus::File * file, PropertyWithValue<std::string> * prop)
  {
    file->makeGroup(prop->name(), "NXlog", 1);
    file->writeData("value", prop->value() );
    file->closeGroup();
  }


  //----------------------------------------------------------------------------------------------
  /** Saves the time vector has time + start attribute */
  template <typename NumT>
  void saveTimeVector(::NeXus::File * file, TimeSeriesProperty<NumT> * prop)
  {
    std::vector<DateAndTime> times = prop->timesAsVector();
    DateAndTime start = times[0];
    std::vector<double> timeSec(times.size());
    for (size_t i=0; i<times.size(); i++)
      timeSec[i] = double(times[i].totalNanoseconds() - start.totalNanoseconds()) * 1e-9;
    file->writeData("time", timeSec);
    file->openData("time");
    file->putAttr("start", start.toISO8601String() );
    file->closeData();
  }

  //----------------------------------------------------------------------------------------------
  /** Helper function to save a TimeSeriesProperty<> */
  template <typename NumT>
  void saveTimeSeriesProperty(::NeXus::File * file, TimeSeriesProperty<NumT> * prop)
  {
    std::vector<NumT> value = prop->valuesAsVector();
    if( value.empty() ) return;
    file->makeGroup(prop->name(), "NXlog", 1);
    file->writeData("value", value );
    saveTimeVector(file, prop);
    file->closeGroup();
  }

  //----------------------------------------------------------------------------------------------
  /** Helper function to save a TimeSeriesProperty<> */
  void saveTimeSeriesPropertyString(::NeXus::File * file, TimeSeriesProperty<std::string> * prop)
  {
    std::vector<std::string> values = prop->valuesAsVector();
    if( values.empty() ) return;
    file->makeGroup(prop->name(), "NXlog", 1);

    // Find the max length of any string
    size_t maxlen=0;
    for(size_t i=0;i<values.size();i++)
      if(values[i].size()>maxlen) maxlen=values[i].size();
    // Increment by 1 to have the 0 terminator
    maxlen++;
    // Copy into one array
    char* strs=new char[values.size()*maxlen];
    memset(strs, 0, values.size()*maxlen);
    for(size_t i=0;i<values.size();i++)
      strncpy(&strs[i*maxlen],values[i].c_str(),values[i].size());

    std::vector<int> dims;
    dims.push_back( int(values.size()) );
    dims.push_back( int(maxlen) );
    file->makeData("value", ::NeXus::CHAR, dims, true);
    file->putData( (void*) strs );
    file->closeData();
    saveTimeVector(file, prop);
    file->closeGroup();
    delete [] strs;
  }

  //----------------------------------------------------------------------------------------------
  void saveProperty(::NeXus::File * file, Property * prop)
  {
    { PropertyWithValue<std::string> * p = dynamic_cast<PropertyWithValue<std::string>*>(prop);
      if (p) { savePropertyWithValueString(file, p); return; } }

    { PropertyWithValue<float> * p = dynamic_cast<PropertyWithValue<float>*>(prop);
      if (p) { savePropertyWithValue(file, p); return; } }
    { PropertyWithValue<double> * p = dynamic_cast<PropertyWithValue<double>*>(prop);
      if (p) { savePropertyWithValue(file, p); return; } }
    { PropertyWithValue<int> * p = dynamic_cast<PropertyWithValue<int>*>(prop);
      if (p) { savePropertyWithValue(file, p); return; } }
    { PropertyWithValue<int32_t> * p = dynamic_cast<PropertyWithValue<int32_t>*>(prop);
      if (p) { savePropertyWithValue(file, p); return; } }
    { PropertyWithValue<uint32_t> * p = dynamic_cast<PropertyWithValue<uint32_t>*>(prop);
      if (p) { savePropertyWithValue(file, p); return; } }
    { PropertyWithValue<int64_t> * p = dynamic_cast<PropertyWithValue<int64_t>*>(prop);
      if (p) { savePropertyWithValue(file, p); return; } }
    { PropertyWithValue<uint64_t> * p = dynamic_cast<PropertyWithValue<uint64_t>*>(prop);
      if (p) { savePropertyWithValue(file, p); return; } }

    { TimeSeriesProperty<std::string> * p = dynamic_cast<TimeSeriesProperty<std::string>*>(prop);
      if (p) { saveTimeSeriesPropertyString(file, p); return; } }

    { TimeSeriesProperty<float> * p = dynamic_cast<TimeSeriesProperty<float>*>(prop);
      if (p) { saveTimeSeriesProperty(file, p); return; } }
    { TimeSeriesProperty<double> * p = dynamic_cast<TimeSeriesProperty<double>*>(prop);
      if (p) { saveTimeSeriesProperty(file, p); return; } }
    { TimeSeriesProperty<int> * p = dynamic_cast<TimeSeriesProperty<int>*>(prop);
      if (p) { saveTimeSeriesProperty(file, p); return; } }
    { TimeSeriesProperty<int32_t> * p = dynamic_cast<TimeSeriesProperty<int32_t>*>(prop);
      if (p) { saveTimeSeriesProperty(file, p); return; } }
    { TimeSeriesProperty<uint32_t> * p = dynamic_cast<TimeSeriesProperty<uint32_t>*>(prop);
      if (p) { saveTimeSeriesProperty(file, p); return; } }
    { TimeSeriesProperty<int64_t> * p = dynamic_cast<TimeSeriesProperty<int64_t>*>(prop);
      if (p) { saveTimeSeriesProperty(file, p); return; } }
    { TimeSeriesProperty<uint64_t> * p = dynamic_cast<TimeSeriesProperty<uint64_t>*>(prop);
      if (p) { saveTimeSeriesProperty(file, p); return; } }

  }

}// namespace PropertyNexus

} // namespace Mantid
} // namespace API



