//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

#ifndef DATASTORM_NODESESSIONMANAGER_H
#define DATASTORM_NODESESSIONMANAGER_H

#include "DataStorm/Config.h"
#include "DataStorm/Contract.h"

#include <Ice/Ice.h>

// TODO temporary for unused parameter warnings
#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-parameter"
#endif

namespace DataStormI
{

    class NodeSessionI;
    class CallbackExecutor;
    class Instance;
    class TraceLevels;
    class NodeI;

    class NodeSessionManager : public std::enable_shared_from_this<NodeSessionManager>
    {
    public:
        NodeSessionManager(const std::shared_ptr<Instance>&, const std::shared_ptr<NodeI>&);

        void init();

        std::shared_ptr<NodeSessionI>
        createOrGet(std::optional<DataStormContract::NodePrx>, const std::shared_ptr<Ice::Connection>&, bool);

        void announceTopicReader(
            const std::string&,
            std::optional<DataStormContract::NodePrx>,
            const std::shared_ptr<Ice::Connection>& = nullptr) const;

        void announceTopicWriter(
            const std::string&,
            std::optional<DataStormContract::NodePrx>,
            const std::shared_ptr<Ice::Connection>& = nullptr) const;

        void announceTopics(
            const DataStormContract::StringSeq&,
            const DataStormContract::StringSeq&,
            std::optional<DataStormContract::NodePrx>,
            const std::shared_ptr<Ice::Connection>& = nullptr) const;

        std::shared_ptr<NodeSessionI> getSession(const Ice::Identity&) const;
        std::shared_ptr<NodeSessionI> getSession(const std::string& name) const
        {
            return getSession(Ice::Identity{name, ""});
        }

        void forward(const Ice::ByteSeq&, const Ice::Current&) const;

    private:
        void connect(std::optional<DataStormContract::LookupPrx>, std::optional<DataStormContract::NodePrx>);

        void connected(std::optional<DataStormContract::NodePrx>, std::optional<DataStormContract::LookupPrx>);

        void disconnected(std::optional<DataStormContract::NodePrx>, std::optional<DataStormContract::LookupPrx>);

        void destroySession(std::optional<DataStormContract::NodePrx>);

        std::shared_ptr<Instance> getInstance() const
        {
            auto instance = _instance.lock();
            assert(instance);
            return instance;
        }

        const std::weak_ptr<Instance> _instance;
        const std::shared_ptr<TraceLevels> _traceLevels;
        const std::optional<DataStormContract::NodePrx> _nodePrx;
        const bool _forwardToMulticast;

        mutable std::mutex _mutex;

        int _retryCount;

        std::map<Ice::Identity, std::shared_ptr<NodeSessionI>> _sessions;
        std::map<
            Ice::Identity,
            std::pair<std::optional<DataStormContract::NodePrx>, std::optional<DataStormContract::LookupPrx>>>
            _connectedTo;

        mutable std::shared_ptr<Ice::Connection> _exclude;
        std::optional<DataStormContract::LookupPrx> _forwarder;
    };

}
#endif
