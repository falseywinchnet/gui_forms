// Bounded expressions (T2 plan, phase E5): the budget and every other defect
// is refused at compile time and changes nothing, what a program reads is
// extracted statically, each operation gives its stated value, a division by
// zero or a value that is not finite gives the declared fallback, and fixed
// tables of results have the same bits on every CI platform.
// GUI_FORMS_ENGINE_IDENTITY_PRINT=1 prints the observed hashes instead of
// checking them.

#include "gui_forms/engine/engine_expression/engine_expression.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <span>
#include <vector>

// The identity tables' inputs are computed here as row × step − offset. Clang
// contracts that into a fused multiply-add on arm64 and not on baseline
// x86-64, which would give the expressions different inputs on different
// hosts, so contraction is off for this file as it is for the engine.
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

bool print_mode() {
    const char* const value = std::getenv("GUI_FORMS_ENGINE_IDENTITY_PRINT");
    const bool printing = value != nullptr && std::strcmp(value, "1") == 0;
    return printing;
}

// FNV-1a over the bytes of each double, in order.
struct BitHash final {
    std::uint64_t value{14695981039346656037ULL};

    void add(double number) {
        std::uint64_t bits = 0U;
        std::memcpy(&bits, &number, sizeof bits);
        for (int shift = 0; shift < 64; shift += 8) {
            const std::uint64_t byte = (bits >> shift) & 0xFFU;
            value = value ^ byte;
            value = value * 1099511628211ULL;
        }
    }
};

using Instruction = ExpressionInstruction;
using Operation = ExpressionOperation;

constexpr ExpressionLimits test_limits{8U, 4U};

Instruction number(double value) {
    return Instruction::number(value);
}

Instruction input(std::uint8_t index) {
    return Instruction::input(ExpressionInputIndex{index});
}

Instruction parameter(std::uint8_t index) {
    return Instruction::parameter(ExpressionParameterIndex{index});
}

Instruction apply(Operation operation) {
    return Instruction::apply(operation);
}

EngineExpression compiled(std::span<const Instruction> program, double fallback) {
    EngineExpression expression{};
    const ExpressionCompilation compilation = expression.compile(program, fallback, test_limits);
    require(compilation.defect == ExpressionDefect::none, "a valid program compiles");
    return expression;
}

// A reference expression a refusal must leave untouched.
EngineExpression reference_expression() {
    const std::array<Instruction, 3> program{input(1), number(2.0), apply(Operation::multiply)};
    const EngineExpression expression = compiled(program, -1.0);
    return expression;
}

void require_refused(std::span<const Instruction> program, double fallback, ExpressionLimits limits,
                     ExpressionDefect expected, std::size_t instruction, const char* message) {
    EngineExpression expression = reference_expression();
    const EngineExpression before = expression;
    const ExpressionCompilation compilation = expression.compile(program, fallback, limits);
    require(compilation.defect == expected, message);
    require(compilation.instruction == instruction, "the refusal names the instruction it concerns");
    require(expression == before, "a refused program leaves the expression unchanged");
}

struct Environment final {
    std::array<double, 8> inputs{};
    std::array<double, 4> parameters{};
    AnatomyStateSet states{};

    // Borrows this environment's arrays, so it refuses a temporary.
    [[nodiscard]] ExpressionEnvironment view() const& noexcept {
        ExpressionEnvironment environment{};
        environment.inputs = inputs;
        environment.parameters = parameters;
        environment.states = states;
        return environment;
    }
    ExpressionEnvironment view() const&& = delete;
};

double evaluate(std::span<const Instruction> program, const Environment& environment) {
    const EngineExpression expression = compiled(program, -99.0);
    const ExpressionResult result = expression.evaluate(environment.view());
    require(result.fallback == ExpressionFallback::none, "the program evaluates without a fallback");
    return result.value;
}

double binary(Operation operation, double a, double b) {
    const std::array<Instruction, 3> program{number(a), number(b), apply(operation)};
    const Environment environment{};
    const double value = evaluate(program, environment);
    return value;
}

