#include "mem/cache/replacement_policies/lfu_rp.hh"

#include <cassert>
#include <memory>

#include "params/LFURP.hh"

// Initialize global timestamp counter
uint64_t LFURP::globalTimestamp = 0;

LFURP::LFURP(const Params *p)
    : BaseReplacementPolicy(p)
{
}

void
LFURP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
const
{
    // Reset reference count and timestamp
    auto data = std::static_pointer_cast<LFUReplData>(replacement_data);
    data->refCount = 0;
    data->timestamp = 0;
}

void
LFURP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Update reference count and timestamp
    auto data = std::static_pointer_cast<LFUReplData>(replacement_data);
    data->refCount++;
    data->timestamp = ++globalTimestamp;
}

void
LFURP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Reset reference count and set timestamp
    auto data = std::static_pointer_cast<LFUReplData>(replacement_data);
    data->refCount = 1;
    data->timestamp = ++globalTimestamp;
}

ReplaceableEntry*
LFURP::getVictim(const ReplacementCandidates& candidates) const
{
    // There must be at least one replacement candidate
    assert(candidates.size() > 0);

    // Initialize with the first candidate
    ReplaceableEntry* victim = candidates[0];
    auto victim_data = std::static_pointer_cast<LFUReplData>(victim->replacementData);
    unsigned min_ref_count = victim_data->refCount;
    uint64_t oldest_timestamp = victim_data->timestamp;

    // Find the entry with minimum reference count and oldest timestamp
    for (const auto& candidate : candidates) {
        auto data = std::static_pointer_cast<LFUReplData>(candidate->replacementData);
        if (data->refCount < min_ref_count) {
            // Lower reference count found, update victim
            victim = candidate;
            min_ref_count = data->refCount;
            oldest_timestamp = data->timestamp;
        } else if (data->refCount == min_ref_count && data->timestamp < oldest_timestamp) {
            // Same reference count, but older timestamp, update victim
            victim = candidate;
            oldest_timestamp = data->timestamp;
        }
    }

    return victim;
}

std::shared_ptr<ReplacementData>
LFURP::instantiateEntry()
{
    return std::shared_ptr<ReplacementData>(new LFUReplData());
}

LFURP*
LFURPParams::create()
{
    return new LFURP(this);
}
