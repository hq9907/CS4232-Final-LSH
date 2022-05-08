#include <Eigen/Dense>
#include <vector>
#include <limits>

using namespace std;
using namespace Eigen;

const float INF = numeric_limits<float>::infinity();

template<typename T, int D>
Vector<T, D> mean(vector<Vector<T, D>> &data) {
    Vector<T, D> m = Vector<T, D>::Zero();
    for (Vector<T, D> &p: data)
        m += p;
    return m / data.size();
}

template<typename T, int D>
Vector<T, D> mean(vector<Vector<T, D>> &data, int begin, int end) {
    Vector<T, D> m = Vector<T, D>::Zero();
    for (int i = begin; i < end; ++i)
        m += data[i];
    return m / (end - begin);
}

template<typename T, int D>
float l2Dist(Vector<T, D> &p1, Vector<T, D> &p2) {
    return (p1 - p2).squaredNorm();
}

template<typename T, int D>
float l2Dist(Vector<T, D> &point, vector<Vector<T, D>> &s) {
    float dis = INF;
    for (Vector<T, D> &p: s)
        dis = min(dis, l2Dist(p, point));
    return dis;
}
