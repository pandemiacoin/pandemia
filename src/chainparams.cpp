// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2017-2018 The Pandemia developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "bignum.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
    (0, uint256("0x00000b1d97421df2ceef97b66499ad171c2455131bc6f8edd98e5297008a66c8"))
    (400, uint256("0x0000008eff714ecabde12aff882bd0b72f2bc6a065e53139068346b25cc3dae7"))
    (1000, uint256("0x0110a8900445c12015c4ca936bffcbd06ecf896dc8db9b7faf7cedb3e8bcfb63"));

static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1536039747, // * UNIX timestamp of last checkpoint block
    1607,          // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    2000        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of
    (0, uint256("0x0000010ec54414321c3a125b9764e2b5c7bdbafe783eb93337c5c2076682202c"));

static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1535673610,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of
    (0, uint256("0x594b0c3a5b8add44009ec619d89a0ccef5f992356187c4cb5fce9c20e3ade38c"));

static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1535673620,
    0,
    100};

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x12;
        pchMessageStart[1] = 0x54;
        pchMessageStart[2] = 0xa8;
        pchMessageStart[3] = 0x33;
        vAlertPubKey = ParseHex("047a77e93b981cb8dcd8245a219dfeaa2f754172d88e8d602701f615436c977e6f56fa0e70eebad3229f7a5a71c2b19fd14dbe6914d5a7aacddd13b80ccf7d11c6");
        nDefaultPort = 47666;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        nSubsidyHalvingInterval = 1050000;
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 1 * 60; // Pandemia: 1 day
        nTargetSpacing = 2 * 60;  // Pandemia: 2 minutes
        nMaturity = 10;
        nMasternodeCountDrift = 20;
        nMaxMoneyOut = 100000000 * COIN;

        /** Height or Time Based Activations **/
        nLastPOWBlock = 400;
        nModifierUpdateBlock = 1; // we use the version 2 for PNDM

        const char* pszTimestamp = "Friday, 31-Aug-18 00:00:00 UTC";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 0 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("0408a94dfaa515db109252c7a95fd30cb92093d8af0776b53f7358efc19afabf287e670d8fd0e00037aabcf2520b782e2c8e3818023c0a72c774a3db8412f4bc05") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1535673600;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 844747;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000b1d97421df2ceef97b66499ad171c2455131bc6f8edd98e5297008a66c8"));
        assert(genesis.hashMerkleRoot == uint256("0x4c5cf0687e116359e043145d6ecc89d57ea2ca20b33e6df77b1438d0681d39a3"));

        // DNS Seeding
        vSeeds.push_back(CDNSSeedData("dnsseed.pandemia.io", "dnsseed.pandemia.io"));

        // Pandemia addresses start with 'P'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 55);
        // Pandemia script addresses start with '3'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 6);
        // Pandemia private keys start with 'K'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 46);
        // Pandemia BIP32 pubkeys start with 'xpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        // Pandemia BIP32 prvkeys start with 'xprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        // Pandemia BIP44 coin type is '47662' (0x8000ba2e)
        // BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0xba)(0x2e).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        strSporkKey = "0464a799b44dcb92ff0fcade74cb0ff7341a629fd5678ce7c255cb92182f96686bc3c2f5dccc86f28b4cf602f9f19675c464de002d86403081481b44c34f517343";
        strMasternodePoolDummyAddress = "PSJVWUkt6HtSCY2SaJ2akeyJUg8bg1hW3S";
        nStartMasternodePayments = genesis.nTime + 86400; // 24 hours after genesis creation

        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0xbc;
        pchMessageStart[1] = 0x8a;
        pchMessageStart[2] = 0xae;
        pchMessageStart[3] = 0x54;
        vAlertPubKey = ParseHex("0457f2420f1ae36f50c00b59be60df82eee5f0359e9ce34acc0bdf7d934ed6eb22795e0bfa93f2889b24f4813eb8f35eedc5d0282578329532aa12a91d00077d87");
        nDefaultPort = 47663;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 1 * 60; // Pandemia: 1 day
        nTargetSpacing = 2 * 60;  // Pandemia: 1 minute
        nLastPOWBlock = 200;
        nMaturity = 15;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = 1;
        nMaxMoneyOut = 100000000 * COIN;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1535673610;
        genesis.nNonce = 550133;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x0000010ec54414321c3a125b9764e2b5c7bdbafe783eb93337c5c2076682202c"));

        vFixedSeeds.clear();
        vSeeds.clear();

        // Testnet Pandemia addresses start with 'p'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 117);
        // Testnet Pandemia script addresses start with '5' or '6'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 12);
        // Testnet private keys start with 'k'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 108);
        // Testnet Pandemia BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet Pandemia BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        // Testnet pandemia BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        strSporkKey = "0499006d850d5463559651dbeea68e9b464c50cc95607c952088b339998ed1fbaf8354e04a8e6559a413e7f60f9d9ddaf6319821c7860ef823bd16037a44faaa9d";
        strMasternodePoolDummyAddress = "pbJ4Qad4xc77PpLzMx6rUegAs6aUPWkcUq";
        nStartMasternodePayments = genesis.nTime + 86400; // 24 hours after genesis
        nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
                                       // here because we only have a 8 block finalization window on testnet
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        strNetworkID = "regtest";
        pchMessageStart[0] = 0x12;
        pchMessageStart[1] = 0x33;
        pchMessageStart[2] = 0xbc;
        pchMessageStart[3] = 0xa8;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 24 * 60 * 60; // Pandemia: 1 day
        nTargetSpacing = 2 * 60;        // Pandemia: 1 minutes
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1535673620;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 1;

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 47664;
        assert(hashGenesisBlock == uint256("0x594b0c3a5b8add44009ec619d89a0ccef5f992356187c4cb5fce9c20e3ade38c"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 47661;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