double ternary(Operation operation, double a, double b, double c) {
    const std::array<Instruction, 4> program{number(a), number(b), number(c), apply(operation)};
    const Environment environment{};
    const double value = evaluate(program, environment);
    return value;
}

void test_budget() {
    // 1 + 1 + ... (32 ones, 31 additions) and a negation: exactly the budget.
    std::vector<Instruction> program{number(1.0)};
    for (int term = 1; term < 32; ++term) {
        program.push_back(number(1.0));
        program.push_back(apply(Operation::add));
    }
    program.push_back(apply(Operation::negate));
    require(program.size() == maximum_expression_operations, "the program fills the budget");
    EngineExpression expression{};
    require(expression.compile(program, 0.0, test_limits).defect == ExpressionDefect::none,
            "a program of exactly the budget compiles");
    require(expression.operation_count() == maximum_expression_operations, "it keeps every instruction");
    const Environment environment{};
    require(expression.evaluate(environment.view()).value == -32.0, "it evaluates to -32");
    program.push_back(apply(Operation::negate));
    require_refused(program, 0.0, test_limits, ExpressionDefect::over_budget, 0U,
                    "one instruction over the budget is refused at load");
}

void test_defects() {
    const std::array<Instruction, 0> empty{};
    require_refused(empty, 0.0, test_limits, ExpressionDefect::empty_program, 0U, "an empty program is refused");
    Instruction unknown = number(0.0);
    unknown.operation = static_cast<Operation>(200);
    const std::array<Instruction, 1> bad_operation{unknown};
    require_refused(bad_operation, 0.0, test_limits, ExpressionDefect::unknown_operation, 0U,
                    "an unknown operation is refused");
    const std::array<Instruction, 3> not_finite{number(1.0), number(std::numeric_limits<double>::infinity()),
                                                apply(Operation::add)};
    require_refused(not_finite, 0.0, test_limits, ExpressionDefect::constant_not_finite, 1U,
                    "a constant that is not finite is refused");
    const std::array<Instruction, 1> one{number(1.0)};
    require_refused(one, std::numeric_limits<double>::quiet_NaN(), test_limits,
                    ExpressionDefect::fallback_not_finite, 0U, "a fallback that is not finite is refused");
    require_refused(one, 0.0, ExpressionLimits{maximum_expression_inputs + 1U, 0U},
                    ExpressionDefect::limits_out_of_range, 0U, "limits beyond the input bound are refused");
    require_refused(one, 0.0, ExpressionLimits{0U, maximum_expression_parameters + 1U},
                    ExpressionDefect::limits_out_of_range, 0U, "limits beyond the parameter bound are refused");
    const std::array<Instruction, 3> far_input{number(1.0), input(8), apply(Operation::add)};
    require_refused(far_input, 0.0, test_limits, ExpressionDefect::input_out_of_range, 1U,
                    "an input past the limits is refused");
    Instruction far_state = Instruction::state(AnatomyState::hot);
    far_state.index = static_cast<std::uint8_t>(anatomy_state_count);
    const std::array<Instruction, 1> bad_state{far_state};
    require_refused(bad_state, 0.0, test_limits, ExpressionDefect::state_out_of_range, 0U,
                    "a state outside the vocabulary is refused");
    const std::array<Instruction, 1> far_parameter{parameter(4)};
    require_refused(far_parameter, 0.0, test_limits, ExpressionDefect::parameter_out_of_range, 0U,
                    "a parameter past the limits is refused");
    const std::array<Instruction, 3> short_mix{number(1.0), number(2.0), apply(Operation::mix)};
    require_refused(short_mix, 0.0, test_limits, ExpressionDefect::missing_operand, 2U,
                    "an operation without its operands is refused");
    std::vector<Instruction> deep{};
    for (std::size_t push = 0; push <= maximum_expression_stack; ++push) deep.push_back(number(1.0));
    require_refused(deep, 0.0, test_limits, ExpressionDefect::stack_too_deep, maximum_expression_stack,
                    "a stack deeper than its bound is refused");
    const std::array<Instruction, 2> two{number(1.0), number(2.0)};
    require_refused(two, 0.0, test_limits, ExpressionDefect::unbalanced, 1U,
                    "a program leaving two values is refused");
}

