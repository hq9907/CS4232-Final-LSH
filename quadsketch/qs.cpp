#include "serialize.h"

#include <boost/program_options.hpp>

#include <chrono>
#include <fstream>
#include <map>
#include <iostream>
#include <random>
#include <vector>
#include <string>

using namespace std;
using namespace std::chrono;
using namespace ir;
namespace po = boost::program_options;

vector<Point> new_dataset;
const unsigned int D = 68;

vector<Point> readData() {

    ifstream f("../dataset/USCensus1990.data.txt");
    vector<Point> data;
    Point p = Point::Zero(D);
    string line, token;

    if (f.is_open()) {
        while (getline(f, line)) {
            // skip the first row
            if (line[0] < '0' || line[0] > '9')
                continue;

            istringstream iss(line);
            float x;
            getline(iss, token, ','); // skip the first column
            for (int i = 0; i < D; ++i) {
                getline(iss, token, ',');
                x = stof(token);
                p[0] = x;
            }
            data.push_back(p);

            // TODO: Short cut for testing
            // if (data.size() >= 10000)
            //     break;
        }
        f.close();
    } else {
        cout << "Can't open file" << endl;
    }
		return data;
}


void solve(const vector<Point> &dataset,
					int b1,
					int b2,
					const vector<pair<float, float> > &bb,
					const vector<int> &who,
					deque<Point> & last_lambda_points,
					int depth,
					int max_depth,
					uint64_t *num_leaves,
					uint64_t *dfs_size,
					uint64_t *reduced_dfs_size,
					uint64_t *num_reduced_edges,
					int *max_reduced_edge,
					const Point &cur,
					int long_edge_length,
					int lambda) 
{
    int n = dataset.size();
    int d = b2 - b1;
    int dd = d / 8;
    if (d % 8) ++dd;
    if (long_edge_length - 2 * lambda >= *max_reduced_edge) {
      *max_reduced_edge = long_edge_length - 2 * lambda;
    }
    if (depth >= max_depth) {
        ++(*num_leaves);
        for (auto x: who) {
						
            for (int i = 0; i < d; ++i) {
								double value = 0;
								for (int j = 0; j < last_lambda_points.size(); ++j) {
										value += last_lambda_points[j][i];
								}
								
                new_dataset[x][b1 + i] = cur[i] + value;
            }
        }
        return;
    }
    vector<float> mm;
    for (auto x: bb) {
        mm.push_back((x.first + x.second) / 2.0);
    }
    map<vector<uint8_t>, vector<int>> parts;
    for (auto x: who) {
        vector<uint8_t> code(dd, 0);
        for (int j = 0; j < d; ++j) {
            if (dataset[x][b1 + j] < mm[j]) {
            }
            else {
                code[j / 8] |= 1 << (j % 8);
            }
        }
        parts[code].push_back(x);
    }
		

		Point tentative_point = Point::Zero(d);
		
    for (auto x: parts) {
        vector<pair<float, float> > newbb(bb.size());
        Point new_p(cur);
        for (int j = 0; j < d; ++j) {
            newbb[j] = bb[j];
            if ((x.first[j / 8] >> (j % 8)) & 1) {
                newbb[j].first = mm[j];
								if (long_edge_length >= lambda && parts.size() == 1) {
									tentative_point[j] += 1.0 / (2.0 * (1 << depth));
									double value = 0;
									for (int i = 0; i < last_lambda_points.size(); ++i) {
											value += last_lambda_points[i][j];
									}
                	new_p[j] += value;
									continue;
								}
								new_p[j] += 1.0 / (2.0 * (1 << depth));
						}
						else {
								newbb[j].second = mm[j];
						}
				}
				if (long_edge_length >= lambda && parts.size() == 1) {
						last_lambda_points.push_back(tentative_point);
						if(last_lambda_points.size() > lambda) {
							last_lambda_points.pop_front();
						}
				} else {
						last_lambda_points.clear();	
				}

				(*dfs_size) += 2 + d;
				(*reduced_dfs_size) += 2;
				if (long_edge_length >= lambda && parts.size() == 1) {
				}
				else {
					(*reduced_dfs_size) += 1 + d;
				}
				if (long_edge_length == lambda && parts.size() == 1) {
					++(*num_reduced_edges);
					(*reduced_dfs_size) += 1;
				}
        solve(dataset,
	      b1,
	      b2,
	      newbb,
	      x.second,
				last_lambda_points,
	      depth + 1,
	      max_depth,
	      num_leaves,
	      dfs_size,
	      reduced_dfs_size,
	      num_reduced_edges,
	      max_reduced_edge,
	      new_p,
	      (parts.size() > 1) ? 1 : long_edge_length + 1,
	      lambda);
    }
}


