#include "../../hnswlib/hnswlib.h"
#include <string>
#include <chrono>
#include <iostream>
#include <memory>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

// #define TEST

template <typename Dat_Type>
Dat_Type *loadBinPtr(std::string srcPath, size_t &nRow, size_t &nDim) {

    std::streampos fileSize;
    std::ifstream inStrm(srcPath, std::ios::binary);
    if(!inStrm.is_open()){
        std::cerr << "File '" << srcPath << "' cannot open for read!\n";
        exit(0) ;
    }
    unsigned int dim = 0;
    inStrm.seekg(0, std::ios::end);
    fileSize = inStrm.tellg();
    inStrm.seekg(0, std::ios::beg);
    inStrm.read((char *)&dim, sizeof(unsigned int));
    unsigned int size_n = (double)fileSize / (sizeof(unsigned int) + dim * sizeof(Dat_Type));

    nRow = size_n;
    nDim = dim;
    Dat_Type *matrix = new Dat_Type[nRow * nDim];

    inStrm.seekg(0, std::ios::beg);
    for (size_t i = 0; i < size_n; ++i)
    {
        inStrm.read((char *)&dim, sizeof(unsigned int));
        inStrm.read((char *)(matrix + i * nDim), nDim * sizeof(Dat_Type));
    }
    inStrm.close();
    return matrix;
}


template <typename Dat_Type>
std::vector<std::vector<Dat_Type>> loadBinVec(std::string in_pth) {
    std::vector<std::vector<Dat_Type>> matrix;

    std::streampos fileSize; 
    std::ifstream data(in_pth, std::ios::binary);

    unsigned int dim = 0; 

    data.seekg(0, std::ios::end);
    fileSize = data.tellg();
    data.seekg(0, std::ios::beg);
    data.read((char *) & dim, sizeof(unsigned int));

    data.seekg(0, std::ios::beg);

    while (data.read((char *) & dim,   sizeof(unsigned int))) {
        std::vector<Dat_Type> tmp_row(dim);
        for (auto i = 0; i < dim; ++i) {
            data.read((char *) & tmp_row[i], sizeof(unsigned));
        }
        matrix.push_back(tmp_row);
    }

    return matrix;
}

bool endsWith(std::string const &str, std::string const &suffix) {
    if (str.length() < suffix.length())
    {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}


float getRecall(std::vector<std::vector<Idx_Type>> &anng, std::vector<std::vector<Idx_Type>> &knng, size_t checkK)
{
    size_t hit = 0;
    size_t checkN = std::min(anng.size(), knng.size());

    for (size_t i = 0; i < checkN; ++i)
    {
        auto &ann = anng[i];
        auto &knn = knng[i];
        for (size_t j = 0; j < checkK; ++j)
        {
            auto idx = ann[j];
            for (size_t l = 0; l < checkK; ++l)
            {
                auto nb = knn[l];
                if (idx == nb)
                {
                    ++hit;
                    break;
                }
            }
        }
    }
    return 1.0 * hit / (checkK * checkN);
}


int runNNSearchCurve(std::string base_path, 
                      std::string query_path,
                      std::string gt_path, 
                      std::string index_path,
                      std::string space_name = "l2",
                      size_t RecallK = 10) {

    auto gt = loadBinVec<unsigned>(gt_path);

    size_t query_size, query_dim; 
    std::unique_ptr<float[]> query(loadBinPtr<float>(query_path, query_size, query_dim));


    std::unique_ptr<AbstractLoader<float>> loader; 
    std::unique_ptr<hnswlib::HierarchicalNSW<float>> alg_hnsw;


    hnswlib::SpaceInterface<float> * space;
    hnswlib::L2Space l2space(query_dim);
    hnswlib::InnerProductSpace ipspace(query_dim);

    if (space_name == "l2") {
        space = & l2space;
    } else if (space_name == "ip") {
        space = & ipspace;
    } else {
        space = & ipspace;
    }

    if (endsWith(index_path, "hnsw")) {
        alg_hnsw.reset(new hnswlib::HierarchicalNSW<float>(space, index_path));
        alg_hnsw->setSearchFunc(true);
    } else {
        if (endsWith(index_path, "ivecs")) {
            loader.reset(new BasicLoader<float>(base_path, index_path));
        } else if (endsWith(index_path, "nsg")) {
            loader.reset(new NSGLoader<float>(base_path, index_path));
        } else if (endsWith(index_path, "ssg")) {
            loader.reset(new SSGLoader<float>(base_path, index_path));
        } else {
            loader.reset(new VamanaLoader<float>(base_path, index_path)); 
        }
        assert(query_dim == loader.dim());
        alg_hnsw.reset(new hnswlib::HierarchicalNSW<float>(space, loader, 32));
        alg_hnsw->setSearchFunc(false); 
    }    

    

#ifdef TEST
{
    auto d = loader->getDataReader(); 
    cout << d->is_open() << "--" << d->good() << endl;
    auto g = loader->getGraphReader(); 
    cout << g->is_open() << "--" << g->good() << endl;

    if (g->is_open()) {  
        if (!g->good()) {  
            std::cerr << "Error: " << g->rdstate() << std::endl;  
        }  
    }

    exit(0);
}
#endif


    std::vector<size_t> search_size_small = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 24, 26, 28, 30, 40, 50, 60, 70, 80, 90, 100, 128, 156, 192, 256, 298, 348, 400, 456, 512};
    std::vector<size_t> search_size_large = {100, 128, 156, 192, 256, 298, 348, 400, 456, 512, 567, 640, 720, 800};
    std::vector<size_t> search_size_list;
    if (RecallK == 10) {
        search_size_list = search_size_small;
    } else {
        search_size_list = search_size_large;
    }

    std::vector<std::vector<unsigned>> searched_res(query_size);
    std::vector<std::pair<float, float>> result(search_size_list.size());

    for (int it = 0; it < 1; it++) {
        for (size_t sz_i = 0; sz_i < search_size_list.size(); ++sz_i) {
            auto search_size = search_size_list[sz_i];
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < query_size; ++i) {
                auto res = alg_hnsw->SearchKNN(query.get() + i * query_dim, search_size);
                while (res.size() > RecallK) {
                    res.pop();
                }
                std::vector<unsigned> tmp;
                while (res.size()) {
                    tmp.emplace_back(res.top().second);
                    res.pop();
                }
                reverse(tmp.begin(), tmp.end());
                searched_res[i] = tmp;
            }
            auto  end = std::chrono::high_resolution_clock::now();
            float QPS = (1.0 * query_size / (1.0 * std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0));
            auto recall = getRecall(searched_res, gt, RecallK);
            result[sz_i].first = std::max(result[sz_i].first, QPS);
            result[sz_i].second = std::max(result[sz_i].second, recall);
        }
    }

    std::cout << "topk,cnt_per_second,recall_at_10" << std::endl;
    for (size_t sz_i = 0; sz_i < search_size_list.size(); ++sz_i) {
        std::cout << search_size_list[sz_i] << "," << result[sz_i].first << "," << result[sz_i].second << std::endl;
    }

    return 0;
}


