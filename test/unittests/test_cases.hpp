// Copyright 2018-2019 Pawel Bylica.
// Licensed under the Apache License, Version 2.0.

/// @file
/// Shared test cases.

#pragma once

// Put in anonymous namespace to allow be include in multiple files
// but also make iteration over test cases easy with range-based for loop.
namespace
{
struct hash_test_case
{
    int block_number;
    const char* header_hash_hex;
    const char* nonce_hex;
    const char* mix_hash_hex;
    const char* final_hash_hex;
};

hash_test_case hash_test_cases[] = {
    {
        0,
        "2a8de2adf89af77358250bf908bf04ba94a6e8c3ba87775564a41d269a05e4ce",
        "4242424242424242",
        "89b6b75f64a89b05393536d14ccea1f8b40d8dffab98d5a812e2f9210d5118d3",
        "89b612bfaf68f940af983f44fc22df6dfc2836a8f6d5f9da4ebe483d868761c5",
    },
    {
        2,
        "100cbec5e5ef82991290d0d93d758f19082e71f234cf479192a8b94df6da6bfe",
        "307692cf71b12f6d",
        "8cdb6f34b433e7871a672c925a7516c469596739b720112b8513e34843cd811f",
        "c6ad74a4ea186a1563bc42606601ab8100712c828394b9eb7f5d37bb5905ca0a",
    },
    {
        2683077,
        "0313d03c5ed78694c90ecb3d04190b82d5b222c75ba4cab83383dde4d11ed512",
        "8c5eaec000788d41",
        "3e0751d22090b810d475d921b710ff5bbb00a1c1d6feffa3f2adb4d34c9f203f",
        "3161ada5c55169123495e5bf124ff111aac62ebc73bd237c1555bac8b4f6576b",
    },
    {
        5000000,
        "bc544c2baba832600013bd5d1983f592e9557d04b0fb5ef7a100434a5fc8d52a",
        "4617a20003ba3f25",
        "81bdeb1ff0b5e149e646335cfa48c3d6044c73fbdc476e83c7523bf95409d792",
        "0782c17aadfbd6294d98bac1b900adbe8e5140c05c627f6b52f435c36fc1d54b",
    },
    {
        5000001,
        "2cd14041cfc3bd13064cfd58e26c0bddf1e97a4202c4b8076444a7cd4515f8c3",
        "1af47f2007922384",
        "5962b39de8365643a4c4e69b62d9ef153f5398f8296d2d242a130f153b5c7896",
        "2d3d623fb95e299e7c71cca6f702ea2e89186f0373ee0d0cd1a253be4f79cee6",
    },
    {
        5000002,
        "9e79bced19062baf7c47e516ad3a1bd779222404d05b4205def30a13c7d87b5b",
        "c9a044201dd998f2",
        "6bd7a9a2d5d2f4c34512c0e7b74f11001353a8938222e36a53ff0e73f6679a18",
        "86f7ad63ea3628c9d8a705870abf123478c6595d8f37c045b1b0625e0b66d668",
    },
    {
        5306861,
        "53a005f209a4dc013f022a5078c6b38ced76e767a30367ff64725f23ec652a9f",
        "d337f82001e992c5",
        "47b1283f5ab6ab2e393cd7212e56247bc76efca89b664c0b8f10470c4fdfa131",
        "f7a17204787a007e1ccec1c51e352fd21256f8b40758c9868aec480eb1d0373d",
    },
};
}  // namespace