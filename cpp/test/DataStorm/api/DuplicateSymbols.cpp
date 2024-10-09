//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
#include <DataStorm/DataStorm.h>

using namespace DataStorm;
using namespace std;

void
testDuplicateSymbols()
{
    // Make sure these don't cause duplicate symbols errors on link
    ostringstream os;
    os << SampleEvent::Add;
    os << vector<SampleEvent>();
    Sample<string, string>* sample = nullptr;
    os << *sample;
}
