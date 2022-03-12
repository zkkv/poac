#include <boost/ut.hpp>

#include <poac/core/builder/ninja_syntax.hpp>

#include <string>
#include <sstream>
#include <vector>

static const std::string LONGWORD = std::string(10, 'a');
static const std::string LONGWORDWITHSPACES = std::string(5, 'a') + "$ " + std::string(5, 'a');
static const std::string INDENT = "    ";

int main() {
    using namespace std::literals::string_literals;
    using namespace boost::ut;
    using namespace boost::ut::spec;

    namespace ninja_syntax = poac::core::builder::ninja_syntax;
    using boost::algorithm::join;
    using vec = std::vector<std::string>;

    describe("test line word wrap") = [] {
        it("test single long word") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 8 };
            writer._line(LONGWORD);
            expect(eq(LONGWORD + '\n',writer.get_value()));
        };

        it("test few long words") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 8 };
            writer._line(join(vec{"x"s, LONGWORD, "y"s}, " "));
            expect(eq(
                join(
                    vec{
                        "x"s,
                        INDENT + LONGWORD,
                        INDENT + "y"
                    },
                    " $\n"
                ) + '\n',
                writer.get_value()
            ));
        };

        it("test comment wrap") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 8 };
            writer.comment("Hello /usr/local/build-tools/bin");
            expect(eq(
                "# Hello\n# /usr/local/build-tools/bin\n"s,
                writer.get_value()
            )) << "Filenames should not be wrapped";
        };

        it("test short words indented") = [] {
            // Test that indent is taking into account when breaking subsequent lines.
            // The second line should not be '    to tree', as that's longer than the
            // test layout width of 8.
            ninja_syntax::writer writer{ std::ostringstream(), 8 };
            writer._line("line_one to tree");
            expect(eq(
                "line_one $\n"
                "    to $\n"
                "    tree\n"s,
                writer.get_value()
            ));
        };

        it("test few long words indented") = [] {
            // Check wrapping in the presence of indenting.
            ninja_syntax::writer writer{ std::ostringstream(), 8 };
            writer._line(join(vec{"x"s, LONGWORD, "y"s}, " "), 1);
            expect(eq(
                join(
                    vec{
                        "  "s + "x",
                        "  " + INDENT + LONGWORD,
                        "  " + INDENT + "y"
                    },
                    " $\n"
                ) + '\n',
                writer.get_value()
            ));
        };

        it("test escaped spaces") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 8 };
            writer._line(join(vec{"x"s, LONGWORDWITHSPACES, "y"s}, " "));
            expect(eq(
                join(
                    vec{
                        "x"s,
                        INDENT + LONGWORDWITHSPACES,
                        INDENT + "y"
                    },
                    " $\n"
                    ) + '\n',
                writer.get_value()
            ));
        };

        it("test fit many words") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 78 };
            writer._line("command = cd ../../chrome; python ../tools/grit/grit/format/repack.py ../out/Debug/obj/chrome/chrome_dll.gen/repack/theme_resources_large.pak ../out/Debug/gen/chrome/theme_resources_large.pak", 1);
            expect(eq(
                "  command = cd ../../chrome; python ../tools/grit/grit/format/repack.py $\n"
                "      ../out/Debug/obj/chrome/chrome_dll.gen/repack/theme_resources_large.pak $\n"
                "      ../out/Debug/gen/chrome/theme_resources_large.pak\n"s,
                writer.get_value()
            ));
        };

        it("test leading space") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 14 };
            writer.variable("foo", vec{"", "-bar", "-somethinglong"}, 0);
            expect(eq(
                "foo = -bar $\n"
                "    -somethinglong\n"s,
                writer.get_value()
            ));
        };

        it("test embedded dollar dollar") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 15 };
            writer.variable("foo", vec{"a$$b", "-somethinglong"}, 0);
            expect(eq(
                "foo = a$$b $\n"
                "    -somethinglong\n"s,
                writer.get_value()
            ));
        };

        it("test two embedded dollar dollars") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 17 };
            writer.variable("foo", vec{"a$$b", "-somethinglong"}, 0);
            expect(eq(
                "foo = a$$b $\n"
                "    -somethinglong\n"s,
                writer.get_value()
            ));
        };

        it("test leading dollar dollar") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 14 };
            writer.variable("foo", vec{"$$b", "-somethinglong"}, 0);
            expect(eq(
                "foo = $$b $\n"
                "    -somethinglong\n"s,
                writer.get_value()
            ));
        };

        it("test trailing dollar dollar") = [] {
            ninja_syntax::writer writer{ std::ostringstream(), 14 };
            writer.variable("foo", vec{"a$$", "-somethinglong"}, 0);
            expect(eq(
                "foo = a$$ $\n"
                "    -somethinglong\n"s,
                writer.get_value()
            ));
        };
    };

    describe("test build") = [] {
        it("test variables dict") = [] {
            ninja_syntax::writer writer{ std::ostringstream() };
            writer.build(
                std::vector<std::filesystem::path>{"out"},
                "cc",
                ninja_syntax::build_set_t{
                    .inputs = "in",
                    .variables = std::unordered_map<std::string, std::string>{
                        {"name", "value"}
                    }
                }
            );

            expect(eq(
                "build out: cc in\n"
                "  name = value\n"s,
                writer.get_value()
            ));
        };

        it("test implicit outputs") = [] {
            ninja_syntax::writer writer{ std::ostringstream() };
            writer.build(
                std::vector<std::filesystem::path>{"o"},
                "cc",
                ninja_syntax::build_set_t{
                    .inputs = "i",
                    .implicit_outputs = "io",
                }
            );

            expect(eq(
                "build o | io: cc i\n"s,
                writer.get_value()
            ));
        };
    };
}