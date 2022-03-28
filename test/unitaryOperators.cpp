#include "unitaryOperators.hpp"

QVector<unitaryMatrix> unitaryOperators2x2() {
    QVector<unitaryMatrix> ops;
    complex                a, b, c, d;
    complex                i = complex(0, 1);
    quint64                seed = QRandomGenerator::global()->generate64();
    QRandomGenerator64     rd(seed);

    // 00
    {
        a = sqrt(1.0 / 3.0);
        b = sqrt(2.0 / 3.0);
        c = sqrt(2.0 / 3.0);
        d = -sqrt(1.0 / 3.0);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 01
    {
        a = 0;
        b = -1.;
        c = 1.;
        d = 0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 02
    {
        a = 0;
        b = complex(0, 1);
        c = -complex(0, 1);
        d = 0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 03
    {
        a = -1;
        b = 0;
        c = 0;
        d = -complex(0, 1);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 04
    {
        double t = 1.0 + sqrt(2.0);
        double t1 = 1.0 - sqrt(2.0);
        a = t / sqrt(6.0);
        b = t1 / sqrt(6.0);
        c = -t1 / sqrt(6.0);
        d = t / sqrt(6.0);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 05 gate G(q)
    {
        rd.seed(seed);
        double q = rd.generateDouble();
        a = sqrt(q);
        b = -sqrt(1.0 - q);
        c = sqrt(1.0 - q);
        d = sqrt(q);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 06 gate G(p)
    {
        rd.seed(seed);
        int p = rd.bounded(1, 100);
        a = sqrt(1.0 / p);
        b = -sqrt(1.0 - 1.0 / p);
        c = sqrt(1.0 - 1.0 / p);
        d = sqrt(1.0 / p);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 07 approximate Z-X-decomposition for X^(-1/4)
    {
        double k1 = cos(M_PI / 8.0) * cos(M_PI / 8.0);
        double k2 = sin(M_PI / 8.0) * sin(M_PI / 8.0);
        double k3 = sin(M_PI / 8.0) * cos(M_PI / 8.0);
        a = complex(k1, -k3);
        b = complex(k2, k3);
        c = complex(k2, k3);
        d = complex(k1, -k3);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 08 Z-X-decomposition for Z^(1/4)
    {
        a = 1.0;
        b = 0;
        c = 0;
        d = 1.0 / sqrt(2.0) * complex(1, 1);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 09 approximate Z-X-decomposition for X^(1/4)
    {
        double k1 = cos(M_PI / 8.0) * cos(M_PI / 8.0);
        double k2 = sin(M_PI / 8.0) * sin(M_PI / 8.0);
        double k3 = sin(M_PI / 8.0) * cos(M_PI / 8.0);
        a = complex(k1, k3);
        b = complex(k2, -k3);
        c = complex(k2, -k3);
        d = complex(k1, k3);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 10 approximate Z-X-decomposition for Y^(1/8)
    {
        a = complex(0.9619, 0.1913);
        b = complex(-0.1913, -0.0381);
        c = complex(0.1913, 0.0381);
        d = complex(0.9619, 0.1913);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 11
    {
        a = 0;
        b = -1.0;
        c = 1.0;
        d = 0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 12
    {
        double t = 1.0 / sqrt(2.0);
        a = t * i;
        b = -t;
        c = -t;
        d = t * i;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 13
    {
        double t = 1.0 / sqrt(2.0);
        a = -t * i;
        b = -t;
        c = t;
        d = t * i;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 14
    {
        double t = 1.0 / sqrt(2.0);
        a = t;
        b = t * i;
        c = t * i;
        d = t;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 15
    {
        double t = 1.0 / sqrt(2.0);
        a = t * i;
        b = t;
        c = t;
        d = t * i;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 16
    {
        a = complex(1, 1) / 2.0;
        b = complex(1, 1) / 2.0;
        c = complex(-1, 1) / 2.0;
        d = complex(1, -1) / 2.0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 17
    {
        a = complex(1, 1) / 2.0;
        b = complex(-1, -1) / 2.0;
        c = complex(1, -1) / 2.0;
        d = complex(1, -1) / 2.0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 18 Rk operator for quantum Fourier transform
    {
        rd.seed(seed);
        int k = rd.bounded(1, 9);
        a = 1;
        b = 0;
        c = 0;
        d = exp(i * M_PI / pow(2.0, k - 1));
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 19 Phase(g)
    {
        rd.seed(seed);
        double g = rd.bounded(360.);
        g *= M_PI / 180.0;
        a = 1.0;
        b = 0;
        c = 0;
        d = exp(i * g);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 20 sqrt(H)
    {
        double t1 = 4 + 2 * sqrt(2.0), t2 = 4 - 2 * sqrt(2.0);
        a = (3.0 + 2.0 * sqrt(2.0)) / t1 + i * (3.0 - 2.0 * sqrt(2.0)) / t2;
        b = (1 + sqrt(2.0)) / t1 + i * (1 - sqrt(2.0)) / t2;
        c = (1 + sqrt(2.0)) / t1 + i * (1 - sqrt(2.0)) / t2;
        d = 1.0 / t1 + i * 1.0 / t2;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 21
    {
        a = sqrt(6) / 4.0 + i * sqrt(2) / 4.0;
        b = -sqrt(2) / 4.0 + i * sqrt(6) / 4.0;
        c = sqrt(2) / 4.0 + i * sqrt(6) / 4.0;
        d = sqrt(6) / 4.0 - i * sqrt(2) / 4.0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 22
    {
        double t = sqrt(1.0 / 3.0) + sqrt(1.0 / 6.0);
        double s = sqrt(1.0 / 3.0) - sqrt(1.0 / 6.0);
        a = t;
        b = s;
        c = -s;
        d = t;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 23
    {
        a = sqrt(2.0 / 3.0);
        b = 1.0 / sqrt(3.0);
        c = 1.0 / sqrt(3.0);
        d = -sqrt(2.0 / 3.0);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 24
    {
        a = sqrt(3.0) / 2.0;
        b = 0.5;
        c = 0.5;
        d = -sqrt(3.0) / 2.0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 25
    {
        a = -sqrt(2.0) / 2.0;
        b = -i * sqrt(2.0) / 2.0;
        c = i * sqrt(2.0) / 2.0;
        d = sqrt(2.0) / 2.0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 26
    {
        double  t = 1.0 / sqrt(2.0);
        complex fi = exp(i * M_PI / 4.0);
        a = fi * t;
        b = -fi * t * i;
        c = -fi * t * i;
        d = fi * t;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 27 quantum operator HTH
    {
        a = 0.5 * (complex(1, 0) + exp(i * M_PI / 4.0));
        b = 0.5 * (complex(1, 0) - exp(i * M_PI / 4.0));
        c = 0.5 * (complex(1, 0) - exp(i * M_PI / 4.0));
        d = 0.5 * (complex(1, 0) + exp(i * M_PI / 4.0));
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 28
    {
        a = complex(1, -1) / 2.0;
        b = complex(1, 1) / 2.0;
        c = complex(1, 1) / 2.0;
        d = complex(1, -1) / 2.0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 29 quantum operator T(HTH)
    {
        complex r = exp(i * M_PI / 4.0);
        a = (1.0 + r) / 2.0;
        b = (1.0 - r) / 2.0;
        c = (r - i) / 2.0;
        d = (r + i) / 2.0;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 30
    {
        a = complex(1.0 / sqrt(2.0), -1.0 / sqrt(2.0));
        b = 0;
        c = 0;
        d = complex(1.0 / sqrt(2.0), 1.0 / sqrt(2.0));
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 31
    {
        a = complex(0.98077, 0);
        b = complex(0.138, -0.138);
        c = complex(-0.138, -0.138);
        d = complex(0.98077, 0);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 32
    {
        double t = 1.0 / 2.0;
        a = t * complex(1, 1);
        b = t * complex(1, -1);
        c = t * complex(-1, -1);
        d = t * complex(1, -1);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 33
    {
        double t = 1.0 / 2.0;
        a = t * complex(-1, -1);
        b = t * complex(1, 1);
        c = t * complex(-1, 1);
        d = t * complex(-1, 1);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 34
    {
        double z1 = cos(M_PI / 8.0);
        double z2 = sin(M_PI / 8.0);
        a = z1 * z1 + i * z2 * z2;
        b = (complex(1, 0) - i) * z2 * z1;
        c = (complex(1, 0) - i) * z2 * z1;
        d = z2 * z2 + i * z1 * z1;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 35
    {
        double z1 = cos(M_PI / 8.0);
        double z2 = sin(M_PI / 8.0);
        a = z1 * z1 - i * z2 * z2;
        b = (complex(1, 0) + i) * z2 * z1;
        c = (complex(1, 0) + i) * z2 * z1;
        d = z2 * z2 - i * z1 * z1;
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 36 V(tetha, phi) from A. Barenco
    {
        rd.seed(seed);
        double teta = rd.bounded(360.);
        double phi0 = rd.bounded(360.);
        teta = teta * M_PI / 180.0;
        phi0 = phi0 * M_PI / 180.0;
        a = cos(teta / 2.0);
        b = -i * exp(-i * phi0) * sin(teta / 2.0);
        c = -i * exp(i * phi0) * sin(teta / 2.0);
        d = cos(teta / 2.0);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 37
    {
        rd.seed(seed);
        int    n = rd.bounded(0, 1000);
        double alpha;
        alpha = sqrt(n);
        a = i * cos(M_PI * alpha / 2.0);
        b = sin(M_PI * alpha / 2.0);
        c = sin(M_PI * alpha / 2.0);
        d = i * cos(M_PI * alpha / 2.0);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    // 38
    {
        rd.seed(seed);
        double theta = rd.bounded(360.);
        theta = theta * M_PI / 180.0;
        a = cos(theta);
        b = sin(theta);
        c = sin(theta);
        d = -cos(theta);
        ops.append(unitaryMatrix{a, b, c, d});
    }

    return ops;
}
