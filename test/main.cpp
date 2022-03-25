#include "operator.h"
#include <gtest/gtest.h>

TEST(Test, PushBack) {
    complex a{0, 0};
    complex b{0, -1};
    complex c{0, 1};
    complex d{0, 0};

    decomposition dec = Operator::zxDecomposition(a, b, c, d);

    dec.alpha *= M_PI / 180;
    dec.beta *= M_PI / 180;
    dec.delta *= M_PI / 180;
    dec.gamma *= M_PI / 180;

    complex i{0, 1};
    double  v = dec.gamma / 2.;
    complex aa = exp(i * (dec.alpha - dec.beta / 2.0 - dec.delta / 2.0)) * cos(v);
    complex bb = -i * exp(i * (dec.alpha - dec.beta / 2.0 + dec.delta / 2.0)) * sin(v);
    complex cc = -i * exp(i * (dec.alpha + dec.beta / 2.0 - dec.delta / 2.0)) * sin(v);
    complex dd = exp(i * (dec.alpha + dec.beta / 2.0 + dec.delta / 2.0)) * cos(v);

    EXPECT_TRUE(abs(a.real() - aa.real()) > EPSILON);
    EXPECT_TRUE(abs(a.imag() - aa.imag()) > EPSILON);

    EXPECT_TRUE(abs(b.real() - bb.real()) > EPSILON);
    EXPECT_TRUE(abs(b.imag() - bb.imag()) > EPSILON);

    EXPECT_TRUE(abs(c.real() - cc.real()) > EPSILON);
    EXPECT_TRUE(abs(c.imag() - cc.imag()) > EPSILON);

    EXPECT_TRUE(abs(d.real() - dd.real()) > EPSILON);
    EXPECT_TRUE(abs(d.imag() - dd.imag()) > EPSILON);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
