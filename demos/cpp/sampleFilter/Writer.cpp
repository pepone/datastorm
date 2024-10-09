//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
#include <DataStorm/DataStorm.h>

using namespace std;

int
main(int argc, char* argv[])
{
    try
    {
        //
        // Instantiates DataStorm node.
        //
        DataStorm::Node node(argc, argv, "config.writer");

        //
        // Instantiates the "hello" topic. The topic uses strings for keys and values.
        //
        DataStorm::Topic<string, string> topic(node, "hello");

        //
        // Configure writers to not clear the history. We want the readers to receive
        // all the writer samples.
        //
        topic.setWriterDefaultConfig({Ice::nullopt, Ice::nullopt, DataStorm::ClearHistoryPolicy::Never});

        //
        // Instantiate the foo writer and wait for a reader to connect.
        //
        auto writer = DataStorm::makeSingleKeyWriter(topic, "foo");
        topic.waitForReaders();

        //
        // Publish samples
        //
        writer.update("hi");
        writer.update("greetings");
        writer.update("good morning");
        writer.update("hello");
        writer.update("good afternoon");

        //
        // Wait for readers to disconnect.
        //
        topic.waitForNoReaders();
    }
    catch (const std::exception& ex)
    {
        cerr << ex.what() << endl;
        return 1;
    }
    return 0;
}
