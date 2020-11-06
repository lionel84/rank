/*
 * @Auth: lionelzhang
 * @Date: 2020-10-27 11:07:08
 * @LastEditors: lionelzhang
 * @LastEditTime: 2020-11-06 21:09:17
 * @Description: 
 */
#ifndef __RANK_SKIP_LIST_H__
#define __RANK_SKIP_LIST_H__

#include <unordered_map>
#include <memory>
#include <vector>

#define SKIP_LIST_MEM_MONITER

namespace nm_rank_skip_list {

#define ZSKIPLIST_MAXLEVEL 64 /* Should be enough for 2^64 elements */
#define ZSKIPLIST_P 0.25  
const int magic = (ZSKIPLIST_P * 0xFFFF);

/*
rank_data_key_t : 待排数据的唯一key，一般是玩家ID之类的,需要实现操作符==
rank_data_value_t: 待排数据的类型，所有的东西都在里面
comp_t: default std::less 表示是小的排前面  需要rank_data_value_t里面实现操作符 <
                std::greater 表示大的排前面 需要rank_data_value_t里面实现操作符 >
example: 
    RankSkipList<uint64, USER_DATA, std::greater<USER_DATA> > list; 
*/
template <typename rank_data_key_t, typename rank_data_value_t , typename comp_t = std::less<rank_data_value_t> >
class RankSkipList
{
    using rank_t = size_t ;

    using rank_data_type = std::pair<rank_data_key_t, rank_data_value_t >; 
    // for iterator
    using pointer = rank_data_type*;
    using const_pointer = const rank_data_type*;
    using reference = rank_data_type&;
    using const_reference = const rank_data_type&;

    struct zskiplistNode
    {
        //struct zskiplistNode *backward;
        rank_data_type _rank_data;
        
    #ifdef SKIP_LIST_MEM_MONITER
        size_t _mem;
    #endif
        int32_t _level;
        //每层的节点信息
        struct zskiplistLevel
        {
            zskiplistNode *forward;  //当前层当前节点下一个节点
            zskiplistNode *backward;
            rank_t span;
        } _levels[0]; // 这里按需分配，可以节约极大空间

    #ifdef SKIP_LIST_MEM_MONITER
        zskiplistNode(int mem):_level(1), _mem(mem) {}
        zskiplistNode(int level, int mem, const_reference rank_data):_level(level), _mem(mem), _rank_data(rank_data){}
    #else
        zskiplistNode():_level(1) {}
        zskiplistNode(int level, const_reference rank_data):_level(level), _rank_data(rank_data){}
    #endif
    };
    
    using node_t = zskiplistNode;
    using node_pointer_t = zskiplistNode* ; 

private:
    void _initFreeNodeList(int level, int32_t nCount){
        while(nCount-- > 0){
            _addNodeToFreeList(zslCreateNode(level, nullptr, false));
        }
    }
    void _addNodeToFreeList(node_t* node){
        node->_levels[0].forward = m_freeList[node->_level];
        m_freeList[node->_level] = node;
    }
    node_t* _getNodeFromFreeList(int level){
        node_t* ret = nullptr;
        if(m_freeList[level] != nullptr){
            ret = m_freeList[level];
            m_freeList[level] = m_freeList[level]->_levels[0].forward;
            ret->_levels[0].forward = nullptr;
        }
        return ret;
    }
    /**
     * 注意初始化的时候调表的头部指针已经存在但是长度为0
     */
    void init()
    {
#ifdef SKIP_LIST_MEM_MONITER
        m_memory = 0;
#endif
        m_level = 1;
        m_length = 0;

        for(auto& it: m_freeList){
            it = nullptr;
        }

        m_header = zslCreateNode(ZSKIPLIST_MAXLEVEL , std::make_pair(rank_data_key_t(0), rank_data_value_t())); // new node_t(ZSKIPLIST_MAXLEVEL);
        m_tail = zslCreateNode(ZSKIPLIST_MAXLEVEL , std::make_pair(rank_data_key_t(0), rank_data_value_t()));;
        for (int j = 0; j < ZSKIPLIST_MAXLEVEL; j++) {
            m_header->_levels[j].forward = m_tail;
            m_header->_levels[j].backward = nullptr;
            m_header->_levels[j].span = 0;
            m_tail->_levels[j].forward = nullptr;
            m_tail->_levels[j].backward = m_header;
            m_tail->_levels[j].span = 0;
        }
    }
public:
    RankSkipList()
    {
        init();
    }
    void printlist(){
        for(int32_t i = m_level-1; i>=0; i--){
            node_pointer_t tempnode = m_header->_levels[i].forward;
            int32_t count = 0;
            while(tempnode){
                count ++;
                tempnode = tempnode->_levels[i].forward;
            }
            if(count)
                printf("level: %d, count: %d\n", i, count);
        }

        for(int32_t i = 1; i <= ZSKIPLIST_MAXLEVEL; i++){
            node_pointer_t tempnode = m_freeList[i];
            int32_t count = 0;
            while(tempnode){
                count ++;
                tempnode = tempnode->_levels[0].forward;
            }
            if(count)
                printf("level: %d, free count: %d\n", i, count);
        }
    }
    ~RankSkipList()
    {
        node_t *node = m_header->_levels[0].forward, *next;
        zslFreeNode(m_header, false);
        while(node) {
            next = node->_levels[0].forward;
            zslFreeNode(node, false);
            node = next;
        }
        // free freelist
        for(int i = 1; i<= ZSKIPLIST_MAXLEVEL; i++ ){
            while(m_freeList[i]){
                node_t* temp = m_freeList[i];
                m_freeList[i] = m_freeList[i]->_levels[0].forward;
                zslFreeNode(temp, false);
            }
        }
#ifdef SKIP_LIST_MEM_MONITER
        printf("memory: %llu\n", mem());
#endif
    }
    