void test_dependencies() {
    // choose(state hot, mix(input 3, input 7, parameter 2), input 3 * 0.5)
    const std::array<Instruction, 10> program{
        Instruction::state(AnatomyState::hot), input(3), input(7), parameter(2), apply(Operation::mix),
        input(3), number(0.5), apply(Operation::multiply), apply(Operation::choose),
        apply(Operation::negate)};
    const EngineExpression expression = compiled(program, 0.0);
    const ExpressionDependencies& reads = expression.dependencies();
    require(reads.inputs == ((std::uint64_t{1} << 3U) | (std::uint64_t{1} << 7U)), "inputs 3 and 7 are read");
    require(reads.reads_input(ExpressionInputIndex{3}) && !reads.reads_input(ExpressionInputIndex{4}),
            "reads_input answers by position");
    require(reads.parameters == (std::uint32_t{1} << 2U), "parameter 2 is read");
    require(reads.reads_parameter(ExpressionParameterIndex{2}) && !reads.reads_parameter(ExpressionParameterIndex{0}),
            "reads_parameter answers by position");
    AnatomyStateSet hot{};
    hot.insert(AnatomyState::hot);
    require(reads.states == hot, "only hot is read");
    // Both branches of a choose are dependencies, whichever is taken.
    Environment environment{};
    environment.inputs[3] = 4.0;
    environment.inputs[7] = 8.0;
    environment.parameters[2] = 0.25;
    require(expression.evaluate(environment.view()).value == -2.0, "not hot: the second branch");
    environment.states.insert(AnatomyState::hot);
    require(expression.evaluate(environment.view()).value == -5.0, "hot: the mix");
    const std::array<Instruction, 1> constant{number(3.0)};
    const EngineExpression none = compiled(constant, 0.0);
    require(none.dependencies() == ExpressionDependencies{}, "a constant reads nothing");
    const EngineExpression initial{};
    require(initial.evaluate(ExpressionEnvironment{}).value == 1.0, "the default expression is 1");
    require(initial.dependencies() == ExpressionDependencies{}, "and reads nothing");
}

void test_operations() {
    require(binary(Operation::add, 1.5, 2.25) == 3.75, "add");
    require(binary(Operation::subtract, 1.5, 2.25) == -0.75, "subtract");
    require(binary(Operation::multiply, 1.5, 2.25) == 3.375, "multiply");
    require(binary(Operation::divide, 3.0, 4.0) == 0.75, "divide");
    require(binary(Operation::minimum, 3.0, -4.0) == -4.0, "minimum");
    require(binary(Operation::maximum, 3.0, -4.0) == 3.0, "maximum");
    require(binary(Operation::less, 1.0, 2.0) == 1.0 && binary(Operation::less, 2.0, 2.0) == 0.0, "less");
    require(binary(Operation::less_equal, 2.0, 2.0) == 1.0 && binary(Operation::less_equal, 3.0, 2.0) == 0.0,
            "less_equal");
    require(binary(Operation::greater, 3.0, 2.0) == 1.0 && binary(Operation::greater, 2.0, 2.0) == 0.0,
            "greater");
    require(binary(Operation::greater_equal, 2.0, 2.0) == 1.0 && binary(Operation::greater_equal, 1.0, 2.0) == 0.0,
            "greater_equal");
    require(binary(Operation::equal, 2.0, 2.0) == 1.0 && binary(Operation::equal, 2.0, 2.5) == 0.0, "equal");
    require(binary(Operation::not_equal, 2.0, 2.5) == 1.0 && binary(Operation::not_equal, 2.0, 2.0) == 0.0,
            "not_equal");
    require(ternary(Operation::clamp, 5.0, 0.0, 1.0) == 1.0 && ternary(Operation::clamp, -5.0, 0.0, 1.0) == 0.0 &&
                ternary(Operation::clamp, 0.25, 0.0, 1.0) == 0.25,
            "clamp");
    require(ternary(Operation::clamp, 0.5, 2.0, 1.0) == 1.0, "a clamp whose low passes its high gives the high");
    require(ternary(Operation::mix, 2.0, 6.0, 0.25) == 3.0 && ternary(Operation::mix, 2.0, 6.0, 1.5) == 8.0,
            "mix, with t unclamped");
    require(ternary(Operation::smoothstep, 0.0, 1.0, 0.5) == 0.5, "smoothstep at the middle");
    require(ternary(Operation::smoothstep, 0.0, 1.0, -1.0) == 0.0 && ternary(Operation::smoothstep, 0.0, 1.0, 2.0) == 1.0,
            "smoothstep clamps outside its edges");
    require(ternary(Operation::smoothstep, 0.0, 2.0, 0.5) == 0.15625, "smoothstep is u²(3 − 2u)");
    require(ternary(Operation::smoothstep, 1.0, 0.0, 0.25) == ternary(Operation::smoothstep, 0.0, 1.0, 0.75),
            "smoothstep with reversed edges falls");
    require(ternary(Operation::choose, 1.0, 7.0, 9.0) == 7.0 && ternary(Operation::choose, 0.0, 7.0, 9.0) == 9.0 &&
                ternary(Operation::choose, -0.5, 7.0, 9.0) == 7.0,
            "choose takes the first value for any condition but 0");
    const std::array<Instruction, 2> negation{number(2.5), apply(Operation::negate)};
    const Environment environment{};
    require(evaluate(negation, environment) == -2.5, "negate");
}

