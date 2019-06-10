## vesting 合约
vesting合约实现GXC资产的定期释放(vesting)，主要参数包括：

```
// GXC 资产id
const uint64_t contract_asset_id = 1; //GXC

// 只有permission_account，才可以调用合约创建vesting
const char * permission_account = "gxbfoundation";

// 创建 vesting时，可指定start_time，在此时间之后，可申领vesting
uint64_t start_time = 1560139200; // 2019-06-10T04:00:00

// vesting申领间隔，单位为秒
const uint64_t claim_period_sec = 365 * 24 * 3600;

// 每次可申领vesting的固定数量
const uint64_t claim_limit = (uint64_t)10 * 10000 * 100000;
```

## 接口
### vestingcreate 创建vesting
创建vesting， 参数指定claim_account， 即申领帐户的帐户名。

```
/// @abi action

/// @abi payable

void vestingcreate(std::string claim_account)
```


### vestingclaim 申领vesting
申领帐户从合约中，定期取回资产。

```
/// @abi action
    
void vestingclaim()
```


## table定义

claim_account | total_amount | last_claim_time | claim_limit
---|---|---|---
申领帐户id| vesting总量| 最近一次申领时间 | 每次的固定申领数量
71 | 4000000000000 | 1560155628 |  500000000000

