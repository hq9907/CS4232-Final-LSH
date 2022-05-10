#include <Eigen/Dense>
#include "util.cpp"
#include <cstdlib>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

template <int D>
class LSH_Forest {

    private:

        static const unsigned int PARTITION_NUM = 2;

        typedef Eigen::Vector<float, D> Point;

        struct Node {
            vector<Node*> children;
            vector<Point> pivots;
            Point vec; // the spliting vector
            float threshold; // and the threshold
        };

        struct L2cmp {
            Point base;
            L2cmp(Point base): base(base) {}

            bool operator() (Point a, Point b) {
                return (a - base).squaredNorm() < (b - base).squaredNorm();
            }
        };

        struct DotCmp {
            Point vec;
            DotCmp(Point vec): vec(vec) {}

            bool operator() (Point a, Point b) {
                return vec.dot(a) < vec.dot(b);
            }
        };



        default_random_engine generator;

        Eigen::Vector<float, D> randomVec() {
            Eigen::Vector<float, D> v;
            normal_distribution<double> distribution(0, 1);

            for (int i = 0; i < D; ++i) {
                v(i) = distribution(generator);
            }

            return v;
        }


        vector<Node*> roots;
        Point center;
        float c, r, k;

        vector<Point> computePivots(vector<Point> &data, int begin, int end, int k) {
            Point pc = mean(data, begin, end);
            vector<Point> pivots;

            sort(data.begin() + begin, data.begin() + end, L2cmp(pc));

            for (int i = begin; i < end; ++i) {
                if (l2Dist(data[i], pivots) > (c - 1) * r) {
                    pivots.push_back(data[i]);
                    if (pivots.size() >= k)
                        return pivots;
                }
            }
            return pivots;
        }

        Node *buildTree(vector<Point> &data,
                        int begin,
                        int end) {
            Node *node = new Node;

            // cout << "build:" << begin << ' ' << end << endl;

            node->pivots = computePivots(data, begin, end, k);

            if (end - begin == 1)
                return node;

            // Euclidean LSH partition
            // Project all points to a random line, and split them
            // TODO: it's a simple bipartitioning now

            vector<Point> partition[PARTITION_NUM];

            node->vec = randomVec(); // select random vector

            int mid = (begin + end) / 2;
            nth_element(data.begin() + begin,
                        data.begin() + mid,
                        data.begin() + end,
                        DotCmp(node->vec)); // find median

            node->threshold = data[mid].dot(node->vec);
            node->children.push_back(buildTree(data, begin, mid));
            node->children.push_back(buildTree(data, mid, end));

            return node;
        }

        Point *queryTree(Node *node, Point point) {
            for (Point &p: node->pivots)
                if (l2Dist(p, point) < c * r)
                    return &p;

            if (node->children.size() < PARTITION_NUM)
                return NULL;

            Node *c = node->children[point.dot(node->vec) >= node->threshold];
            Point *ans = queryTree(c, point);
            if (ans)
                return ans;

            return NULL;
        }

        void freeTree(Node *node) {
            for (Node *c: node->children)
                freeTree(c);
            free(node);
        }

    public:

        LSH_Forest(float c, float r, float k): c(c),r(r),k(k) {
        }

        ~LSH_Forest() {
            for (Node *root: roots)
                freeTree(root);
        }

        void build(vector<Point> data, int num = 1) {
            for (int i = 0; i < num; ++i)
                roots.push_back(buildTree(data, 0, data.size()));
        }

        Point *query(Point p) {
            for (Node *root: roots) {
                Point *ans = queryTree(root, p);
                if (ans)
                    return ans;
            }
            return NULL;
        }
};
