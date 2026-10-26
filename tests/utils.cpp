#include <doctest/doctest.h>

import std;
import gm.types;
import gm.utils;

using namespace gm;

template <class R, class T>
    requires std::is_arithmetic_v<R> && std::is_arithmetic_v<T>
R static_cast_check(T num) noexcept {
    return gm::saturating_cast<R>(num) == static_cast<R>(num);
}

template <std::floating_point T>
bool is_equal(T num1, T num2) noexcept {
    return std::abs(num1 - num2)
           < std::numeric_limits<T>::epsilon() * 8 * std::max({ std::abs(num1), std::abs(num2), T{ 1 } });
}

TEST_SUITE("gm2") {
    TEST_CASE("utils") {
        SUBCASE("saturating_cast") {
            static constexpr i32 i32_max{ std::numeric_limits<i32>::max() };
            static constexpr i32 i32_min{ std::numeric_limits<i32>::lowest() };
            static constexpr u32 u32_max{ std::numeric_limits<u32>::max() };
            static constexpr i64 i64_max{ std::numeric_limits<i64>::max() };
            static constexpr i64 i64_min{ std::numeric_limits<i64>::lowest() };
            static constexpr f32 f32_max{ std::numeric_limits<f32>::max() };
            static constexpr f32 f32_min{ std::numeric_limits<f32>::lowest() };
            static constexpr f64 f64_max{ std::numeric_limits<f64>::max() };
            static constexpr f64 f64_min{ std::numeric_limits<f64>::lowest() };

            CHECK(static_cast_check<i32>(i32{}));
            CHECK(static_cast_check<i32>(i32{ 1 }));
            CHECK(static_cast_check<i32>(i32{ -1 }));
            CHECK(static_cast_check<i32>(i32_max));
            CHECK(static_cast_check<i32>(i32_min));

            CHECK(static_cast_check<i64>(i32{}));
            CHECK(static_cast_check<i64>(i32{ 1 }));
            CHECK(static_cast_check<i64>(i32{ -1 }));
            CHECK(static_cast_check<i64>(i32_max));
            CHECK(static_cast_check<i64>(i32_min));

            CHECK(static_cast_check<i32>(i64{}));
            CHECK(static_cast_check<i32>(i64{ 1 }));
            CHECK(static_cast_check<i32>(i64{ -1 }));
            CHECK(static_cast_check<i32>(i64{ i32_max }));
            CHECK(static_cast_check<i32>(i64{ i32_min }));
            CHECK(saturating_cast<i32>(i64{ i32_max } + 1) == i32_max);
            CHECK(saturating_cast<i32>(i64{ i32_min } - 1) == i32_min);
            CHECK(saturating_cast<i32>(i64_max) == i32_max);
            CHECK(saturating_cast<i32>(i64_min) == i32_min);

            CHECK(static_cast_check<u32>(i32{}));
            CHECK(static_cast_check<u32>(i32{ 1 }));
            CHECK(saturating_cast<u32>(i32{ -1 }) == 0);
            CHECK(static_cast_check<u32>(i32_max));
            CHECK(saturating_cast<u32>(i32_min) == 0);

            CHECK(static_cast_check<i32>(u32{}));
            CHECK(static_cast_check<i32>(u32{ 1 }));
            CHECK(static_cast_check<i32>(u32{ i32_max }));
            CHECK(saturating_cast<i32>(static_cast<u32>(i32_max) + 1) == i32_max);
            CHECK(saturating_cast<i32>(u32_max) == i32_max);

            CHECK(static_cast_check<f64>(f32{}));
            CHECK(static_cast_check<f64>(f32{ 1 }));
            CHECK(static_cast_check<f64>(f32{ -1 }));
            CHECK(static_cast_check<f64>(f32_max));
            CHECK(static_cast_check<f64>(f32_min));
            CHECK(saturating_cast<f64>(std::numeric_limits<f32>::quiet_NaN()) == 0);
            CHECK(saturating_cast<f64>(std::numeric_limits<f32>::infinity()) == f64_max);
            CHECK(saturating_cast<f64>(-std::numeric_limits<f32>::infinity()) == f64_min);

            CHECK(static_cast_check<f32>(f64{}));
            CHECK(static_cast_check<f32>(f64{ 1 }));
            CHECK(static_cast_check<f32>(f64{ -1 }));
            CHECK(static_cast_check<f32>(f64{ f32_max }));
            CHECK(static_cast_check<f32>(f64{ f32_min }));
            CHECK(saturating_cast<f32>(std::nextafter(f64{ f32_max }, f64_max)) == f32_max);
            CHECK(saturating_cast<f32>(std::nextafter(f64{ f32_min }, f64_min)) == f32_min);
            CHECK(saturating_cast<f32>(f64_max) == f32_max);
            CHECK(saturating_cast<f32>(f64_min) == f32_min);
            CHECK(saturating_cast<f32>(std::numeric_limits<f64>::quiet_NaN()) == 0);
            CHECK(saturating_cast<f32>(std::numeric_limits<f64>::infinity()) == f32_max);
            CHECK(saturating_cast<f32>(-std::numeric_limits<f64>::infinity()) == f32_min);

            CHECK(static_cast_check<f32>(i32{}));
            CHECK(static_cast_check<f32>(i32{ 1 }));
            CHECK(static_cast_check<f32>(i32{ -1 }));
            CHECK(static_cast_check<f32>(i32_max));
            CHECK(static_cast_check<f32>(i32_min));

            CHECK(static_cast_check<i32>(f32{}));
            CHECK(static_cast_check<i32>(f32{ 1 }));
            CHECK(static_cast_check<i32>(f32{ -1 }));
            CHECK(static_cast_check<i32>(std::nextafter(static_cast<f32>(i32_max), f32_min)));
            CHECK(static_cast_check<i32>(f32{ i32_min }));
            CHECK(saturating_cast<i32>(static_cast<f32>(i32_max)) == i32_max);
            CHECK(saturating_cast<i32>(std::nextafter(f32{ i32_min }, f32_min)) == i32_min);
            CHECK(saturating_cast<i32>(f32_max) == i32_max);
            CHECK(saturating_cast<i32>(f32_min) == i32_min);
            CHECK(saturating_cast<i32>(std::numeric_limits<f32>::quiet_NaN()) == 0);
            CHECK(saturating_cast<i32>(std::numeric_limits<f32>::infinity()) == i32_max);
            CHECK(saturating_cast<i32>(-std::numeric_limits<f32>::infinity()) == i32_min);
        }

        SUBCASE("deg_to_rad") {
            CHECK(is_equal(deg_to_rad(0.), 0.));
            CHECK(is_equal(deg_to_rad(180.), std::numbers::pi));
            CHECK(is_equal(deg_to_rad(-180.), -std::numbers::pi));
        }

        SUBCASE("lerp_color") {
            CHECK(lerp_color(0x00000000, 0xffffffff, 0) == 0x00000000);
            CHECK(lerp_color(0x00000000, 0xffffffff, 1) == 0xffffffff);
            CHECK(lerp_color(0x11223344, 0x55667788, .25) == 0x22334455);
        }

        SUBCASE("bilerp_color") {
            CHECK(bilerp_color(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff, 0, 0) == 0xff000000);
            CHECK(bilerp_color(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff, 1, 0) == 0x00ff0000);
            CHECK(bilerp_color(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff, 0, 1) == 0x0000ff00);
            CHECK(bilerp_color(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff, 1, 1) == 0x000000ff);
            CHECK(bilerp_color(0x00000000, 0x00ff0000, 0x0000ff00, 0x000000ff, .25, .25) == 0x00303010);
        }

        SUBCASE("swap_red_blue") {
            CHECK(swap_red_blue(0) == 0);
            CHECK(swap_red_blue(0x12345678) == 0x12785634);
            CHECK(swap_red_blue(0x12785634) == 0x12345678);
        }

        SUBCASE("make_rgb") {
            CHECK(make_rgb(0, 0, 0) == 0xff000000);
            CHECK(make_rgb(255, 255, 255) == 0xffffffff);
            CHECK(make_rgb(255, 0, 0) == 0xffff0000);
            CHECK(make_rgb(0, 255, 0) == 0xff00ff00);
            CHECK(make_rgb(0, 0, 255) == 0xff0000ff);
            CHECK(make_rgb(0x12, 0x34, 0x56) == 0xff123456);
            CHECK(make_rgb(0x12, 0x34, 0x56, 0x78) == 0x78123456);
            CHECK(make_rgb(0, 0, 0, 0) == 0x00000000);
        }

        SUBCASE("make_hsv") {
            CHECK(make_hsv(0, 0, 0) == 0xff000000);
            CHECK(make_hsv(0, 0, 128) == 0xff808080);
            CHECK(make_hsv(0, 0, 255) == 0xffffffff);
            CHECK(make_hsv(0, 255, 0) == 0xff000000);
            CHECK(make_hsv(0, 255, 128) == 0xff800000);
            CHECK(make_hsv(0, 255, 255) == 0xffff0000);
            CHECK(make_hsv(85, 255, 255) == 0xff00ff00);
            CHECK(make_hsv(170, 255, 255) == 0xff0000ff);
            CHECK(make_hsv(149, 202, 86) == 0xff123456);
            CHECK(make_hsv(149, 202, 86, 0x78) == 0x78123456);
            CHECK(make_hsv(0, 0, 0, 0) == 0x00000000);
        }

        SUBCASE("filename") {
            CHECK(filename("") == "");
            CHECK(filename("file.txt") == "file.txt");
            CHECK(filename("C:/path/to/file.txt") == "file.txt");
            CHECK(filename("C:\\path\\to\\file.txt") == "file.txt");
            CHECK(filename(std::source_location::current().file_name()) == "utils.cpp");
        }
    }
}
