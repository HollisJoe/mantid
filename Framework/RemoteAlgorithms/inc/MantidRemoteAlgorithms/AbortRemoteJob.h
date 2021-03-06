#ifndef ABORTREMOTEJOB_H_
#define ABORTREMOTEJOB_H_

#include "MantidAPI/Algorithm.h"
#include "MantidAPI/DeprecatedAlgorithm.h"

namespace Mantid {
namespace RemoteAlgorithms {

class DLLExport AbortRemoteJob : public Mantid::API::Algorithm,
                                 public API::DeprecatedAlgorithm {
public:
  /// Default constructor
  AbortRemoteJob();

  /// Algorithm's name
  const std::string name() const override { return "AbortRemoteJob"; }
  /// Summary of algorithms purpose
  const std::string summary() const override {
    return "Abort a previously submitted job.";
  }

  /// Algorithm's version
  int version() const override { return (1); }
  /// Algorithm's category for identification
  const std::string category() const override { return "Remote"; }

private:
  void init() override;
  /// Execution code
  void exec() override;
};

} // end namespace RemoteAlgorithms
} // end namespace Mantid
#endif /*ABORTREMOTEJOB_H_*/
