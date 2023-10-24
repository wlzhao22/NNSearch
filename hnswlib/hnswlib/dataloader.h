#pragma once

#include <fstream> 
#include <cassert>
#include <memory>

using Idx_Type = unsigned; 

template <typename Dat_Type>
class AbstractLoader {
protected:
    std::streampos startReadPoint{0};
    std::ifstream inGraph{nullptr};
    std::ifstream inBaseData{nullptr};
    std::string graphPath{""};
    std::string basePath{""};
    size_t graphSize{0};
    size_t dataSize{0};
    size_t dataDim{0}; 
    bool successOpen{false};
    size_t maxEdges{0};

public:
    AbstractLoader (std::string base_path, std::string graph_path) {
        basePath  = base_path;
        graphPath = graph_path;
    }

    virtual ~AbstractLoader () {
        if (inBaseData.is_open()) {
            inBaseData.close();
        }
        if (inGraph.is_open()) {
            inGraph.close();
        }
    }

    virtual void load_graph() = 0;

    virtual void load_data() {
        inBaseData.open(basePath, std::ios::binary);
        if (! inBaseData.is_open()) {   
            throw std::runtime_error(basePath + " can not open!\n");
        } 
        unsigned int dim = 0;
        inBaseData.seekg(0, std::ios::end);
        std::streampos fileSize = inBaseData.tellg();
        inBaseData.seekg(0, std::ios::beg);
        inBaseData.read((char *)&dim, sizeof(unsigned int));
        size_t size_n = static_cast<size_t>(fileSize / (sizeof(unsigned int) + dim * sizeof(Dat_Type)));
        dataSize = size_n;
        dataDim = static_cast<size_t>(dim); 
        inBaseData.seekg(0, std::ios::beg);
    }

    size_t size() {
        assert(graphSize == dataSize);
        return dataSize;
    }

    size_t dim() {
        return dataDim; 
    }

    size_t getMaxEdges() {
        return maxEdges;
    }

    void refresh() {
        inGraph.clear(); 
        inBaseData.clear(); 
        inGraph.seekg(startReadPoint, std::ios::beg);
        inBaseData.seekg(0, std::ios::beg);
    }

    std::ifstream * getDataReader() {
        return &inBaseData;
    }

    std::ifstream * getGraphReader() {
        return &inGraph;
    }
}; /// end of AbstractLoader 

template <typename Dat_Type>
class BasicLoader : public AbstractLoader<Dat_Type> {

public:
    BasicLoader(std::string base_path, std::string graph_path) : AbstractLoader<Dat_Type>(base_path, graph_path)
    {
        load_graph();
        this->load_data(); 
    }

    void load_graph() {

        this->inGraph.open(this->graphPath, std::ios::binary);  
        
        if (this->inGraph.is_open()) {  
            if (!this->inGraph.good()) {  
                std::cerr << "Error: " << this->inGraph.rdstate() << std::endl;  
            }  
        }
        unsigned int dim = 0;
        std::streampos fileSize;

        this->inGraph.seekg(0, std::ios::beg);
        {   //// statistics max_edges. 
            unsigned max_size_g{0};
            unsigned size_g{0};

            while(this->inGraph.read((char *)&size_g, sizeof(unsigned int))) {
                ++this->graphSize;
                max_size_g = std::max(max_size_g, size_g);
                this->inGraph.seekg(size_g * sizeof(Idx_Type), std::ios::cur);
            }

            this->inGraph.clear();
            this->maxEdges = static_cast<size_t>(max_size_g);
        }

        // std::cout << "load graph: " << this->graphSize << ", max_edge = " << this->maxEdges << "\n";
        this->inGraph.seekg(0, std::ios::beg);
    }

}; /// end of BasicLoader 



template <typename Dat_Type>
class NSGLoader : public AbstractLoader<Dat_Type> {

public:
    NSGLoader(std::string base_path, std::string graph_path) : AbstractLoader<Dat_Type>(base_path, graph_path)
    {
        this->load_data(); 
        load_graph();
    }

    void load_graph() {

        this->inGraph.open(this->graphPath, std::ios::binary);    
        
        if (this->inGraph.is_open()) {  
            if (!this->inGraph.good()) {  
                std::cerr << "Error: " << this->inGraph.rdstate() << std::endl;  
            }  
        }

        this->inGraph.seekg(0, std::ios::beg);
        unsigned width, ep_;
        this->inGraph.read((char *)&width, sizeof(unsigned));
        this->inGraph.read((char *)&ep_, sizeof(unsigned));
        // std::cout << "width = " << width << ", ep = " << ep_ << "\n";

        this->startReadPoint = this->inGraph.tellg(); 

        {   //// statistics max_edges. 
            unsigned max_size_g{0};
            unsigned size_g{0};

            for (size_t i = 0; i < this->dataSize; ++i) {
                this->inGraph.read((char *)&size_g, sizeof(unsigned int));
                ++this->graphSize;
                max_size_g = std::max(max_size_g, size_g);
                this->inGraph.seekg(size_g * sizeof(Idx_Type), std::ios::cur);
            }
            
            this->inGraph.clear();
            this->maxEdges = static_cast<size_t>(max_size_g);
        }

        // std::cout << "load graph: size: " << this->graphSize << ", max_edge = " << this->maxEdges << "\n";
        this->inGraph.clear();
        this->inGraph.seekg(this->startReadPoint, std::ios::beg);

        if (this->inGraph.is_open()) {  
            if (!this->inGraph.good()) {  
                std::cerr << "Error: " << this->inGraph.rdstate() << std::endl;  
            }  
        }
    }
}; // end of NSGLoader 



