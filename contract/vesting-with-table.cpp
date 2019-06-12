#include <graphenelib/asset.h>
#include <graphenelib/contract.hpp>
#include <graphenelib/contract_asset.hpp>
#include <graphenelib/dispatcher.hpp>
#include <graphenelib/global.h>
#include <graphenelib/multi_index.hpp>
#include <vector>

using namespace graphene;
using std::string;

// only for GXC
const uint64_t contract_asset_id = 1; //GXC

// the account authorized to create vesting
const char * permission_account = "gxbfoundation";

// time at which the first claim period begins
uint64_t start_time = 1577836800; // '2020-01-01T00:00:00'

// length of the claim period in seconds
const uint64_t claim_period_sec = 365 * 24 * 3600;

// claim limit in a given claim period
const uint64_t claim_limit = (uint64_t)500 * 10000 * 100000;

class vesting : public contract
{
public:
    vesting(uint64_t n)
        : contract(n)
          , vestingtab(_self, _self)
    {
    }

    /// @abi action
    /// @abi payable
    void vestingcreate(std::string claim_account)
    {
        // only permission_account can create vesting
        uint64_t sender = get_trx_sender();
        uint64_t permission_account_id = get_account_id(permission_account, strlen(permission_account));
        graphene_assert(sender == permission_account_id, "no vesting create permission");

        // only support GXC
        graphene_assert(contract_asset_id == get_action_asset_id(), "only suppor GXC");

        // asset amount must > 0
        int64_t amnt = get_action_asset_amount();
        graphene_assert(amnt > 0, "amount must > 0");

        // check claim_account
        int64_t claim_account_id = get_account_id(claim_account.c_str(), claim_account.size());
        graphene_assert(claim_account_id >= 0, "invalid claim_account");

        // update vesting table
        auto iter = vestingtab.find(claim_account_id);
        if (iter == vestingtab.end()) {
            vestingtab.emplace(sender, [&](auto &o) {
                    o.total_amount = amnt;
                    o.claim_account = claim_account_id;
                    o.claim_limit = claim_limit;
                    o.last_claim_time = 0;
                    });
        } else {
            vestingtab.modify(iter, sender, [&](auto &o) {
                    o.total_amount += amnt;
                    });
        }
    }

    /// @abi action
    void vestingclaim()
    {
        uint64_t sender = get_trx_sender();

        // check vesting exists
        auto iter = vestingtab.find(sender);
        graphene_assert(iter != vestingtab.end(), "current account have no vesting");

        // check vesting claim seconds
        uint64_t now = get_head_block_time();
        graphene_assert(now >= start_time, "vesting time not yet come");
        graphene_assert(now - iter->last_claim_time >= claim_period_sec, "vesting time not yet come");

        uint64_t claim_amount = std::min(iter->total_amount, iter->claim_limit);
        graphene_assert(claim_amount > 0, "vesting balance not enough");

        // update vesting
        vestingtab.modify(iter, sender, [&](auto &o) {
                o.total_amount -= claim_amount;
                o.last_claim_time = now;
                });

        // transfer GXC
        string memo = "claim vesting GXC";
        inline_transfer(_self, sender, contract_asset_id, claim_amount, memo.c_str(), memo.size());
    }

private:
    // @abi table vestinginfo i64
    struct vestinginfo {
	    uint64_t claim_account;  // account who can claim vesting
	    uint64_t total_amount; // total vesting amount
	    uint64_t last_claim_time; // last claim time
	    uint64_t claim_limit;  // claim limit once

	    uint64_t primary_key() const { return claim_account; }

	    GRAPHENE_SERIALIZE(vestinginfo, (claim_account)(total_amount)(last_claim_time)(claim_limit))
    };

    typedef graphene::multi_index<N(vestinginfo), vestinginfo> vesting_index;

private:
    vesting_index vestingtab;
};

GRAPHENE_ABI(vesting, (vestingcreate)(vestingclaim))
