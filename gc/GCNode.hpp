#pragma once

#include <array>
#include <memory>
#include <utility>
#include <type_traits>

namespace sstd {

    class GCNode;
    class GCNodeManager;
    class GCNodeChildIterator;
    class GCNodeManagerPrivate;

    enum class GCNodeState : std::size_t {
        Black,
        White,
        Gray,
    };

    class GCNode {
        GCNodeState mmmGCState;
        GCNodeManager * mmmManager;
        friend class ::sstd::GCNodeManager;
    protected:
        GCNode();
    public:
        virtual ~GCNode();
    public:
        inline std::size_t theClassSize() const noexcept;
        inline GCNodeChildIterator * theClassChildren() const noexcept;
        inline GCNodeManager * theClassManager() const noexcept;
        void theClassAddRef(GCNode *) noexcept;
    protected:
        virtual std::size_t doTheClassSize() const noexcept;
        virtual GCNodeChildIterator * doTheClassChildren() const noexcept;
        virtual void doTheClassAddRef(GCNode *) noexcept;
    public:
        GCNode(const GCNode &) = delete;
        GCNode(GCNode&&) = delete;
        GCNode&operator=(const GCNode &) = delete;
        GCNode&operator=(GCNode&&) = delete;
    private:
        inline void setGCNodeManager(GCNodeManager *) noexcept;
    };

    class GCNodeChildIterator {
    public:
        GCNodeChildIterator();
        virtual ~GCNodeChildIterator();
        virtual GCNode * next() noexcept;
    };

    class GCNodeManager {
        std::array< std::uint8_t, 256 > mmmTheData;
        GCNodeManagerPrivate * thePrivate;
        friend class ::sstd::GCNode ;
    public:
        GCNodeManager();
        virtual ~GCNodeManager();
    public:
        template<typename T, typename ... Args>
        inline T * createNode(Args &&...);
    public:
        void nextStep() noexcept;
        void markAsRoot(GCNode *) noexcept;
        void removeRoot(GCNode *) noexcept;
        std::size_t newMeomorySize() const noexcept;
    private:
        void addANode(GCNode *) noexcept;
        void removeANode(GCNode *) noexcept;
        void markAsGray(GCNode *) noexcept;
    public:
        GCNodeManager(const GCNodeManager &) = delete;
        GCNodeManager(GCNodeManager &&) = delete;
        GCNodeManager&operator=(const GCNodeManager &) = delete;
        GCNodeManager&operator=(GCNodeManager &&) = delete;
    };


}/*NAMESPACE SSTD*/

namespace sstd {

    inline std::size_t GCNode::theClassSize() const noexcept {
        return this->doTheClassSize();
    }

    inline GCNodeChildIterator * GCNode::theClassChildren() const noexcept {
        return doTheClassChildren();
    }

    inline void GCNode::setGCNodeManager(GCNodeManager * arg) noexcept {
        this->mmmManager = arg;
    }

    inline GCNodeManager * GCNode::theClassManager() const noexcept {
        return this->mmmManager;
    }

    template<typename T, typename ... Args>
    inline T * GCNodeManager::createNode(Args &&... args) {

        static_assert( false==std::is_reference_v<T> );

        T * varAns;

        if constexpr (std::is_constructible_v<T, Args &&...> && (sizeof...(Args) > 0)) {
            varAns = new T(std::forward<Args>(args)...);
        } else {
            varAns = new T{ std::forward<Args>(args)... };
        }

        {
            GCNode * varNode = varAns;
            this->addANode(varNode);
        }

        return varAns;

    }

}/*NAMESPACE SSTD*/


