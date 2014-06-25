#pragma once

namespace DVLib
{
	namespace UnitTests 
	{
		class ShellUtilUnitTests :  public CPPUNIT_NS::TestFixture
		{
			CPPUNIT_TEST_SUITE( ShellUtilUnitTests );
			CPPUNIT_TEST( testGetEnvironmentVariable );
			CPPUNIT_TEST( testExpandEnvironmentVariables );
			CPPUNIT_TEST( testDetachCmd );
			CPPUNIT_TEST( testRunCmd );
			CPPUNIT_TEST( testExecCmd );
			CPPUNIT_TEST( testShellCmd );
			CPPUNIT_TEST( RunCmd_WithHiddenWindow_DoesNotShowWindow );
			CPPUNIT_TEST( ShellCmd_WithHiddenWindow_DoesNotShowWindow );
			CPPUNIT_TEST_SUITE_END();
		public:
			void testGetEnvironmentVariable();
			void testExpandEnvironmentVariables();
			void testDetachCmd();
			void testExecCmd();
			void testShellCmd();
			void testRunCmd();
			void RunCmd_WithHiddenWindow_DoesNotShowWindow();
			void ShellCmd_WithHiddenWindow_DoesNotShowWindow();
		};
	}
}
