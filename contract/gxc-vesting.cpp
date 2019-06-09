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

// the account authorized to make claims from contract
const char * permission_account = "gxbfoundation";

// time at which the first claim period begins
const char* start_time = "2018-12-19T14:27:40";

// claim limit
const uint64_t claim_limit = 500 * 10000;

class vesting : public contract
{
  public:
    vesting(uint64_t n)
        : contract(n)
        , historytab(_self, _self)
    {
    }

    /// @abi action
    void vestingclaim()
    {
        // check permission of permission_account
        // add history table
        // transfer GXC
    }

  private:
    //@abi table history i64
    struct history {
        uint64_t id;
        string claim_account;      //已释放资产

        int64_t claim_time;    //锁定开始时间
        int64_t claim_amount;      //锁定多久开始释放

        uint64_t primary_key() const { return id; }

        GRAPHENE_SERIALIZE(history,
                           (id)(claim_account)(claim_time)(claim_amount))
    };

    typedef graphene::multi_index<N(history), history> history_index;

  private:
    history_index historytab;
};

GRAPHENE_ABI(vesting, (vestingclaim))
