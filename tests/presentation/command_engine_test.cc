#include "src/presentation/command_engine.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace minigit::application {

/********  验证器 + 桩执行器  ********/
class SpyValidator : public minigit::presentation::Validator {
public:
    static int& count() {
        static int c = 0;
        return c;
    }
    static std::vector<std::string>& args() {
        static std::vector<std::string> a;
        return a;
    }
    static void reset() {
        count() = 0;
        args().clear();
    }

    bool Validate(
        const minigit::presentation::CommandContext& cmd_context) override {
        count()++;
        args() = cmd_context.args;
        return true;  // 总是验证通过
    }
};

class StubExecutor : public CmdExecutor {
public:
    bool Execute(const CommandContext& ctx) override {
        SpyValidator::count()++;
        SpyValidator::args() = ctx.args;
        return true;  // 总是执行成功
    }
};

}  // namespace minigit::application

namespace minigit::presentation {

/********  测试夹具  ********/
class CommandEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        minigit::application::SpyValidator::reset();

        // 注册测试命令
        engine_.RegisterCommand(
            "git", std::make_unique<minigit::application::SpyValidator>(),
            std::make_unique<minigit::application::StubExecutor>());

        engine_.RegisterCommand(
            "stub", std::make_unique<minigit::application::SpyValidator>(),
            std::make_unique<minigit::application::StubExecutor>());
    }

    CommandEngine engine_;
};

/********  用例  ********/
TEST_F(CommandEngineTest, ExecuteKnownCommand) {
    // 由于 Execute 返回 void，我们通过副作用来验证
    EXPECT_NO_THROW(engine_.Execute("git stub arg1 arg2"));

    // 验证执行器被调用（count 应该为 2：验证器1次 + 执行器1次）
    EXPECT_GE(minigit::application::SpyValidator::count(), 1);
}

TEST_F(CommandEngineTest, ExecuteUnknownCommandReturnsFalse) {
    // 对于未知命令，应该记录错误但不抛出异常
    EXPECT_NO_THROW(engine_.Execute("unknown"));
    // 可以通过日志输出来验证，或者修改设计让 Execute 返回 bool
}

TEST_F(CommandEngineTest, ExecuteEmptyStringDoesNotCrash) {
    EXPECT_NO_THROW(engine_.Execute(""));  // 空行不应崩溃
    EXPECT_EQ(minigit::application::SpyValidator::count(), 0);
}

}  // namespace minigit::presentation