#include <set>
#include <list>
#include <cassert>
#include "GCNode.hpp"

namespace sstd {

    template<typename T, typename F>
    inline void destoryAll(T & arg, F && fun) {
        auto varPos = arg.rbegin();
        auto varEnd = arg.rend();
        for (; varPos != varEnd; ++varPos) {
            fun(*varPos);
        }
    }

    GCNode::GCNode() :
        mmmGCState{ GCNodeState::White },
        mmmManager{ nullptr }{
    }

    GCNode::~GCNode() {
    }

    std::size_t GCNode::doTheClassSize() const noexcept {
        return sizeof(*this);
    }

    GCNodeChildIterator * GCNode::doTheClassChildren() const noexcept {
        static GCNodeChildIterator varAns;
        return &varAns;
    }

    GCNodeChildIterator::GCNodeChildIterator() {
    }

    GCNodeChildIterator::~GCNodeChildIterator() {
    }

    GCNode * GCNodeChildIterator::next() noexcept {
        return nullptr;
    }

    class GCNodeManagerPrivate {
    public:
        std::set<GCNode *> roots;
        std::list<GCNode *> grayNodes;
        std::list<GCNode *> allNodes;
        std::size_t currentMemorySize{ 0 };
        std::size_t oldMemorySize{ 0 };
        inline GCNodeManagerPrivate();
        inline ~GCNodeManagerPrivate();
    };

    inline GCNodeManagerPrivate::GCNodeManagerPrivate() {
    }

    inline GCNodeManagerPrivate::~GCNodeManagerPrivate() {
        destoryAll(allNodes, [](GCNode * v) { delete v; });
    }

    GCNodeManager::GCNodeManager() {
        using TheDataType = decltype(mmmTheData);
        static_assert((sizeof(GCNodeManagerPrivate) + alignof(GCNodeManagerPrivate))
            <= sizeof(TheDataType));
        {
            void * varPointer = mmmTheData.data();
            auto varSize = mmmTheData.size();
            varPointer = std::align(
                alignof(GCNodeManagerPrivate),
                sizeof(GCNodeManagerPrivate),
                varPointer,
                varSize);
            thePrivate = ::new(varPointer) GCNodeManagerPrivate;
        }
    }

    GCNodeManager::~GCNodeManager() {
        std::destroy_at(thePrivate);
    }

    void GCNodeManager::markAsGray(GCNode * arg) noexcept {
        thePrivate->grayNodes.push_back(arg);
        arg->mmmGCState = GCNodeState::Gray;
    }

    void GCNode::theClassAddRef(GCNode *arg) noexcept {
        assert(theClassManager());

        if (arg->mmmManager == nullptr) {
            theClassManager()->addANode(arg);
        }

        if (mmmGCState == GCNodeState::Black) {
            if (arg->mmmGCState == GCNodeState::White) {
                theClassManager()->markAsGray(arg);
            }
        }
        doTheClassAddRef(arg);
    }

    void GCNode::doTheClassAddRef(GCNode *) noexcept {
        /*do it yourself...*/
    }

    void GCNodeManager::nextStep() noexcept {

        const auto & varRoots = thePrivate->roots;
        auto & varGray = thePrivate->grayNodes;
        auto & varAll = thePrivate->allNodes;

        for (const auto & varI : varRoots) {
            if (varI->mmmGCState == GCNodeState::White) {
                markAsGray(varI);
            }
        }

        while (!varGray.empty()) {

            auto varValue = varGray.front();
            varGray.pop_front();

            varValue->mmmGCState = GCNodeState::Black;

            {
                auto varI = varValue->theClassChildren();
                while (auto varNext = varI->next()) {
                    if (varNext->mmmGCState == GCNodeState::White) {
                        markAsGray(varNext);
                    }
                }
            }

        }

        std::list< GCNode * > varDoNotDelete;
        std::list< GCNode * > varDelete;

        while (!varAll.empty()) {

            auto varPos = varAll.begin();
            if ((*varPos)->mmmGCState != GCNodeState::White) {
                varDoNotDelete.splice(varDoNotDelete.end(),
                    varAll,
                    varPos);
                (*varPos)->mmmGCState = GCNodeState::White;
            } else {
                varDelete.splice(varDelete.end(),
                    varAll,
                    varPos);
            }

        }

        varAll = std::move(varDoNotDelete);

        {
            auto varRemove = [this](GCNode * v) {
                removeANode(v);
            };
            destoryAll(varDelete, varRemove);
        }

        thePrivate->oldMemorySize = thePrivate->currentMemorySize;

    }

    void GCNodeManager::addANode(GCNode * arg) noexcept {
        arg->setGCNodeManager(this);
        thePrivate->allNodes.push_back(arg);
        thePrivate->currentMemorySize += arg->theClassSize();
        if (thePrivate->currentMemorySize > thePrivate->oldMemorySize) {
            if (newMeomorySize() > (1024uLL * 1024uLL * 64uLL)) {
                this->nextStep();
            }
        }
    }

    void GCNodeManager::removeANode(GCNode * arg) noexcept {
        thePrivate->currentMemorySize -= arg->theClassSize();
        delete arg;
    }

    void GCNodeManager::markAsRoot(GCNode * arg) noexcept {
        if (arg->mmmManager == nullptr) {
            addANode(arg);
        }
        thePrivate->roots.insert(arg);
        if (arg->mmmGCState == GCNodeState::White) {
            markAsGray(arg);
        }
    }

    void GCNodeManager::removeRoot(GCNode * arg) noexcept {
        thePrivate->roots.erase(arg);
    }

    std::size_t GCNodeManager::newMeomorySize() const noexcept {
        const auto varOld = thePrivate->oldMemorySize;
        const auto varNew = thePrivate->currentMemorySize;
        if (varNew > varOld) {
            return varNew - varOld;
        }
        return varOld - varNew;
    }

}/*namespace sstd*/