template <typename Dat_Type>
class SSGLoader : public AbstractLoader<Dat_Type> {

public:
    SSGLoader(std::string base_path, std::string graph_path) : AbstractLoader<Dat_Type>(base_path, graph_path)
    {
        this->load_data(); 
        load_graph();
    }

    void load_graph() {

        this->inGraph.open(this->graphPath, std::ios::binary);    
        
        if (this->inGraph.is_open()) {  
            if (!this->inGraph.good()) {  
                std::cerr << "Error: " << this->inGraph.rdstate() << std::endl;  
            }  
        }

        this->inGraph.seekg(0, std::ios::beg);
        unsigned width;
        this->inGraph.read((char *)&width, sizeof(unsigned));
        unsigned n_ep = 0;
        this->inGraph.read((char *)&n_ep, sizeof(unsigned));
        std::vector<unsigned> eps_;
        eps_.resize(n_ep);
        this->inGraph.read((char *)eps_.data(), n_ep * sizeof(unsigned));

        this->startReadPoint = this->inGraph.tellg(); 

        {   //// statistics max_edges. 
            unsigned max_size_g{0};
            unsigned size_g{0};

            for (size_t i = 0; i < this->dataSize; ++i) {
                this->inGraph.read((char *)&size_g, sizeof(unsigned int));
                ++this->graphSize;
                max_size_g = std::max(max_size_g, size_g);
                this->inGraph.seekg(size_g * sizeof(Idx_Type), std::ios::cur);
            }
            
            this->inGraph.clear();
            this->maxEdges = static_cast<size_t>(max_size_g);
        }

        // std::cout << "load graph: " << this->graphSize << ", max_edge = " << this->maxEdges << "\n";
        this->inGraph.clear();
        this->inGraph.seekg(this->startReadPoint, std::ios::beg);

        if (this->inGraph.is_open()) {  
            if (!this->inGraph.good()) {  
                std::cerr << "Error: " << this->inGraph.rdstate() << std::endl;  
            }  
        }
    }
}; // end of SSGLoader 


template <typename Dat_Type>
class VamanaLoader : public AbstractLoader<Dat_Type> {

public:
    VamanaLoader(std::string base_path, std::string graph_path) : AbstractLoader<Dat_Type>(base_path, graph_path)
    {
        this->load_data(); 
        load_graph();
    }

    void load_graph() {

        this->inGraph.open(this->graphPath, std::ios::binary);    
        
        if (this->inGraph.is_open()) {  
            if (!this->inGraph.good()) {  
                std::cerr << "Error: " << this->inGraph.rdstate() << std::endl;  
            }  
        }

        this->inGraph.seekg(0, std::ios::beg);

        typedef uint64_t _u64;
        typedef uint32_t _u32;

        _u64 file_offset        = 0; // will need this for single file format support
        _u64 expected_file_size = 0;
        _u64 file_frozen_pts    = 0;
        unsigned _max_observed_degree, _start;

        this->inGraph.exceptions(std::ios::badbit | std::ios::failbit);
        this->inGraph.seekg(file_offset, this->inGraph.beg);
        this->inGraph.read((char *)&expected_file_size, sizeof(_u64));
        this->inGraph.read((char *)&_max_observed_degree, sizeof(unsigned));
        this->inGraph.read((char *)&_start, sizeof(unsigned));
        this->inGraph.read((char *)&file_frozen_pts, sizeof(_u64));
        _u64 vamana_metadata_size = sizeof(_u64) + sizeof(_u32) + sizeof(_u32) + sizeof(_u64);

        size_t bytes_read   = vamana_metadata_size;
        size_t cc           = 0;
        unsigned nodes_read = 0;

        this->startReadPoint = this->inGraph.tellg(); 

        {   //// statistics max_edges. 
            unsigned max_size_g{0};
            unsigned size_g{0};

            for (size_t i = 0; i < this->dataSize; ++i) {
                this->inGraph.read((char *)&size_g, sizeof(unsigned int));
                ++this->graphSize;
                max_size_g = std::max(max_size_g, size_g);
                this->inGraph.seekg(size_g * sizeof(Idx_Type), std::ios::cur);
            }

            this->inGraph.clear();
            this->maxEdges = static_cast<size_t>(max_size_g);
        }

        // std::cout << "load graph: " << this->graphSize << ", max_edge = " << this->maxEdges << "\n";
        this->inGraph.clear();
        this->inGraph.seekg(this->startReadPoint, std::ios::beg);
        if (this->inGraph.is_open()) {  
            if (!this->inGraph.good()) {  
                std::cerr << "Error: " << this->inGraph.rdstate() << std::endl;  
            }
        }
    }
}; // end of VamanaLoader 