int main(int argc, char ** argv) {
		
    cout << "Reading data..." << endl;
    vector < Point > dataset = readData();

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print usage message")
    ("output,o", po::value < string > (), "output file")
    ("depth,d", po::value < int > (), "depth of a tree")
    ("num_blocks,n", po::value < int > (), "number of blocks")
    ("lambda,l", po::value < int > (), "compression parameter")
    ("num_queries,q", po::value < int > (), "number of queries used for evaluation");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        cout << desc << endl;
        return 0;
    }
    if (!vm.count("output") || !vm.count("depth") || !vm.count("num_blocks") || !vm.count("lambda") || !vm.count("num_queries")) {
        cout << desc << endl;
        throw runtime_error("output file, number of blocks, the compression parameter and depth of a tree must be specified");
    }
    string output_file = vm["output"].as < string > ();
    int num_blocks = vm["num_blocks"].as < int > ();
    int depth = vm["depth"].as < int > ();
    int lambda = vm["lambda"].as < int > ();
		int num_queries = vm["num_queries"].as < int > ();
    if (num_blocks <= 0) {
        cout << desc << endl;
        throw runtime_error("number of blocks must be positive");
    }
    if (depth <= 0) {
        cout << desc << endl;
        throw runtime_error("depth must be positive");
    }
    if (lambda <= 0) {
        cout << desc << endl;
        throw runtime_error("lambda must be positive");
    }
    if (lambda > depth) {
        cout << desc << endl;
        throw runtime_error("lambda must be at most depth");
    }
    random_device rd;
    mt19937_64 gen(rd());

		std::default_random_engine generator;
		std::uniform_real_distribution<double> distribution(-1,1);

    vector < Point > queries;
		
		// Generate some queries
		for(int i = 0; i < num_queries; ++i) {
			int idx = rand() % dataset.size();
			auto query = dataset[idx];
			for (int j = 0; j < query.size(); ++j) {
				query[j] += distribution(generator);
			}
			queries.push_back(query);
		}


    int q = queries.size();
    for (auto x: queries) {
        dataset.push_back(x);
    }
    int n = dataset.size();
    int d = dataset[0].size();
    cout << n << " " << d << endl;
    float cmin = 1e100;
    float cmax = -1e100;
    for (int i = 0; i < d; ++i) {
        for (int j = 0; j < n; ++j) {
            cmin = min(cmin, dataset[j][i]);
            cmax = max(cmax, dataset[j][i]);
        }
    }
    float delta = cmax - cmin;
    cmin -= delta;
    uniform_real_distribution < float > u(0.0, delta);
    vector < int > all(n);
    for (int i = 0; i < n; ++i) {
        all[i] = i;
    }
    int start = 0;
    uint64_t total = 0;
    new_dataset.resize(n);
    for (int i = 0; i < n; ++i) {
        new_dataset[i].resize(d);
    }
		auto start_time = chrono::steady_clock::now();
    for (int block_id = 0; block_id < num_blocks; ++block_id) {
        int block_len = d / num_blocks;
        if (block_id < d % num_blocks) {
            ++block_len;
        }
        cout << start << " " << start + block_len << endl;
        vector < pair < float, float > > bb;
        for (int i = 0; i < d; ++i) {
            float s = u(gen);
            bb.push_back(make_pair(cmin + s, cmax + s));
        }
        uint64_t num_leaves = 0;
        uint64_t dfs_size = 0;
        uint64_t reduced_dfs_size = 0;
        uint64_t num_reduced_edges = 0;
        int max_reduced_edge = 0;
        deque < Point > last_lambda_points;
				
        solve(dataset, start, start + block_len, bb, all, last_lambda_points, 0, depth, & num_leaves, & dfs_size, & reduced_dfs_size, & num_reduced_edges, & max_reduced_edge, Point::Zero(block_len), 0, lambda);
				

        cout << num_leaves << " " << reduced_dfs_size << " " << num_reduced_edges << " " << max_reduced_edge << endl;
        start += block_len;
    }
		auto end_time = chrono::steady_clock::now();
		cout << "Elapsed time in milliseconds: " << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms" << endl;

		cout << "Completed" << endl;
    cout << endl;
    // cout << (counter + 0.0) / (q + 0.0) << endl;
    // cout << "distortion " << (distortion + 0.0) / (q + 0.0) << endl;
    ofstream output(output_file);
    output << "method qs" << endl;
    // output << "dataset " << input_folder << endl;
    output << "num_blocks " << num_blocks << endl;
    output << "depth " << depth << endl;
    output << "lambda " << lambda << endl;
    output << "size " << total << endl;
    output.close();
    return 0;
}