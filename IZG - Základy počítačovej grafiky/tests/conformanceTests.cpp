#include <iostream>
#include <tests/conformanceTests.hpp>

#define CATCH_CONFIG_RUNNER
#include <tests/catch.hpp>

std::string groundTruthFile;

void runConformanceTests(std::string const&groundTruth) {
  groundTruthFile = groundTruth;
  int         argc   = 1;
  char const* argv[] = {"test"};

  Catch::Config cfg;
  auto tests = Catch::getAllTestCasesSorted(cfg);
  auto nofTests = tests.size();

  int  result = Catch::Session().run(argc, argv);
 
  size_t maxPoints = 18 + 4*3;
  std::cout << std::fixed << std::setprecision(1) << maxPoints * (float)(nofTests-result)/(float)nofTests << std::endl;
}
