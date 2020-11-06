/*
 * @Auth: lionelzhang
 * @Date: 2020-11-04 14:30:44
 * @LastEditors: lionelzhang
 * @LastEditTime: 2020-11-06 21:46:47
 * @Description: 
 */
#include "rank_skip_list.h"
#include "rank_rbtree.h"

#include <unistd.h>
#include <vector>

#include "base.h"
namespace nm_rank_test{
using namespace nm_rank_skip_list;
using namespace nm_rank_tree;
using rank_data_key_t = uint64_t;

int global_init_count = 0;
int global_uninit_count = 0;
struct RANK_DATA
{
    RANK_DATA(){ 
        global_init_count ++;
    }
    RANK_DATA(rank_data_key_t uid, int32_t score, int32_t ts): _uid(uid), _score(score), _ts(ts), _name("name"){
        global_init_count ++;
        _level = 1;
        //end[1023] = 0;
    }
    const bool operator < (const RANK_DATA& other) const{
        if(_score < other._score)return true;
        if(_score == other._score){
            return _ts > other._ts;
            if(_ts > other._ts)return true;
            if(_ts == other._ts)
                return _uid < other._uid;
            //return _ts > other._ts;
        }
        return false;
    }

    const bool operator > (const RANK_DATA& other) const{
        if(_score > other._score)return true;
        if(_score == other._score){
            return _ts < other._ts;
            if(_ts < other._ts)return true;
            if(_ts == other._ts)
                return _uid < other._uid;
        }
        return false;
    }
    ~RANK_DATA(){
        global_uninit_count ++;
    }
    rank_data_key_t _uid;
    long _score;
    long _ts;
    std::string _name;
    int32_t _level;
   // char end[1024];
};
using rank_data_value_t = RANK_DATA;
int _test(int nMaxCount){
    srand(time(NULL));
    //vector<std::shared_ptr<rank_data_value_t> > test_data;
    std::vector<rank_data_value_t > test_data;
    std::vector<rank_data_value_t > test_data_for_rb_tree;
    for(int i = 0; i< nMaxCount; i++){
        test_data.push_back(std::move(rank_data_value_t(i,random(), random())));
        test_data_for_rb_tree.push_back(rank_data_value_t(i,random(), random()));
    }

    RankSkipList<rank_data_key_t, rank_data_value_t, std::greater<rank_data_value_t> > rank_skiplist;

    rank_tree<rank_data_key_t, rank_data_value_t, std::greater<rank_data_value_t> > rank_rbtree;
    printnow();
    printf("排行榜测试数量： %d\n", nMaxCount);
    for(const auto& it : test_data){
        rank_skiplist.zslInsert(std::make_pair(it._uid, it));
    }
    printnow("rank_skiplist 插入");
    for(const auto& it : test_data_for_rb_tree){
        rank_rbtree.insert(std::make_pair(it._uid, it));
    }
    printnow("rank_rbtree 插入");
    printnow();
    for(uint64_t i = 0; i< nMaxCount+100; i++){
        auto it = rank_skiplist.getNodeByRank(i);
    }
    printnow("rank_skiplist 根据排名查询节点");
    for(uint64_t i = 0; i<= nMaxCount+100; i++){
        auto it = rank_rbtree.find_by_rank(i); 
    }
    printnow("rank_rbtree 根据排名查询节点");
    printnow();
    for(uint64_t i = 0; i< nMaxCount+100; i++){
        auto it = rank_skiplist.find_by_rank(i, i+10);
    }
    printnow("rank_skiplist 批量根据排名查询节点");
    for(uint64_t i = 0; i<= nMaxCount+100; i++){
        auto it = rank_rbtree.find_by_rank(i, i+10);
    }
    printnow("rank_rbtree 批量根据排名查询节点");
    for(uint64_t i = 0; i< nMaxCount+100; i++){
        auto it = rank_skiplist.getRankByKey(i);
    }
    printnow("rank_skiplist 根据uid查询排名");
    for(uint64_t i = 0; i<= nMaxCount+100; i++){
        auto it = rank_rbtree.rank_by_key(i); 
        //cout<<"rank: "<<rank_tree.rank(it)<<" id: "<<it->second._uid<<" value: "<<it->second._score<<" ts: "<<it->second._ts<<endl;
    }
    printnow("rank_rbtree 根据uid查询排名");
    {
        auto first = rank_skiplist.begin();
        if(first != rank_skiplist.end()){
            printf("rank_skiplist:\n 第一名 begin()：uid: %llu, 分数: %llu\n", first->second._uid, first->second._score);
        }
        auto first_v1 = rank_skiplist.getNodeByRank(1);
        if(first_v1 != rank_skiplist.end()){
            printf(" 第一名 getNodeByRank(1)：uid: %llu, 分数: %llu\n", first_v1->second._uid, first_v1->second._score);
        }
        auto last = rank_skiplist.back();
        if(last != rank_skiplist.end()){
            printf(" 最后一名 back(): uid: %llu, 分数: %llu\n", last->second._uid, last->second._score);
        }
        auto last_v1 = rank_skiplist.getNodeByRank(rank_skiplist.size());
        if(last_v1 != rank_skiplist.end()){
            printf(" 最后一名 getNodeByRank(rank_skiplist.size())：uid: %llu, 分数: %llu\n", last_v1->second._uid, last_v1->second._score);
        }
    }
    {
        for(int i = 0; i< rank_rbtree.size(); i++){
            const auto& it = rank_rbtree[i];
        }
    }
    {
        const auto& first = rank_rbtree.begin();
        if(first != rank_rbtree.end()){
            printf("rank_rbtree:\n 第一名 begin()：uid: %llu, 分数: %llu\n", first->second._uid, first->second._score);
        }
        auto first_v1 = rank_rbtree.find_by_rank(1);
        if(first_v1 != rank_rbtree.end()){
            printf(" 第一名 find_by_rank(1): uid: %llu, 分数: %llu\n", first_v1->second._uid, first_v1->second._score);
        }
        auto last = rank_rbtree.back();
        if(last != rank_rbtree.end()){
            printf(" 最后一名 back()：uid: %llu, 分数: %llu\n", last->second._uid, last->second._score);
        }
        auto last_v1 = rank_rbtree.find_by_rank(rank_rbtree.size());
        if(last_v1 != rank_rbtree.end()){
            printf(" 最后一名 find_by_rank(rank_rbtree.size())：uid: %llu, 分数: %llu\n", last_v1->second._uid, last_v1->second._score);
        }
    }
    //#if 1
    for(uint64_t i = 0; i<nMaxCount; i++){
        auto it = rank_skiplist.getNodeByKey(i); 
        if(it != rank_skiplist.end())
        {
            rank_skiplist.zslDelete(i);
            test_data[i]._score = random();
            rank_skiplist.zslInsert(std::make_pair(i, test_data[i]));
        }
    }
    printnow("rank_skiplist 根据uid修改积分");

    //#else
    for(uint64_t i = 1; i<= nMaxCount+100; i++){
        auto it = rank_rbtree.find_by_key(i); 
        if(it != rank_rbtree.end())
        {
            //auto it_v = *it;
            std::pair<nm_rank_test::rank_data_key_t, nm_rank_test::rank_data_value_t > rank_data = *it;
            rank_rbtree.erase(it);
            rank_data.second._score += 10000; //= random();
            //rank_data->_score = random();
            
            rank_rbtree.insert(rank_data);
        }
        //cout<<"rank: "<<rank_tree.rank(it)<<" id: "<<it->second._uid<<" value: "<<it->second._score<<" ts: "<<it->second._ts<<endl;
    }
    printnow("rank_rbtree 根据uid修改积分");
    {
        auto first = rank_skiplist.begin();
        if(first != rank_skiplist.end()){
            printf("rank_skiplist:\n 第一名 begin()：uid: %llu, 分数: %llu\n", first->second._uid, first->second._score);
        }
        auto first_v1 = rank_skiplist.getNodeByRank(1);
        if(first_v1 != rank_skiplist.end()){
            printf(" 第一名 getNodeByRank(1)：uid: %llu, 分数: %llu\n", first_v1->second._uid, first_v1->second._score);
        }
        auto last = rank_skiplist.back();
        if(last != rank_skiplist.end()){
            printf(" 最后一名 back(): uid: %llu, 分数: %llu\n", last->second._uid, last->second._score);
        }
        auto last_v1 = rank_skiplist.getNodeByRank(rank_skiplist.size());
        if(last_v1 != rank_skiplist.end()){
            printf(" 最后一名 getNodeByRank(rank_skiplist.size())：uid: %llu, 分数: %llu\n", last_v1->second._uid, last_v1->second._score);
        }
    }
    {
        for(int i = 0; i< rank_rbtree.size(); i++){
            const auto& it = rank_rbtree[i];
        }
    }
    {
        const auto& first = rank_rbtree.begin();
        if(first != rank_rbtree.end()){
            printf("rank_rbtree:\n 第一名 begin()：uid: %llu, 分数: %llu\n", first->second._uid, first->second._score);
        }
        auto first_v1 = rank_rbtree.find_by_rank(1);
        if(first_v1 != rank_rbtree.end()){
            printf(" 第一名 find_by_rank(1): uid: %llu, 分数: %llu\n", first_v1->second._uid, first_v1->second._score);
        }
        auto last = rank_rbtree.back();
        if(last != rank_rbtree.end()){
            printf(" 最后一名 back()：uid: %llu, 分数: %llu\n", last->second._uid, last->second._score);
        }
        auto last_v1 = rank_rbtree.find_by_rank(rank_rbtree.size());
        if(last_v1 != rank_rbtree.end()){
            printf(" 最后一名 find_by_rank(rank_rbtree.size())：uid: %llu, 分数: %llu\n", last_v1->second._uid, last_v1->second._score);
        }
    }
   // #endif

    for(int i = 0; i< nMaxCount+100; i++){
        auto it = rank_skiplist.zslDelete(i);
        if(it == false){
            //cout<<i<<endl;
            auto node = rank_skiplist.getNodeByKey(i);
            if(node != rank_skiplist.end()){
                printf("failed \n");
            }
        }
    }
    printnow("rank_skiplist 根据uid删除");

    for(int i = 0; i<= nMaxCount+100; i++){
        auto it = rank_rbtree.find_by_key(i); 
        if(it != rank_rbtree.end())
            rank_rbtree.erase(it);
    }
    printnow("rank_rbtree 根据uid删除");
}

void test(int nMaxCount){
    _test(nMaxCount);
    printf("global_init_count: %d\n", global_init_count);
    printf("global_uninit_count: %d\n", global_uninit_count);
}

}