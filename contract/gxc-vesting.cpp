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
const char* start_time = "2018-12-19T14:27:40";

// length of the claim period in seconds
const uint64_t claim_period_sec = 365 * 24 * 3600;

// claim limit in a given claim period
const uint64_t claim_limit = 500 * 10000;

class vesting : public contract
{
public:
    vesting(uint64_t n)
        : contract(n)
          , vestingtab(_self, _self)
    {
    }

    /// @abi action
    void vestingcreate(std::string claim_account)
    {
        // only permission_account can create vesting
        uint64_t sender = get_trx_sender();
        uint64_t permission_account_id = get_account_id(permission_account, strlen(permission_account));
        graphene_assert(sender == permission_account_id, "no claim permission");

        // only support GXC
        graphene_assert(contract_asset_id == get_action_asset_id(), "only suppor GXC");
        int64_t amnt = get_action_asset_amount();
	graphene_assert(amnt > 0, "amount must > 0");

        // get claim_account
        int64_t claim_account_id = get_account_id(claim_account.c_str(), claim_account.size());
        graphene_assert(claim_account_id >= 0, "invalid claim_account");

        // update vesting table
        auto iter = vestingtab.find(claim_account_id);
        if (iter == vestingtab.end()) {
            vestingtab.emplace(0, [&](auto &o) {
                    o.total_amount = amnt;
                    o.claim_account = claim_account_id;
                    o.claim_limit = claim_limit;
                    o.last_claim_time = 0;
                    });
        }
        else {
            vestingtab.modify(iter, 0, [&](auto &o) {
                    o.total_amount += amnt;
                    });
        }
    }

    /// @abi action
    void vestingclaim()
    {
        uint64_t sender = get_trx_sender();
        uint64_t now = get_head_block_time();

        // check vesting exists
        auto iter = vestingtab.find(sender);
        graphene_assert(iter != vestingtab.end(), "current account have no vesting");

        // check seconds
        graphene_assert(now - iter->last_claim_time >= claim_period_sec, "vesting time not yet come");

        uint64_t amnt = std::max(iter->total_amount, iter->claim_limit);
        // update vesting
        vestingtab.modify(iter, 0, [&](auto &o) {
                o.total_amount -= amnt;
                o.last_claim_time = now;
                });

        // transfer GXC
        string memo = "claim vesting GXC";
        inline_transfer(_self, sender, contract_asset_id, claim_limit, memo.c_str(), memo.size());
    }

private:
    // @abi table vesting
    struct vestinginfo {
	    uint64_t claim_account;  // account who can claim vesting
	    uint64_t total_amount;
	    uint64_t last_claim_time; // last claim time
	    uint64_t claim_limit;  // claim limit once

	    uint64_t primary_key() const { return claim_account; }

	    GRAPHENE_SERIALIZE(vestinginfo, (claim_account)(total_amount)(last_claim_time)(claim_limit))
    };

    typedef graphene::multi_index<N(vestinginfo), vestinginfo> vesting_index;

private:
    vesting_index vestingtab;
};

GRAPHENE_ABI(vesting, (vestingclaim))