    inline uint64_t mem(){
#ifdef SKIP_LIST_MEM_MONITER
        return m_memory;
#else
        return 0;
#endif
    }
    
    static node_pointer_t prev(node_pointer_t h)
    {
        return h->_levels[0].backward;
    }

    static node_pointer_t next(node_pointer_t h)
    {
        return h->_levels[0].forward;
    }
    class iterator
    {
    public:
        iterator(node_pointer_t p):m_p(p){}
        iterator():m_p(nullptr){}
        iterator operator--()
        {
            m_p = prev(m_p);
            return *this;
        }
        iterator operator++()
        {
            m_p = next(m_p);
            return *this;
        }
        iterator operator--(int)
        {
            iterator tmp = *this;
            --*this;
            return tmp;
        }
        iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
        reference operator*()
        {
            return static_cast<node_pointer_t>(m_p)->_rank_data; 
        }
        const_reference operator*() const
        {
            return static_cast<node_pointer_t>(m_p)->_rank_data; 
        }
        pointer operator->()
        {
            return &(static_cast<node_pointer_t>(m_p)->_rank_data); 
        }
        const_pointer operator->() const
        {
            return &(static_cast<node_pointer_t>(m_p)->_rank_data); 
        }
        bool operator==(iterator other) const
        {
            return m_p == other.m_p;
        }
        bool operator !=(iterator other) const
        {
            return !(*this == other);
        }
    private:
        node_pointer_t m_p;
        friend class RankSkipList;
    };

    iterator begin(){
        return iterator(m_header->_levels[0].forward);
    }
    iterator end(){
        return iterator(m_tail);
    }
    iterator back(){
        return m_length == 0 ? iterator(m_tail): iterator(m_tail->_levels[0].backward);
    }
    size_t size(){
        return m_length;
    }
private:
    /**
     * 创建一个节点
     * @param level
     * @param score
     * @param value
     * @return
     */
    node_pointer_t zslCreateNode(int level,const_reference rank_data, bool use_cache = true){
        size_t need_mem = sizeof(node_t)+level*sizeof(struct node_t::zskiplistLevel);

        if(use_cache){
            if(node_pointer_t node =  _getNodeFromFreeList(level)){
                node->_rank_data = rank_data;
                return node;
                //p_m = (void*)node;
            }
        }
        void *p_m = malloc(need_mem);
        m_memory += need_mem;
        if(!p_m)return nullptr;
#ifdef SKIP_LIST_MEM_MONITER
        node_pointer_t p = new(p_m)node_t(level, need_mem, rank_data);
#else
        node_pointer_t p = new(p_m)node_t(level, rank_data);
#endif
        return p;
    }

    void zslFreeNode(node_t*& p, bool use_cache = true){
        if(use_cache){
            _addNodeToFreeList(p);
            p = nullptr;
            return;
        }
#ifdef SKIP_LIST_MEM_MONITER
        m_memory -= p->_mem;
#endif
        if(p == nullptr)return;
        delete p;
        p = nullptr;
    }
 
