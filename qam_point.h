#ifndef QAM_POINT_H
#define QAM_POINT_H

struct qam_point
{
    double i = 0.0;
    double q = 0.0;

    qam_point() = default;
    qam_point(double i_val, double q_val) : i(i_val), q(q_val) {}
    qam_point(const qam_point &x) = default;
    qam_point(qam_point &&x) = default;
    ~qam_point() = default;

    qam_point& operator= (const qam_point &x) = default;
    qam_point& operator= (qam_point &&x) = default;
};

#endif
