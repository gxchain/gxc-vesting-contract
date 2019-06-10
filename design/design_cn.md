## vesting 合约
vesting合约实现GXC资产的定期释放(vesting)，主要参数包括：

```
// GXC 资产id
const uint64_t contract_asset_id = 1; //GXC

// 只有permission_account，才可以调用合约创建vesting
const char * permission_account = "gxbfoundation";

// 创建 vesting时，可指定最早申领start_time，在此时间之后，可申领vesting
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
{
// 获取sender
// 检查sender， 必须为permission_account， 本例中为gxbfoundation
// 检查向合约发送的资产，必须为GXC，数量必须大于0
// 参数中的claim_account帐户，必须存在
// 从vestinginfo表中查询claim_account的记录，如果不存在，则创建vesting； 如果存在，则更新记录中的vesting总量
}
```


### vestingclaim 申领vesting
申领帐户从合约中，定期取回资产。

```
/// @abi action
    
void vestingclaim()
{
// 获取sender
// 从vestinginfo表中查询sender对应的记录
// 如果查不到对应记录，则中止
// 检查当前时间，必须最早申领时间start_time
// 检查当前时间，必须大于 最近一次申领时间(last_claim_time) + 申领间隔(claim_period_sec)
// 更新vestinginfo表，更新总量(total_amount)和最近一次的申领时间(last_claim_time)
// 转帐给sender, 转帐数量为total_amount和claim_limit最小值
}
```


## table定义

vestinginfo表结构：

claim_account | total_amount | last_claim_time | claim_limit
---|---|---|---
申领帐户id| vesting总量| 最近一次申领时间 | 每次的固定申领数量
71 | 4000000000000 | 1560155628 |  500000000000

