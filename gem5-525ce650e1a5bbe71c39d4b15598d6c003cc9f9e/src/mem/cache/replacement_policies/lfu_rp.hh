/**
 * @file
 * Declaration of a Least Frequently Used replacement policy with Oldest Victim.
 * The victim is chosen using the reference frequency. Among entries with the
 * least reference count, the one with the oldest access time is chosen to be evicted.
 */

#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_LFU_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_LFU_RP_HH__

#include "mem/cache/replacement_policies/base.hh"

struct LFURPParams;

class LFURP : public BaseReplacementPolicy
{
  protected:
    /** LFU-specific implementation of replacement data. */
    struct LFUReplData : ReplacementData
    {
        /** Number of references to this entry since it was reset. */
        unsigned refCount;
        /** Timestamp of the last access to this entry. */
        uint64_t timestamp;

        /**
         * Default constructor. Invalidate data.
         */
        LFUReplData() : refCount(0), timestamp(0) {}
    };

    /** Global counter to simulate time for timestamp assignment. */
    static uint64_t globalTimestamp;

  public:
    /** Convenience typedef. */
    typedef LFURPParams Params;

    /**
     * Construct and initialize this replacement policy.
     */
    LFURP(const Params *p);

    /**
     * Destructor.
     */
    ~LFURP() {}

    /**
     * Invalidate replacement data to set it as the next probable victim.
     * Clear the number of references and timestamp.
     *
     * @param replacement_data Replacement data to be invalidated.
     */
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
                                                              const override;

    /**
     * Touch an entry to update its replacement data.
     * Increase number of references and update timestamp.
     *
     * @param replacement_data Replacement data to be touched.
     */
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Reset replacement data. Used when an entry is inserted.
     * Reset number of references and set timestamp.
     *
     * @param replacement_data Replacement data to be reset.
     */
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Find replacement victim using reference frequency and oldest timestamp.
     *
     * @param cands Replacement candidates, selected by indexing policy.
     * @return Replacement entry to be replaced.
     */
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const
                                                                     override;

    /**
     * Instantiate a replacement data entry.
     *
     * @return A shared pointer to the new replacement data.
     */
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_LFU_RP_HH__
