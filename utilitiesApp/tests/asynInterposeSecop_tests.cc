#include <string>

#include "gtest/gtest.h"
#include "asynInterposeSecop.h"
#pragma warning( disable : 4129 )
namespace {

    TEST(SecopParseTests, test_GIVEN_action_only_THEN_parses_ok) {
        std::string action, specifier, xml_data;
        std::string secop_input("hello");
        secopParse(secop_input, action, specifier, xml_data); 
        
        // Then:
        ASSERT_EQ(action, "hello");
        ASSERT_EQ(specifier, "");
        ASSERT_EQ(xml_data, "");
    }

    TEST(SecopParseTests, test_GIVEN_action_and_specifier_THEN_parses_ok) {
        std::string action, specifier, xml_data;
        std::string secop_input("hello there");
        secopParse(secop_input, action, specifier, xml_data); 
        
        // Then:
        ASSERT_EQ(action, "hello");
        ASSERT_EQ(specifier, "there");
        ASSERT_EQ(xml_data, "");
    }

    TEST(SecopParseTests, test_GIVEN_action_specifier_and_simple_data_THEN_parses_ok) {
        std::string action, specifier, xml_data;
        std::string secop_input("hello there 1");
        secopParse(secop_input, action, specifier, xml_data); 
        
        // Then:
        ASSERT_EQ(action, "hello");
        ASSERT_EQ(specifier, "there");
        ASSERT_EQ(xml_data, "<uint>1</uint>");
    }

    TEST(SecopParseTests, test_GIVEN_action_specifier_and_array_data_THEN_parses_ok) {
        std::string action, specifier, xml_data;
        std::string secop_input("hello there [1 , 2]");
        secopParse(secop_input, action, specifier, xml_data); 
        
        // Then:
        ASSERT_EQ(action, "hello");
        ASSERT_EQ(specifier, "there");
        ASSERT_EQ(xml_data, "<array><uint>1</uint><uint>2</uint></array>");
    }

    TEST(SecopParseTests, test_GIVEN_action_specifier_and_structure_data_THEN_parses_ok) {
        std::string action, specifier, xml_data;
        std::string secop_input("hello there {\"a\" : 3, \"b\" : 6}");
        secopParse(secop_input, action, specifier, xml_data); 
        
        // Then:
        ASSERT_EQ(action, "hello");
        ASSERT_EQ(specifier, "there");
        ASSERT_EQ(xml_data, "<a><uint>3</uint></a><b><uint>2</uint></b>");
    }





}
