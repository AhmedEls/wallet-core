// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "proto/Cosmos.pb.h"
#include "Cosmos/Signer.h"
#include "HexCoding.h"

#include <gtest/gtest.h>
#include <google/protobuf/util/json_util.h>

using namespace TW;

TEST(THORChainSigner, SignTx) {
    auto input = Cosmos::Proto::SigningInput();
    input.set_memo("memo1234");

    auto msg = input.add_messages();
    auto& message = *msg->mutable_send_coins_message();
    message.set_from_address("thor1z53wwe7md6cewz9sqwqzn0aavpaun0gw0exn2r");
    message.set_to_address("thor1e2ryt8asq4gu0h6z2sx9u7rfrykgxwkmr9upxn");
    auto amountOfTx = message.add_amounts();
    amountOfTx->set_denom("rune");
    amountOfTx->set_amount(50000000);

    auto& fee = *input.mutable_fee();
    fee.set_gas(2000000);
    auto amountOfFee = fee.add_amounts();
    amountOfFee->set_denom("rune");
    amountOfFee->set_amount(200);

    std::string json;
    google::protobuf::util::MessageToJsonString(input, &json);

    EXPECT_EQ(R"({"fee":{"amounts":[{"denom":"rune","amount":"200"}],"gas":"2000000"},"memo":"memo1234","messages":[{"sendCoinsMessage":{"fromAddress":"thor1z53wwe7md6cewz9sqwqzn0aavpaun0gw0exn2r","toAddress":"thor1e2ryt8asq4gu0h6z2sx9u7rfrykgxwkmr9upxn","amounts":[{"denom":"rune","amount":"50000000"}]}}]})", json);

    auto privateKey = parse_hex("7105512f0c020a1dd759e14b865ec0125f59ac31e34d7a2807a228ed50cb343e");
    input.set_private_key(privateKey.data(), privateKey.size());

    auto output = Cosmos::Signer::sign(input);

    EXPECT_EQ(R"({"mode":"block","tx":{"fee":{"amount":[{"amount":"200","denom":"rune"}],"gas":"2000000"},"memo":"memo1234","msg":[{"type":"cosmos-sdk/MsgSend","value":{"amount":[{"amount":"50000000","denom":"rune"}],"from_address":"thor1z53wwe7md6cewz9sqwqzn0aavpaun0gw0exn2r","to_address":"thor1e2ryt8asq4gu0h6z2sx9u7rfrykgxwkmr9upxn"}}],"signatures":[{"pub_key":{"type":"tendermint/PubKeySecp256k1","value":"A+2Zfjls9CkvX85aQrukFZnM1dluMTFUp8nqcEneMXx3"},"signature":"AWfOWDFZA3ElfcazPCL8S+ezM4xFfb3gIq+CGS267vYvA3VGxQcUQfhnY89OvkHVN7eZ+XwXZZ3ZUGgdK/n/ug=="}]}})", output.json());
    EXPECT_EQ(hex(output.signature()), "0167ce5831590371257dc6b33c22fc4be7b3338c457dbde022af82192dbaeef62f037546c5071441f86763cf4ebe41d537b799f97c17659dd950681d2bf9ffba");
}
