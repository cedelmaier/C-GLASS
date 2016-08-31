#ifndef _SIMCORE_TEST_XLINK_KMC_H_
#define _SIMCORE_TEST_XLINK_KMC_H_

#include "xlink_kmc.h"
#include "space.h"
#include "test_module_base.h"

#include <functional>

class BrRod;
class Xlink;

class TestXlinkKMC : public TestModuleBase, public XlinkKMC {
  public:
    TestXlinkKMC() : TestModuleBase(), XlinkKMC() {}
    virtual ~TestXlinkKMC() {}

    virtual void InitTestModule(const std::string& filename);
    virtual void RunTests();
    virtual void UnitTests();
    virtual void IntegrationTests();

  protected:

    bool FailTest(int test_num);
    bool UnitTestCalcCutoff(int test_num);
    bool UnitTestUpdate_0_1(int test_num);
    bool UnitTestUpdate_1_2(int test_num);
    bool UnitTestDetach_1_0(int test_num);
    bool UnitTestDetach_2_1(int test_num);
    bool UnitTestDetach_2_0(int test_num);
    bool UnitTestUpdateStage1(int test_num);
    bool UnitTestUpdateStage2(int test_num);
    bool UnitTestPolarAffinity(int test_num);

    void CreateTestRod(BrRod **rod,
                       const std::string &unitname,
                       const std::string &rodname,
                       int itest);

    void CreateTestXlink(Xlink **mxit,
                         const std::string &unitname,
                         const std::string &xname,
                         int itest,
                         int attachoid);

    void CreateTestXlink(Xlink **mxit,
                         const std::string &unitname,
                         const std::string &xname,
                         int itest,
                         int attachoid0,
                         int attachoid1);

    std::vector<std::function<bool(int)>> unit_tests_;
    std::vector<std::string> unit_tests_names_;
    std::vector<std::vector<bool>> unit_tests_results_;

    system_parameters params_sub_;
    SpaceProperties space_sub_;
};

#endif /* _SIMCORE_TEST_XLINK_KMC_H_ */
