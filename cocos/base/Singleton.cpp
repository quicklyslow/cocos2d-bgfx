#include "ccHeader.h"
#include "Singleton.h"
#include <mutex>

NS_CC_BEGIN

struct LifeCycler
{
    void destroy(String itemName = silly::slice::Slice::Empty)
    {
        std::unordered_set<std::string> entries;
        if (itemName.empty())
        {
            for (const auto& life : lives_)
            {
                entries.insert(life.first);
            }

            for (const auto& itemRef : itemRefs_)
            {
                entries.insert(itemRef.first);
            }

            for (auto& ref : refs_)
            {
                ref.visited = false;
                auto it = entries.find(ref.target);
                if (it != entries.end())
                {
                    entries.erase(it);
                }
            }
        }
        else
        {
            for (auto& ref : refs_)
            {
                ref.visited = false;
            }
            entries.insert(itemName);
        }

        for (const auto& entry : entries)
        {
            std::queue<silly::slice::Slice> refList;
            std::vector<std::string> items;
            refList.push(entry);
            while (!refList.empty())
            {
                String item = refList.front();
                refList.pop();
                items.push_back(item);
                auto it = itemRefs_.find(item);
                if (it != itemRefs_.end())
                {
                    for (Reference* r : *it->second)
                    {
                        if (!r->visited)
                        {
                            r->visited = true;
                            refList.push(r->target);
                        }
                    }
                }
            }
#if COCOS2D_DEBUG
            std::unordered_set<std::string> names;
            for (auto it = items.rbegin(); it != items.rend(); ++it)
            {
                if (names.find(*it) == names.end() && lives_.find(*it) != lives_.end())
                {
                    names.insert(*it);
                    CCLOG("destory singleton \" %s \",", it->c_str());
                }
            }
#endif

            for (auto it = items.rbegin(); it != items.rend(); ++it)
            {
                lives_.erase(*it);
                itemRefs_.erase(*it);
            }
        }
    }

    ~LifeCycler()
    {
        destroy();
    }

    struct Reference
    {
        bool visited;
        std::string target;
    };

    std::list<Reference> refs_;
    std::unordered_set<std::string> names_;
    std::unordered_map<std::string, Own<Life>> lives_;
    std::unordered_map<std::string, Own<std::vector<Reference*>>> itemRefs_;
};

Own<LifeCycler> globalCycler;

std::once_flag initCyclerOnce;

LifeCycler* getCycler()
{
    std::call_once(initCyclerOnce, []()
    {
        globalCycler = New<LifeCycler>();
    });
    return globalCycler;
}

#if COCOS2D_DEBUG
void Life::assertIf(bool disposed, String name)
{
    //CCASSERT(!disposed, "accessing disposed singleton instance named \" %s \".", name);
}
#endif

void Life::addName(String name)
{
    LifeCycler* cycler = getCycler();
    cycler->names_.insert(name);
    if (cycler->lives_.find(name) == cycler->lives_.end())
    {
        cycler->lives_[name] = nullptr;
    }
}

void Life::addDependency(String target, String dependency)
{
    LifeCycler* cycler = getCycler();
    cycler->refs_.push_back(LifeCycler::Reference{ false, target });
    auto it = cycler->itemRefs_.find(dependency);
    if (it != cycler->itemRefs_.end())
    {
        it->second->push_back(&cycler->refs_.back());
    }
    else
    {
        auto refList = new std::vector<LifeCycler::Reference*>();
        refList->push_back(&cycler->refs_.back());
        cycler->itemRefs_[dependency] = MakeOwn(refList);
    }
}

void Life::addItem(String name, Life* life)
{
    LifeCycler* cycler = getCycler();
    cycler->lives_[name] = MakeOwn(life);
}

void Life::destroy(String name)
{
    LifeCycler* cycler = getCycler();
    cycler->destroy(name);
}
NS_CC_END
