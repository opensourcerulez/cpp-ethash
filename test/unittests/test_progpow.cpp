// ethash: C/C++ implementation of Ethash, the Ethereum Proof of Work algorithm.
// Copyright 2018-2019 Pawel Bylica.
// Licensed under the Apache License, Version 2.0.

#include <ethash/endianness.hpp>
#include <ethash/progpow.hpp>

#include "helpers.hpp"
#include "progpow_test_vectors.hpp"

#include <gtest/gtest.h>
#include <array>

//TEST(progpow, revision)
//{
//    static_assert(progpow::revision[0] == '0', "");
//    static_assert(progpow::revision[1] == '.', "");
//    static_assert(progpow::revision[2] == '9', "");
//    static_assert(progpow::revision[3] == '.', "");
//    static_assert(progpow::revision[4] == '4', "");
//    EXPECT_EQ(progpow::revision, "0.9.4");
//    EXPECT_EQ(progpow::revision, (std::string{"0.9.4"}));
//}

TEST(progpow, l1_cache)
{
    auto& context = get_ethash_epoch_context_0();

    constexpr auto test_size = 20;
    std::array<uint32_t, test_size> cache_slice;
    for (size_t i = 0; i < cache_slice.size(); ++i)
        cache_slice[i] = ethash::le::uint32(context.l1_cache[i]);

    const std::array<uint32_t, test_size> expected{
            {2492749011, 430724829, 2029256771, 3095580433, 3583790154, 3025086503,
                    805985885, 4121693337, 2320382801, 3763444918, 1006127899, 1480743010,
                    2592936015, 2598973744, 3038068233, 2754267228, 2867798800, 2342573634,
                    467767296, 246004123}};
    EXPECT_EQ(cache_slice, expected);
}

TEST(progpow, hash_empty)
{
    auto& context = get_ethash_epoch_context_0();

    const auto result = progpow::hash(context, 0, {}, 0);
    const auto mix_hex = "b1388e12e9898029a487f5534225c2ea8bd79c6ef6754db0405809f53d83c497";
    const auto final_hex = "3d2f6484ee555362e9d8e2ca54fb99741e755e849f9900ef84ad65aa3c935bd1";
    EXPECT_EQ(to_hex(result.mix_hash), mix_hex);
    EXPECT_EQ(to_hex(result.final_hash), final_hex);
}

TEST(progpow, hash_30000)
{
    const int block_number = 30000;
    const auto header =
        to_hash256("ffeeddccbbaa9988776655443322110000112233445566778899aabbccddeeff");
    const uint64_t nonce = 0x123456789abcdef0;

    auto context = ethash::create_epoch_context(ethash::get_epoch_number(block_number));

    const auto result = progpow::hash(*context, block_number, header, nonce);
    const auto mix_hex = "aa15c8b7b34bfc7da13e6d734f63dd0517da65072f4b833c52d586cc3089a990";
    const auto final_hex = "d8f9786659a08e0ae792bb39725d3dc1e2cc0fb36c228a0ec3101bf17a7cb44b";
    EXPECT_EQ(to_hex(result.mix_hash), mix_hex);
    EXPECT_EQ(to_hex(result.final_hash), final_hex);
}

TEST(progpow, hash_and_verify)
{
    ethash::epoch_context_ptr context{nullptr, nullptr};

    for (auto& t : progpow_hash_test_cases)
    {
        const auto epoch_number = ethash::get_epoch_number(t.block_number);
        if (!context || context->epoch_number != epoch_number)
            context = ethash::create_epoch_context(epoch_number);

        const auto header_hash = to_hash256(t.header_hash_hex);
        const auto nonce = std::stoull(t.nonce_hex, nullptr, 16);
        const auto result = progpow::hash(*context, t.block_number, header_hash, nonce);
        EXPECT_EQ(to_hex(result.mix_hash), t.mix_hash_hex);
        EXPECT_EQ(to_hex(result.final_hash), t.final_hash_hex);

        auto success = progpow::verify(
            *context, t.block_number, header_hash, result.mix_hash, nonce, result.final_hash);
        EXPECT_TRUE(success);

        auto lower_boundary = result.final_hash;
        --lower_boundary.bytes[31];
        auto final_failure = progpow::verify(
            *context, t.block_number, header_hash, result.mix_hash, nonce, lower_boundary);
        EXPECT_FALSE(final_failure);

        auto different_mix = result.mix_hash;
        ++different_mix.bytes[7];
        auto mix_failure = progpow::verify(
            *context, t.block_number, header_hash, different_mix, nonce, result.final_hash);
        EXPECT_FALSE(mix_failure);
    }
}

TEST(progpow, search)
{
    auto ctxp = ethash::create_epoch_context_full(0);
    auto& ctx = *ctxp;
    auto& ctxl = reinterpret_cast<const ethash::epoch_context&>(ctx);

    auto boundary = to_hash256("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    auto sr = progpow::search(ctx, 0, {}, boundary, 300, 100);
    auto srl = progpow::search_light(ctxl, 0, {}, boundary, 300, 100);

    EXPECT_EQ(sr.mix_hash, ethash::hash256{});
    EXPECT_EQ(sr.final_hash, ethash::hash256{});
    EXPECT_EQ(sr.nonce, 0x0);
    EXPECT_EQ(sr.mix_hash, srl.mix_hash);
    EXPECT_EQ(sr.final_hash, srl.final_hash);
    EXPECT_EQ(sr.nonce, srl.nonce);

    sr = progpow::search(ctx, 0, {}, boundary, 700, 100);
    srl = progpow::search_light(ctxl, 0, {}, boundary, 700, 100);

    EXPECT_NE(sr.mix_hash, ethash::hash256{});
    EXPECT_NE(sr.final_hash, ethash::hash256{});
    EXPECT_EQ(sr.nonce, 740);
    EXPECT_EQ(sr.mix_hash, srl.mix_hash);
    EXPECT_EQ(sr.final_hash, srl.final_hash);
    EXPECT_EQ(sr.nonce, srl.nonce);

    auto r = progpow::hash(ctx, 0, {}, 740);
    EXPECT_EQ(sr.final_hash, r.final_hash);
    EXPECT_EQ(sr.mix_hash, r.mix_hash);
}

#if ETHASH_TEST_GENERATION
TEST(progpow, generate_hash_test_cases)
{
    constexpr auto num_epochs = 2;

    using namespace progpow;
    hash256 h{};
    for (int e = 0; e < num_epochs; ++e)
    {
        auto context = ethash::create_epoch_context(e);
        auto block_numbers = {
            e * epoch_length,
            e * epoch_length + period_length - 1,
            e * epoch_length + period_length,
            e * epoch_length + 2 * period_length - 1,
            (e + 1) * epoch_length - period_length,
            (e + 1) * epoch_length - 1,
        };
        for (auto b : block_numbers)
        {
            auto i = uint64_t(b);
            auto nonce = i * i * i * 977 + i * i * 997 + i * 1009;
            auto r = hash(*context, b, h, nonce);

            std::cout << "{" << b << ", \"" << to_hex(h) << "\", \"" << std::hex
                      << std::setfill('0') << std::setw(16) << nonce << std::dec << "\", \""
                      << to_hex(r.mix_hash) << "\", \"" << to_hex(r.final_hash) << "\"},\n";

            h = r.final_hash;
        }
    }
}
#endif