#include <string>

#include "gtest/gtest.h"
#include "dbLoadRecordsFuncs.h"
#pragma warning( disable : 4129 )
namespace {

    TEST(SubMacrosTest, test_GIVEN_macro_using_brackets_THEN_removes_backslash) {
        const char* macros = "string that contains \$(a macro)";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains $(a macro)");
    }

    TEST(SubMacrosTest, test_GIVEN_macro_using_brace_THEN_removes_backslash) {
        const char* macros = "string that contains \${}";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains ${}");
    }

    TEST(SubMacrosTest, test_GIVEN_nested_macros_using_brackets_THEN_removes_backslash) {
        const char* macros = "string that contains \$(\$())";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains $($())");
    }

    TEST(SubMacrosTest, test_GIVEN_nested_macro_using_brace_THEN_removes_backslash) {
        const char* macros = "string that contains \${2 \${macros}}";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains ${2 ${macros}}");
    }

    TEST(SubMacrosTest, test_GIVEN_many_nested_macros_THEN_removes_backslash) {
        const char* macros = "string that contains \${1\${2\${3\${4\${5\${6\${7\${8}}}}}}}}";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains ${1${2${3${4${5${6${7${8}}}}}}}}");
    }

    TEST(SubMacrosTest, test_GIVEN_nested_macro_using_both_THEN_removes_backslash) {
        const char* macros = "string that contains \$(2 \${macros})";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains $(2 ${macros})");
    }

    TEST(SubMacrosTest, test_GIVEN_multiple_unnested_macro_using_both_THEN_removes_backslash) {
        const char* macros = "string that contains \${1} \$(2)";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains ${1} $(2)");
    }

    TEST(SubMacrosTest, test_GIVEN_multiple_different_nested_macro_using_both_THEN_removes_backslash) {
        const char* macros = "string that contains \${1\$(2\$(3))} \$(4\${5\${6}})";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains ${1$(2$(3))} $(4${5${6}})");
    }

    TEST(SubMacrosTest, test_GIVEN_no_macros_THEN_string_does_not_change) {
        const char* macros = "string that contains no macro";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains no macro");
    }

    TEST(SubMacrosTest, test_GIVEN_macro_not_escaped_THEN_string_does_not_change) {
        const char* macros = "string that contains non-escaped $(macro)";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string that contains non-escaped $(macro)");
    }

    TEST(SubMacrosTest, test_GIVEN_no_macros_AND_has_escapes_THEN_string_does_not_change) {
        const char* macros = "string \that \contains no macro";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string \that \contains no macro");
    }

    TEST(SubMacrosTest, test_GIVEN_macro_not_escaped_AND_has_escapes_THEN_string_does_not_change) {
        const char* macros = "string \that \()contains non-escaped $(macro)";
        std::string new_macros;
        subMacros(new_macros, macros);
        
        // Then:
        ASSERT_EQ(new_macros, "string \that \()contains non-escaped $(macro)");
    }
}
