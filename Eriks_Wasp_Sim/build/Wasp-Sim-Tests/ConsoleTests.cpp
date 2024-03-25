#include "pch.h"
#include "CppUnitTest.h"
#include "Console.h"
#include "CommandUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ConsoleTests
{
	// TESTING UTIL
	std::streambuf* originalCoutBuffer;

	static std::stringstream* createAndInjectCoutBuffer()
	{
		originalCoutBuffer = std::cout.rdbuf();
		std::stringstream* buffer = new std::stringstream();
		std::cout.rdbuf(buffer->rdbuf());
		return buffer;
	}

	static void restoreOriginalCoutBuffer()
	{
		std::cout.rdbuf(originalCoutBuffer);
	}

	//TESTS
	TEST_CLASS(ConsoleTests)
	{
		public:
		
			TEST_METHOD(TestCallingHandlersCorrectly)
			{
                CommandHandlerMap commandHandlers;
                bool handlerCalled = false;
                commandHandlers["test"] = [&handlerCalled](const std::string& subcommand)
                    {
                        handlerCalled = true;
                    };

                Console::processCommand("test", commandHandlers);
                Assert::IsTrue(handlerCalled);

				handlerCalled = false;
				Console::processCommand("  test qdwd qwdqwd  ", commandHandlers);
				Assert::IsTrue(handlerCalled);

				handlerCalled = false;
				Console::processCommand("notTest", commandHandlers);
				Assert::IsFalse(handlerCalled);
			}

			TEST_METHOD(TestProccessCommandOutput)
			{
				std::stringstream* buffer;

				// Get syntax error print string
				buffer = createAndInjectCoutBuffer();
				CommandUtil::printInvalidSyntaxError();
				std::string errorPrint = buffer->str();
				restoreOriginalCoutBuffer();

				// Test without fitting commandHandler
				buffer = createAndInjectCoutBuffer();
				CommandHandlerMap commandHandlers;
				Console::processCommand("test", commandHandlers);
				restoreOriginalCoutBuffer();

				std::string output = buffer->str();
				Assert::AreEqual(errorPrint.c_str(), output.c_str());

				// Test with fitting commandHandler
				buffer = createAndInjectCoutBuffer();
				commandHandlers["test"] = [](const std::string& subcommand){};
				Console::processCommand("test", commandHandlers);
				restoreOriginalCoutBuffer();

				output = buffer->str();
				Assert::AreEqual("", output.c_str());
			}

			TEST_METHOD(TestCorrectPassingOfSubcommand)
			{
				CommandHandlerMap commandHandlers;
				std::string passedIn;
				commandHandlers["test"] = [&passedIn](const std::string& subcommand)
					{
						passedIn = subcommand;
					};

				const std::string correctSubcommand = " this is a subcommand";
				Console::processCommand("test" + correctSubcommand, commandHandlers);
				Assert::AreEqual(correctSubcommand, passedIn);
			}
	};
}
