#ifndef MANTID_API_PARAMETERREFERENCE_H_
#define MANTID_API_PARAMETERREFERENCE_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAPI/DllConfig.h"
#include <string>

namespace Mantid {
namespace API {
class IFunction;
/**
    A reference to a parameter in a function. To uniquely identify a parameter
    in a composite function

    @author Roman Tolchenov, Tessella Support Services plc
    @date 26/02/2010

    Copyright &copy; 2009 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
   National Laboratory & European Spallation Source

    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    File change history is stored at: <https://github.com/mantidproject/mantid>
    Code Documentation is available at: <http://doxygen.mantidproject.org>
  */
class MANTID_API_DLL ParameterReference {
public:
  ParameterReference();
  ParameterReference(IFunction *fun, std::size_t index, bool isDefault = false);
  void setParameter(const double &value, bool isExplicitlySet = true);
  double getParameter() const;
  bool isDefault() const;
  bool isParameterOf(const IFunction *fun) const;
  virtual ~ParameterReference() = default;
  IFunction *getLocalFunction() const;
  std::size_t getLocalIndex() const;
  std::size_t parameterIndex() const;
  std::string parameterName() const;
  IFunction *ownerFunction() const;

protected:
  void reset(IFunction *fun, std::size_t index, bool isDefault = false);

private:
  /// Function-owner of this reference. parameterName() and parameterIndex()
  /// return values relative to this function.
  IFunction *m_owner;
  /// Function that together with m_index uniquely identify the parameter.
  IFunction *m_function;
  /// Index of the parameter in m_function. It is assumed that this index
  /// uniquely identifies the parameter withing m_function
  std::size_t m_index;
  /// Flag to mark as default the value of an object associated with this
  /// reference: a tie or a constraint.
  bool m_isDefault;
};

} // namespace API
} // namespace Mantid

#endif /*MANTID_API_PARAMETERREFERENCE_H_*/