    //随机跳表的层数
    int zslRandomLevel(void)
    {
        int level = 1;
        while ((random() & 0xFFFF) < magic)
        //while(0x1 == (0x1 & random())) // 四分之一的概率有上一层
            level += 1;
        return (level < ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
    }
public:
    iterator zslInsert(const_reference  v){
        if(m_cache.find(v.first) != m_cache.end()){
            return iterator(nullptr);
        }
        node_t *update[ZSKIPLIST_MAXLEVEL] = {0}; //每一层需要修改的结点,在每层中，新的节点需要插入在该节点的后面
        rank_t rank[ZSKIPLIST_MAXLEVEL];
        node_t *tmpNode;
        int i, level;
    
        tmpNode = m_header;
        for (i = m_level-1; i >= 0; i--)
        {
            rank[i] = i == (m_level-1) ? 0 : rank[i+1];
            //如果当前节点的下一个节点is not null，且他的下一个节点的score小于插入节点的score，或者他们积分相等但是ele小于插入节点的ele，则继续往后找，直到
            //找到条件不满足的跳出循环，则新插入的节点需要插入该层的当前节点后面，把此节点记录到update数组中
            while (tmpNode->_levels[i].forward != m_tail && m_comp(tmpNode->_levels[i].forward->_rank_data.second , v.second))
            {
                rank[i] += tmpNode->_levels[i].span;
                tmpNode = tmpNode->_levels[i].forward;
            }
            update[i] = tmpNode;
        }
    
        //随机层数
        level = zslRandomLevel();
        //如果随机的层数比当前skiplist的层数要大，则补充高出的层的每层update节点信息
        if (level > m_level)
        {
            for (i = m_level; i < level; i++)
            {
                rank[i] = 0;
                update[i] = m_header; //当前节点初始化为header
                update[i]->_levels[i].span = m_length;
            }
            m_level = level;  //更新skiplist的层数
        }
    
        //创建节点
        tmpNode = zslCreateNode(level,v);
        for (i = 0; i < level; i++)
        {
            //修改创建节点tmpNode,需要修改的节点的forward指针(从每层来看上是一个链表的插入操作即把tmpNode插入到update[i]后面)
            update[i]->_levels[i].forward->_levels[i].backward = tmpNode;
            tmpNode->_levels[i].backward = update[i];
            
            tmpNode->_levels[i].forward = update[i]->_levels[i].forward;
            update[i]->_levels[i].forward = tmpNode;
            /* update span covered by update[i] as x is inserted here */
            tmpNode->_levels[i].span = update[i]->_levels[i].span - (rank[0] - rank[i]);
            update[i]->_levels[i].span = (rank[0] - rank[i]) + 1;
        }
        /* increment span for untouched levels */
        for (i = level; i < m_level; i++) {
            update[i]->_levels[i].span++;
        }

        m_length++;
        m_cache.insert(std::make_pair(v.first, tmpNode));
        return iterator(tmpNode);
    }

    void _zslDeleteNode(node_t* x, node_t **update){
        int i;
        for (i = 0; i < m_level; i++) {
            if (update[i]->_levels[i].forward == x) {
                update[i]->_levels[i].span += x->_levels[i].span - 1;
                x->_levels[i].forward->_levels[i].backward = x->_levels[i].backward;
                update[i]->_levels[i].forward = x->_levels[i].forward;
            } else {
                update[i]->_levels[i].span -= 1;
            }
        }
        while(m_level > 1 && m_header->_levels[m_level-1].forward == m_header)
            m_level--;
        m_length--;
    }

    bool zslDelete(iterator it){
        if(it == end())return false;
        return zslDelete(it->key());
    }

    void _findUpdateByNode(node_t* node, node_t** update){
        node_t* tmpNode = node;
        int32_t i = 0; 
        tmpNode = node->_levels[0].backward;
        for (i = 0; i < m_level; i++)
        {
            //如果当前节点的下一个节点is not null，且他的下一个节点的score小于插入节点的score，或者他们积分相等但是ele小于插入节点的ele，则继续往后找，直到
            //找到条件不满足的跳出循环，则每层要删除的节点的前一个节点都存在update数组中
            update[i] = tmpNode;
            while(tmpNode->_level < i + 2){
                tmpNode = tmpNode->_levels[i].backward;
            }
            
        }
    }
    
    bool zslDelete(const rank_data_key_t& key){
        auto it = m_cache.find(key);
        if(it == m_cache.end())
            return false;
        node_t *update[ZSKIPLIST_MAXLEVEL] = {0}; //每一层需要修改的结点,在每层中，新的节点需要插入在该节点的后面

        _findUpdateByNode(it->second, update);
        _zslDeleteNode(it->second, update);
        m_cache.erase(it);
        zslFreeNode(it->second);
        return true;
#if 0
        node_t * tmpNode;
        int i;
    
        tmpNode = m_header;
        for (i = m_level-1; i >= 0; i--)
        {
            //如果当前节点的下一个节点is not null，且他的下一个节点的score小于插入节点的score，或者他们积分相等但是ele小于插入节点的ele，则继续往后找，直到
            //找到条件不满足的跳出循环，则每层要删除的节点的前一个节点都存在update数组中
            while (tmpNode->_levels[i].forward != m_tail && m_comp(*tmpNode->_levels[i].forward->_rank_data , *rank_data))
            {
                tmpNode = tmpNode->_levels[i].forward;
            }

            update[i] = tmpNode;
        }
    
        tmpNode = tmpNode->_levels[0].forward;
        if(tmpNode == it->second) //&& tmpNode->_rank_data->key() == key)
        {
            _zslDeleteNode(tmpNode, update);
            m_cache.erase(it);
            zslFreeNode(tmpNode);
            return true;
        }
    
        return false; 
        #endif
    }
    
    iterator zslUpdateScore(const_reference v) {
        node_t *x = nullptr;
        auto it = m_cache.find(v.first);
        if(it != m_cache.end()){
            x = it->second;
        }
        
        if(x){
            zslDelete(v.first);
        }
        
        iterator newnode = zslInsert(v);
        if(newnode.m_p){
            m_cache[v.first] = newnode.m_p ;
        }
        return newnode;
    }

    iterator getNodeByKey(const rank_data_key_t& key){
        auto it = m_cache.find(key);
        if(it != m_cache.end()){
            if(it->second){
                return iterator(it->second);
            }
            return iterator(it->second);
        }
        return iterator(m_tail);
    }
    std::vector<iterator> find_by_rank(rank_t rmin, rank_t rmax)
    {
        std::vector<iterator> ret;
        if(rmin > rmax){
            std::swap(rmin, rmax);
        }
        if(rmin < 1)rmin = 1;
        if(rmax > size())rmax = size();
        rank_t i = rmin;
        for(auto it = getNodeByRank(rmin); it != end() && i<= rmax; it++, i++){
            ret.push_back(it);
        }
        return ret;
    }
    /* Finds an element by its rank. The rank argument needs to be 1-based. */
    iterator getNodeByRank(rank_t rank) {
        node_t *x;
        rank_t traversed = 0;
        int i;

        x = m_header;
        for (i = m_level-1; i >= 0; i--) {
            while (x->_levels[i].forward != m_tail && (traversed + x->_levels[i].span) <= rank)
            {
                traversed += x->_levels[i].span;
                x = x->_levels[i].forward;
            }
            if (traversed == rank) {
                return iterator(x);
            }
        }
        return iterator(m_tail);
    }

    rank_t getRankByKey(rank_data_key_t key){
        auto it = m_cache.find(key);
        if(it == m_cache.end())
            return 0;
        const_reference rank_data = it->second->_rank_data;
        
        node_t *x;
        rank_t rank = 0;
        int i;

        x = m_header;
        for (i = m_level-1; i >= 0; i--) {
            while (x->_levels[i].forward != m_tail && m_comp(x->_levels[i].forward->_rank_data.second , rank_data.second )) {
                rank += x->_levels[i].span;
                x = x->_levels[i].forward;
            }
            if(x->_levels[i].forward == it->second){ //x->_levels[i].forward->_rank_data->key() == key){
                rank += x->_levels[i].span;
                //x = x->_levels[i].forward;
                return rank;
            }
        }
        return 0;
    }
    
private:
    //skiplist_t mskiplist;
    std::unordered_map<rank_data_key_t, node_pointer_t> m_cache;
    comp_t  m_comp;
#ifdef SKIP_LIST_MEM_MONITER
    uint64_t m_memory;
#endif
    node_t *m_header, *m_tail; //header和tail分别指向头结点和尾结点，
    rank_t m_length; //结点数量，
    int m_level; //level为表中结点的最高等级。
    node_t *m_freeList[ZSKIPLIST_MAXLEVEL+1];
};
 
void test(int count = 100000);
}

#endif