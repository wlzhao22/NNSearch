# TSDG
./hnswlib/build/uni_nnsearch \
--base_path=/home/jfwang/data/sift1m/sift1m_base.fvecs \
--query_path=/home/jfwang/data/sift1m/sift1m_query.fvecs \
--gt_path=/home/jfwang/data/sift1m/sift1m_gt.ivecs \
--index_path=/home/jfwang/data/sift1m/sift1m_TSDGed_K200_SavMem_XNDesc_SCG_1.2_4_50.ivecs \
>> ./nns_records/sift1m_TSDG_plus.record

# DPG
./hnswlib/build/uni_nnsearch \
--base_path=/home/jfwang/data/sift1m/sift1m_base.fvecs \
--query_path=/home/jfwang/data/sift1m/sift1m_query.fvecs \
--gt_path=/home/jfwang/data/sift1m/sift1m_gt.ivecs \
--index_path=/home/jfwang/data/sift1m/sift1m_ThinDPG_K200_SavMem_XNDesc_NND.ivecs \
>> ./nns_records/sift1m_DPG.record

# NSG
./hnswlib/build/uni_nnsearch \
--base_path=/home/jfwang/data/sift1m/sift1m_base.fvecs \
--query_path=/home/jfwang/data/sift1m/sift1m_query.fvecs \
--gt_path=/home/jfwang/data/sift1m/sift1m_gt.ivecs \
--index_path=/home/jfwang/data/sift1m/sift1m_K200_SavMem_XNDesc_NND_NSGed_512.nsg \
>> ./nns_records/sift1m_NSG.record

# SSG
./hnswlib/build/uni_nnsearch \
--base_path=/home/jfwang/data/sift1m/sift1m_base.fvecs \
--query_path=/home/jfwang/data/sift1m/sift1m_query.fvecs \
--gt_path=/home/jfwang/data/sift1m/sift1m_gt.ivecs \
--index_path=/home/jfwang/data/sift1m/sift1m_K200_SavMem_XNDesc_NND_SSGed_512.ssg \
>> ./nns_records/sift1m_SSG.record

# Vamana
./hnswlib/build/uni_nnsearch \
--base_path=/home/jfwang/data/sift1m/sift1m_base.fvecs \
--query_path=/home/jfwang/data/sift1m/sift1m_query.fvecs \
--gt_path=/home/jfwang/data/sift1m/sift1m_gt.ivecs \
--index_path=/home/jfwang/data/sift1m/sift1m_index_R50_L512_A1.2  \
>> ./nns_records/sift1m_Vamana.record

#HNSW
./hnswlib/build/uni_nnsearch \
--base_path=/home/jfwang/data/sift1m/sift1m_base.fvecs \
--query_path=/home/jfwang/data/sift1m/sift1m_query.fvecs \
--gt_path=/home/jfwang/data/sift1m/sift1m_gt.ivecs \
--index_path=/home/jfwang/data/sift1m/sift1m_HNSW_M25_EF_512.hnsw  \
>> ./nns_records/sift1m_hnswlib.record