int main(int argc, char * argv[]) {

    std::string base_path  = "/home/jfwang/data/sift1m/sift1m_base.fvecs";
    std::string query_path = "/home/jfwang/data/sift1m/sift1m_query.fvecs";
    std::string gt_path    = "/home/jfwang/data/sift1m/sift1m_gt.ivecs";
    std::string index_path = "/home/jfwang/data/sift1m/sift1m_TSDGed_K200_SavMem_XNDesc_SCG_1.2_4_50.ivecs"; 
    // std::string index_path = "/home/jfwang/data/sift1m/sift1m_K200_SavMem_XNDesc_NND_NSGed_512.nsg"; 
    // std::string index_path = "/home/jfwang/data/sift1m/sift1m_K200_SavMem_XNDesc_NND_SSGed_512.ssg"; 
    // std::string index_path = "/home/jfwang/data/sift1m/sift1m_HNSW_32_512.hnsw"; 
    // std::string index_path = "/home/jfwang/data/sift1m/sift1m_index_R50_L512_A1.2";

    std::string space_name{"l2"};
    size_t RecallK = 10;

    po::options_description desc{"Arguments"};
    try {
        desc.add_options()("help,h", "Print information on arguments");
        desc.add_options()("base_path",     po::value<std::string>(&base_path)->required(),     "path of vectors, should be in fvecs format.");
        desc.add_options()("query_path",    po::value<std::string>(&query_path)->required(),    "path of queries, should be in fvecs format.");
        desc.add_options()("gt_path",       po::value<std::string>(&gt_path)->required(),       "path of ground_truth, should be in ivecs format.");
        desc.add_options()("index_path",    po::value<std::string>(&index_path)->required(),    "path of indexing graph, support various formats, eg. fvecs, nsg, ssg, vamana, hnsw, etc.");
        desc.add_options()("space_name",    po::value<std::string>(&space_name),                "space_name, the method to calculate distance, l2 (default) or ip");
        desc.add_options()("K",             po::value<std::size_t>(&RecallK),                   "Recall@K: the range to evaluate recall, default value is 10");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.count("help"))
        {
            std::cout << desc;
            return 0;
        }
        po::notify(vm);
    }
    catch(std::exception & e)
    {
        std::cout << "cmd error\n";
        return -1;
    }

    return runNNSearchCurve(base_path, query_path, gt_path, index_path, space_name, RecallK);

    return 0;
}
