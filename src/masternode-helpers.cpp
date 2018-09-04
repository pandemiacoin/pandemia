// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2017-2018 The Pandemia developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "masternode-helpers.h"
#include "init.h"
#include "main.h"
#include "masternodeman.h"
#include "activemasternode.h"
#include "masternode-payments.h"
#include "amount.h"
#include "swifttx.h"

// A helper object for signing messages from Masternodes
CMasternodeSigner masternodeSigner;

void ThreadMasternodePool()
{
    if (fLiteMode) return; //disable all Masternode related functionality

    // Make this thread recognisable
    RenameThread("pandemia-mnpool");

    unsigned int c = 0;

    while (true) {
        MilliSleep(1000);

        // try to sync from all available nodes, one step at a time
        masternodeSync.Process();

        if (masternodeSync.IsBlockchainSynced()) {
            c++;

            // check if we should activate or ping every few minutes,
            // start right after sync is considered to be done
            if (c % MASTERNODE_PING_SECONDS == 0) activeMasternode.ManageStatus();

            if (c % 60 == 0) {
                mnodeman.CheckAndRemove();
                mnodeman.ProcessMasternodeConnections();
                masternodePayments.CleanPaymentList();
                CleanTransactionLocksList();
            }
        }
    }
}

bool CMasternodeSigner::IsVinAssociatedWithPubkey(CTxIn& vin, CPubKey& pubkey)
{
    CScript payee2;
    payee2 = GetScriptForDestination(pubkey.GetID());

    CAmount collateral = 8000 * COIN;

    CTransaction txVin;
    uint256 hash;
    if (GetTransaction(vin.prevout.hash, txVin, hash, true)) {
        BlockMap::iterator iter = mapBlockIndex.find(hash);
        if (iter != mapBlockIndex.end()) {
            int txnheight = iter->second->nHeight;
            
            if (txnheight <= GetSporkValue(SPORK_21_REWARDS_4_SWITCH)) {
                collateral = 2000 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_22_REWARDS_5_COLLATERAL_2500_SWITCH)) {
                collateral = 2500 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_23_REWARDS_10_COLLATERAL_3000_SWITCH)) {
                collateral = 3000 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_24_REWARDS_18_COLLATERAL_3500_SWITCH)) {
                collateral = 3500 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_25_REWARDS_28_COLLATERAL_4000_SWITCH)) {
                collateral = 4000 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_26_REWARDS_40_COLLATERAL_4500_SWITCH)) {
                collateral = 4500 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_27_REWARDS_54_COLLATERAL_5000_SWITCH)) {
                collateral = 5000 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_28_REWARDS_70_COLLATERAL_5500_SWITCH)) {
                collateral = 5500 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_29_REWARDS_88_COLLATERAL_6000_SWITCH)) {
                collateral = 6000 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_30_REWARDS_108_COLLATERAL_6500_SWITCH)) {
                collateral = 6500 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_31_REWARDS_130_COLLATERAL_7000_SWITCH)) {
                collateral = 7000 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_32_REWARDS_154_COLLATERAL_7500_SWITCH)) {
                collateral = 7500 * COIN;
            } else if (txnheight <= GetSporkValue(SPORK_33_REWARDS_180_COLLATERAL_8000_SWITCH)) {
                collateral = 8000 * COIN;
            }
        }

        BOOST_FOREACH (CTxOut out, txVin.vout) {
            if (out.nValue == collateral) {
                if (out.scriptPubKey == payee2) return true;
            }
        }
    }

    return false;
}

bool CMasternodeSigner::SetKey(std::string strSecret, std::string& errorMessage, CKey& key, CPubKey& pubkey)
{
    CBitcoinSecret vchSecret;
    bool fGood = vchSecret.SetString(strSecret);

    if (!fGood) {
        errorMessage = _("Invalid private key.");
        return false;
    }

    key = vchSecret.GetKey();
    pubkey = key.GetPubKey();

    return true;
}

bool CMasternodeSigner::GetKeysFromSecret(std::string strSecret, CKey& keyRet, CPubKey& pubkeyRet)
{
    CBitcoinSecret vchSecret;

    if (!vchSecret.SetString(strSecret)) return false;

    keyRet = vchSecret.GetKey();
    pubkeyRet = keyRet.GetPubKey();

    return true;
}

bool CMasternodeSigner::SignMessage(std::string strMessage, std::string& errorMessage, vector<unsigned char>& vchSig, CKey key)
{
    CHashWriter ss(SER_GETHASH, 0);
    ss << strMessageMagic;
    ss << strMessage;

    if (!key.SignCompact(ss.GetHash(), vchSig)) {
        errorMessage = _("Signing failed.");
        return false;
    }

    return true;
}

bool CMasternodeSigner::VerifyMessage(CPubKey pubkey, vector<unsigned char>& vchSig, std::string strMessage, std::string& errorMessage)
{
    CHashWriter ss(SER_GETHASH, 0);
    ss << strMessageMagic;
    ss << strMessage;

    CPubKey pubkey2;
    if (!pubkey2.RecoverCompact(ss.GetHash(), vchSig)) {
        errorMessage = _("Error recovering public key.");
        return false;
    }

    if (fDebug && pubkey2.GetID() != pubkey.GetID())
        LogPrintf("CMasternodeSigner::VerifyMessage -- keys don't match: %s %s\n", pubkey2.GetID().ToString(), pubkey.GetID().ToString());

    return (pubkey2.GetID() == pubkey.GetID());
}

bool CMasternodeSigner::SetCollateralAddress(std::string strAddress)
{
    CBitcoinAddress address;
    if (!address.SetString(strAddress)) {
        LogPrintf("CMasternodeSigner::SetCollateralAddress - Invalid collateral address\n");
        return false;
    }
    collateralPubKey = GetScriptForDestination(address.Get());
    return true;
}

bool CMasternodeSigner::IsCollateralAmount(const CAmount& amount)
{
    return
            amount == 2000  * COIN ||
            amount == 2500  * COIN ||
            amount == 3000  * COIN ||
            amount == 3500  * COIN ||
            amount == 4000  * COIN ||
            amount == 4500  * COIN ||
            amount == 5000  * COIN ||
            amount == 5500  * COIN ||
            amount == 6000  * COIN ||
            amount == 6500  * COIN ||
            amount == 7000  * COIN ||
            amount == 7500  * COIN ||
            amount == 8000 * COIN;
}
