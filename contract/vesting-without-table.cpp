#include <graphenelib/system.h>
#include <graphenelib/asset.h>
#include <graphenelib/contract.hpp>
#include <graphenelib/contract_asset.hpp>
#include <graphenelib/dispatcher.hpp>
#include <graphenelib/global.h>
#include <graphenelib/multi_index.hpp>
#include <math.h>

using namespace graphene;
using std::string;

// only for GXC
const uint64_t contract_asset_id = 1; //GXC

// gxchain foundation account
const char * foundation_account = "gxbfoundation";

// time at which the first claim period begins
uint64_t start_time = 1577836800; // '2020-01-01T00:00:00'

// length of the claim period in seconds
const int64_t claim_period_sec = 365 * 24 * 3600;

// claim limit in a given claim period
const uint64_t claim_limit = (uint64_t)500 * 10000 * 100000;

// total claim amount
const uint64_t total_claim_amount = (uint64_t)4000 * 10000 * 100000;

// total claim count
const int64_t total_claim_count = 8;

class vesting : public contract
{
public:
    vesting(uint64_t n)
        : contract(n)
    {
    }

    /// @abi action
    void claim()
    {
      // check sender
      uint64_t sender = get_trx_sender();
      uint64_t origin_sender = get_trx_origin();
      uint64_t foundation_account_id = get_account_id(foundation_account, strlen(foundation_account));
      graphene_assert(sender == origin_sender, "Only normal account can claim vesting balances");
      graphene_assert(sender == foundation_account_id, "Only foundation account can claim vesting balances");

      // get contract balance
      uint64_t total_balance = get_balance(_self, contract_asset_id);
      graphene_assert(total_balance > 0, "Insufficient balance of contract");

      // check vesting claim count
      uint64_t now = get_head_block_time();
      graphene_assert(now >= start_time, "First claim time not arrived");

      int64_t current_claim_count = (total_claim_amount - total_balance) / claim_limit;
      if (current_claim_count < 0) {
          current_claim_count = 0;
      }

      print(current_claim_count, "\n");

      int64_t expect_claim_count = ((int64_t)now - start_time) / claim_period_sec;
      graphene_assert(current_claim_count <= expect_claim_count, "Next claim time has not arrived");

      print(expect_claim_count, "\n");
      // limit claim amount
      uint64_t claim_amount = std::min(total_balance, claim_limit);
      graphene_assert(claim_amount > 0, "Insufficient balance");

      // transfer GXC
      string memo = "Claim vesting GXC";
      inline_transfer(_self, sender, contract_asset_id, claim_amount, memo.c_str(), memo.size());
    }
};

GRAPHENE_ABI(vesting, (claim))
