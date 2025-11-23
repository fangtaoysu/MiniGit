// #include "src/presentation/command_engine.h"
// #include <gtest/gtest.h>
// #include <vector>
// #include <string>

// namespace minigit::application {

// /********  验证器 + 桩执行器  ********/
// class SpyValidator {
// public:
//     static int&         count()       { static int c = 0; return c; }
//     static std::vector<std::string>& args() { static std::vector<std::string> a; return a; }
//     static void reset() { count() = 0; args().clear(); }
// };

// class StubExecutor : public CmdExecutor {
// public:
//     bool Execute(const CommandContext& ctx) override {
//         SpyValidator::count()++;
//         SpyValidator::args() = ctx.args;
//         return true;
//     }
//     const char* CommandName() const noexcept override { return "stub"; }
// };

// }  // namespace minigit::application

// namespace minigit::presentation {

// /********  测试夹具  ********/
// class CommandEngineTest : public ::testing::Test {
// protected:
//     void SetUp() override {
//         minigit::application::SpyValidator::reset();
//     }
//     CommandEngine engine_;
// };

// /********  用例  ********/
// TEST_F(CommandEngineTest, ExecuteKnownCommand) {
//     ASSERT_TRUE(engine_.Execute("git stub arg1 arg2"));
//     EXPECT_EQ(minigit::application::SpyValidator::count(), 1);
//     EXPECT_EQ(minigit::application::SpyValidator::args(),
//               std::vector<std::string>({"arg1", "arg2"}));
// }

// TEST_F(CommandEngineTest, ExecuteUnknownCommandReturnsFalse) {
//     EXPECT_FALSE(engine_.Execute("unknown"));
// }

// TEST_F(CommandEngineTest, ExecuteEmptyStringDoesNotCrash) {
//     EXPECT_TRUE(engine_.Execute(""));   // 空行视为合法，不崩溃即可
//     EXPECT_EQ(minigit::application::SpyValidator::count(), 0);
// }

// }  // namespace minigit::presentation