void test_fallbacks() {
    const double declared = 0.375;
    EngineExpression expression{};
    const std::array<Instruction, 3> ratio{input(0), input(1), apply(Operation::divide)};
    require(expression.compile(ratio, declared, test_limits).defect == ExpressionDefect::none, "a ratio compiles");
    Environment environment{};
    environment.inputs[0] = 1.0;
    environment.inputs[1] = 0.0;
    ExpressionResult result = expression.evaluate(environment.view());
    require(result.value == declared && result.fallback == ExpressionFallback::division_by_zero,
            "a division by zero gives the declared fallback");
    environment.inputs[1] = -0.0;
    result = expression.evaluate(environment.view());
    require(result.value == declared && result.fallback == ExpressionFallback::division_by_zero,
            "so does a division by negative zero");
    environment.inputs[0] = 0.0;
    environment.inputs[1] = 0.0;
    result = expression.evaluate(environment.view());
    require(result.value == declared && result.fallback == ExpressionFallback::division_by_zero,
            "so does zero over zero");
    environment.inputs[0] = std::numeric_limits<double>::quiet_NaN();
    environment.inputs[1] = 2.0;
    result = expression.evaluate(environment.view());
    require(result.value == declared && result.fallback == ExpressionFallback::not_finite,
            "a NaN input gives the declared fallback");
    environment.inputs[0] = std::numeric_limits<double>::infinity();
    result = expression.evaluate(environment.view());
    require(result.value == declared && result.fallback == ExpressionFallback::not_finite,
            "an infinite input gives the declared fallback");
    environment.inputs[0] = 1e300;
    environment.inputs[1] = 1e-300;
    result = expression.evaluate(environment.view());
    require(result.value == declared && result.fallback == ExpressionFallback::not_finite,
            "a result that overflows gives the declared fallback");
    environment.inputs[0] = 3.0;
    environment.inputs[1] = 4.0;
    result = expression.evaluate(environment.view());
    require(result.value == 0.75 && result.fallback == ExpressionFallback::none, "finite inputs give the value");
    const std::array<Instruction, 4> flat{number(0.5), number(0.5), input(0), apply(Operation::smoothstep)};
    EngineExpression edges{};
    require(edges.compile(flat, declared, test_limits).defect == ExpressionDefect::none, "a smoothstep compiles");
    result = edges.evaluate(environment.view());
    require(result.value == declared && result.fallback == ExpressionFallback::division_by_zero,
            "a smoothstep with equal edges gives the declared fallback");
    const std::array<double, 1> short_inputs{1.0};
    ExpressionEnvironment short_environment = environment.view();
    short_environment.inputs = short_inputs;
    result = expression.evaluate(short_environment);
    require(result.value == declared && result.fallback == ExpressionFallback::environment_too_short,
            "an environment shorter than the limits gives the declared fallback");
}

