#include "MantidQtCustomInterfaces/Reflectometry/ReflEventPresenter.h"
#include "MantidQtCustomInterfaces/Reflectometry/IReflEventTabPresenter.h"
#include "MantidQtCustomInterfaces/Reflectometry/IReflEventView.h"

#include <boost/algorithm/string.hpp>

namespace MantidQt {
namespace CustomInterfaces {

/** Constructor
* @param view :: The view we are handling
*/
ReflEventPresenter::ReflEventPresenter(IReflEventView *view) : m_view(view) {}

/** Destructor
*/
ReflEventPresenter::~ReflEventPresenter() {}

/** Returns the time-slicing options
* @return :: The time-slicing options
*/
std::string ReflEventPresenter::getTimeSlicingOptions() const {

  return m_view->getTimeSlices();
}
}
}