//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

#include <random>
#include <string>
#include <thread>

#include <DataStorm/DataStorm.h>

using namespace std;

int
main(int argc, char* argv[])
{
    try
    {
        //
        // Setup a random generator to generate an identifier for the writer.
        //
        random_device rd;
        mt19937 generator(rd());
        uniform_int_distribution<> id(1);

        //
        // CtrlCHandler::maskSignals() must be called before the node or any other
        // threads are started.
        //
        DataStorm::CtrlCHandler::maskSignals();

        //
        // Instantiates node.
        //
        DataStorm::Node node(argc, argv, "config.writer");

        //
        // Shutdown the node on Ctrl-C.
        //
        DataStorm::CtrlCHandler ctrlCHandler([&node](int) { node.shutdown(); });

        //
        // Instantiates the "time" topic.
        //
        DataStorm::Topic<int, string> topic(node, "time");

        //
        // Instantiate a writer to write the local time. The writer key is a random
        // integer value that identifies the writer.
        //
        auto writer = DataStorm::makeSingleKeyWriter(topic, id(generator));

        while (!node.isShutdown())
        {
            auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
            char timeString[100];
            if (strftime(timeString, sizeof(timeString), "%x %X", localtime(&now)) == 0)
            {
                timeString[0] = '\0';
            }
            writer.update(timeString);
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
    catch (const std::exception& ex)
    {
        cerr << ex.what() << endl;
        return 1;
    }
    return 0;
}