// The bits of fixed tables, the same on every host. The expected values were
// produced on Windows x64 and are checked by every CI platform.
constexpr std::array<std::uint64_t, 4> expected_program_hashes{
    0x313896f989da0652ULL,
    0x72a3e451170377eeULL,
    0xd7080b84ce9692b2ULL,
    0x8f590eea2e5bd78aULL,
};

void test_identity() {
    // A sheen's offset: mix(p0, p1, smoothstep(0, p2, x)) − y × 0.125.
    const std::array<Instruction, 11> sheen{
        parameter(0), parameter(1), number(0.0), parameter(2), input(0), apply(Operation::smoothstep),
        apply(Operation::mix), input(1), number(0.125), apply(Operation::multiply), apply(Operation::subtract)};
    // Division and rounding: x ÷ (y² + 1.5) × 0.37 − x × y ÷ 7.
    const std::array<Instruction, 15> ratio{
        input(0), input(1), input(1), apply(Operation::multiply), number(1.5), apply(Operation::add),
        apply(Operation::divide), number(0.37), apply(Operation::multiply), input(0), input(1),
        apply(Operation::multiply), number(7.0), apply(Operation::divide), apply(Operation::subtract)};
    // choose(x > y, clamp(x × 0.3, −0.5, 0.75), y ÷ 3).
    const std::array<Instruction, 13> branch{
        input(0), input(1), apply(Operation::greater), input(0), number(0.3), apply(Operation::multiply),
        number(-0.5), number(0.75), apply(Operation::clamp), input(1), number(3.0), apply(Operation::divide),
        apply(Operation::choose)};
    // A polynomial: ((x × 1.1 + 0.3) × x − 0.7) × x ÷ 3, the products not
    // fused.
    const std::array<Instruction, 13> polynomial{
        input(0), number(1.1), apply(Operation::multiply), number(0.3), apply(Operation::add), input(0),
        apply(Operation::multiply), number(0.7), apply(Operation::subtract), input(0), apply(Operation::multiply),
        number(3.0), apply(Operation::divide)};
    const std::array<std::span<const Instruction>, 4> programs{sheen, ratio, branch, polynomial};
    Environment environment{};
    environment.parameters[0] = 0.15;
    environment.parameters[1] = 0.85;
    environment.parameters[2] = 37.0;
    std::array<std::uint64_t, 4> observed{};
    for (std::size_t program = 0; program < programs.size(); ++program) {
        const EngineExpression expression = compiled(programs[program], -1.0);
        BitHash hash{};
        for (int row = 0; row < 211; ++row) {
            environment.inputs[1] = static_cast<double>(row) * 0.0287 - 3.03;
            for (int column = 0; column < 197; ++column) {
                environment.inputs[0] = static_cast<double>(column) * 0.4113 - 40.1;
                const ExpressionResult result = expression.evaluate(environment.view());
                hash.add(result.value);
            }
        }
        observed[program] = hash.value;
    }
    if (print_mode()) {
        for (std::size_t program = 0; program < observed.size(); ++program) {
            std::cout << "expression program " << program << " hash 0x" << std::hex << observed[program] << std::dec
                      << '\n';
        }
        return;
    }
    for (std::size_t program = 0; program < observed.size(); ++program) {
        require(observed[program] == expected_program_hashes[program],
                "the expression tables have the same bits on every host");
    }
}

} // namespace

int main() {
    test_budget();
    test_defects();
    test_dependencies();
    test_operations();
    test_fallbacks();
    test_identity();
    std::cout << "engine expression tests passed\n";
    return 0;
}
