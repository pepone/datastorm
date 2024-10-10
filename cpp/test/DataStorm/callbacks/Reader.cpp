//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
#include <DataStorm/DataStorm.h>

#include <TestCommon.h>

using namespace DataStorm;
using namespace std;

int
main(int argc, char* argv[])
{
    Node node(argc, argv);

    ReaderConfig config;
    config.sampleCount = -1; // Unlimited sample count
    config.clearHistory = ClearHistoryPolicy::Never;

    Topic<string, int> controller(node, "controller");
    auto writers = makeSingleKeyReader(controller, "writers");
    auto readers = makeSingleKeyWriter(controller, "readers");
    readers.waitForReaders();

    Topic<string, string> topic(node, "string");

    // onSamples
    {
        {
            auto reader = makeSingleKeyReader(topic, "elem1", "", config);
            reader.waitForUnread();
            promise<void> p;
            reader.onSamples(
                [&p](const vector<Sample<string, string>>& samples)
                {
                    test(samples.size() == 1);
                    p.set_value();
                },
                [](const Sample<string, string>& sample) { test(false); });
            p.get_future().wait();
            readers.update(1);
        }
        {
            auto reader = makeSingleKeyReader(topic, "elem2", "", config);
            reader.waitForWriters();
            promise<void> p;
            reader.onSamples(
                [](const vector<Sample<string, string>>& samples) { test(false); },
                [&p](const Sample<string, string>& sample) { p.set_value(); });
            readers.update(2);
            p.get_future().wait();
            readers.update(3);
        }
        {
            auto reader = makeSingleKeyReader(topic, "elem3", "", config);
            reader.waitForUnread(3);
            promise<void> p;
            reader.onSamples(
                [&p](const vector<Sample<string, string>>& samples)
                {
                    test(samples.size() == 3);
                    p.set_value();
                },
                [](const Sample<string, string>& sample) { test(false); });
            p.get_future().wait();
            readers.update(4);
        }
    }
    // onConnectedKeys
    {
        {
            test(writers.getNextUnread().getValue() == 1);
            auto reader = makeSingleKeyReader(topic, "elem1", "", config);
            test(writers.getNextUnread().getValue() == 2);
        }
        {
            auto reader = makeSingleKeyReader(topic, "elem2", "", config);
            test(writers.getNextUnread().getValue() == 3);
        }
        {
            auto reader = makeSingleKeyReader(topic, "elem3", "", config);
            promise<bool> p1, p2, p3;
            reader.onConnectedKeys(
                [&p1](vector<string> keys) { p1.set_value(keys.empty()); },
                [&p2, &p3](CallbackReason action, string key)
                {
                    if (action == CallbackReason::Connect)
                    {
                        p2.set_value(key == "elem3");
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p3.set_value(key == "elem3");
                    }
                });
            test(p1.get_future().get());
            readers.update(1);
            test(p2.get_future().get());
            readers.update(2);
            test(p3.get_future().get());
        }
        {
            auto reader = makeSingleKeyReader(topic, "elem4", "", config);
            reader.waitForWriters();
            promise<bool> p1, p2;
            reader.onConnectedKeys(
                [&p1](vector<string> keys) { p1.set_value(!keys.empty() && keys[0] == "elem4"); },
                [&p2](CallbackReason action, string key)
                {
                    if (action == CallbackReason::Connect)
                    {
                        test(false);
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p2.set_value(key == "elem4");
                    }
                });
            test(p1.get_future().get());
            readers.update(3);
            test(p2.get_future().get());
        }
        readers.update(4);
        {
            test(writers.getNextUnread().getValue() == 1);
            auto reader = makeFilteredKeyReader(topic, Filter<string>("_regex", "elem1"), "", config);
            test(writers.getNextUnread().getValue() == 2);
        }
        {
            auto reader = makeFilteredKeyReader(topic, Filter<string>("_regex", "elem2"), "", config);
            test(writers.getNextUnread().getValue() == 3);
        }
        {
            auto reader = makeFilteredKeyReader(topic, Filter<string>("_regex", "elem3"), "", config);
            promise<bool> p1, p2, p3;
            reader.onConnectedKeys(
                [&p1](vector<string> keys) { p1.set_value(keys.empty()); },
                [&p2, &p3](CallbackReason action, string key)
                {
                    if (action == CallbackReason::Connect)
                    {
                        p2.set_value(key == "elem3");
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p3.set_value(key == "elem3");
                    }
                });
            test(p1.get_future().get());
            readers.update(1);
            test(p2.get_future().get());
            readers.update(2);
            test(p3.get_future().get());
        }
        {
            auto reader = makeFilteredKeyReader(topic, Filter<string>("_regex", "elem4"), "", config);
            reader.waitForWriters();
            promise<bool> p1, p2;
            reader.onConnectedKeys(
                [&p1](vector<string> keys) { p1.set_value(!keys.empty() && keys[0] == "elem4"); },
                [&p2](CallbackReason action, string key)
                {
                    if (action == CallbackReason::Connect)
                    {
                        test(false);
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p2.set_value(key == "elem4");
                    }
                });
            test(p1.get_future().get());
            readers.update(3);
            test(p2.get_future().get());
        }

        readers.update(4);

        {
            test(writers.getNextUnread().getValue() == 1);
            auto reader = makeSingleKeyReader(topic, "anyelem1", "", config);
            test(writers.getNextUnread().getValue() == 2);
        }

        test(writers.getNextUnread().getValue() == 3);

        {
            auto reader = makeAnyKeyReader(topic, "", config);
            promise<bool> p1, p2, p3;
            reader.onConnectedKeys(
                [&p1](vector<string> keys) { p1.set_value(keys.empty()); },
                [&p2, &p3](CallbackReason action, string key)
                {
                    if (action == CallbackReason::Connect)
                    {
                        p2.set_value(key == "anyelem3");
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p3.set_value(key == "anyelem3");
                    }
                });
            test(p1.get_future().get());
            readers.update(1);
            test(p2.get_future().get());
            readers.update(2);
            test(p3.get_future().get());
        }

        readers.update(3);
    }
    // onConnected
    {
        {
            test(writers.getNextUnread().getValue() == 1);
            auto reader = makeSingleKeyReader(topic, "elem1", "reader1", config);
            test(writers.getNextUnread().getValue() == 2);
        }
        {
            auto reader = makeSingleKeyReader(topic, "elem2", "reader2", config);
            test(writers.getNextUnread().getValue() == 3);
        }
        {
            auto reader = makeSingleKeyReader(topic, "elem3", "", config);
            promise<bool> p1, p2, p3;
            reader.onConnectedWriters(
                [&p1](vector<string> writers) { p1.set_value(writers.empty()); },
                [&p2, &p3](CallbackReason action, string writer)
                {
                    if (action == CallbackReason::Connect)
                    {
                        p2.set_value(writer == "writer1");
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p3.set_value(writer == "writer1");
                    }
                });
            test(p1.get_future().get());
            test(reader.getConnectedWriters().empty());
            readers.update(1);
            test(p2.get_future().get());
            test(reader.getConnectedWriters() == vector<string>{"writer1"});
            readers.update(2);
            test(p3.get_future().get());
        }
        {
            auto reader = makeSingleKeyReader(topic, "elem4", "", config);
            reader.waitForWriters();
            promise<bool> p1, p2;
            reader.onConnectedWriters(
                [&p1](vector<string> writers) { p1.set_value(!writers.empty() && writers[0] == "writer2"); },
                [&p2](CallbackReason action, string writer)
                {
                    if (action == CallbackReason::Connect)
                    {
                        test(false);
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p2.set_value(writer == "writer2");
                    }
                });
            test(p1.get_future().get());
            readers.update(3);
            test(p2.get_future().get());
        }

        readers.update(4);

        {
            test(writers.getNextUnread().getValue() == 1);
            auto reader = makeFilteredKeyReader(topic, Filter<string>("_regex", "elem1"), "reader1", config);
            test(writers.getNextUnread().getValue() == 2);
        }
        {
            auto reader = makeFilteredKeyReader(topic, Filter<string>("_regex", "elem2"), "reader2", config);
            test(writers.getNextUnread().getValue() == 3);
        }
        {
            auto reader = makeFilteredKeyReader(topic, Filter<string>("_regex", "elem3"), "", config);
            promise<bool> p1, p2, p3;
            reader.onConnectedWriters(
                [&p1](vector<string> writers) { p1.set_value(writers.empty()); },
                [&p2, &p3](CallbackReason action, string writer)
                {
                    if (action == CallbackReason::Connect)
                    {
                        p2.set_value(writer == "writer1");
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p3.set_value(writer == "writer1");
                    }
                });
            test(p1.get_future().get());
            readers.update(1);
            test(p2.get_future().get());
            readers.update(2);
            test(p3.get_future().get());
        }
        {
            auto reader = makeFilteredKeyReader(topic, Filter<string>("_regex", "elem4"), "", config);
            reader.waitForWriters();
            promise<bool> p1, p2;
            reader.onConnectedWriters(
                [&p1](vector<string> writers) { p1.set_value(!writers.empty() && writers[0] == "writer2"); },
                [&p2](CallbackReason action, string writer)
                {
                    if (action == CallbackReason::Connect)
                    {
                        test(false);
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p2.set_value(writer == "writer2");
                    }
                });
            test(p1.get_future().get());
            readers.update(3);
            test(p2.get_future().get());
        }

        readers.update(4);

        {
            test(writers.getNextUnread().getValue() == 1);
            auto reader = makeSingleKeyReader(topic, "anyelem1", "reader1", config);
            test(writers.getNextUnread().getValue() == 2);
        }

        test(writers.getNextUnread().getValue() == 3);

        {
            auto reader = makeAnyKeyReader(topic, "", config);
            promise<bool> p1, p2, p3;
            reader.onConnectedWriters(
                [&p1](vector<string> writers) { p1.set_value(writers.empty()); },
                [&p2, &p3](CallbackReason action, string writer)
                {
                    if (action == CallbackReason::Connect)
                    {
                        p2.set_value(writer == "writer1");
                    }
                    else if (action == CallbackReason::Disconnect)
                    {
                        p3.set_value(writer == "writer1");
                    }
                });
            test(p1.get_future().get());
            readers.update(1);
            test(p2.get_future().get());
            readers.update(2);
            test(p3.get_future().get());
        }

        readers.update(3);
    }
    return 0;
}
