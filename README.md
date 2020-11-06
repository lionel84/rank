# rank

test

cmake .
cmake --build . 

./main 10

排行榜测试数量： 10
rank_skiplist 插入 cost time = 31 us
rank_rbtree 插入 cost time = 56 us
rank_skiplist 根据排名查询节点 cost time = 3 us
rank_rbtree 根据排名查询节点 cost time = 7 us
rank_skiplist 批量根据排名查询节点 cost time = 5 us
rank_rbtree 批量根据排名查询节点 cost time = 19 us
rank_skiplist 根据uid查询排名 cost time = 23 us
rank_rbtree 根据uid查询排名 cost time = 8 us
rank_skiplist:
 第一名 begin()：uid: 4, 分数: 1650721031
 第一名 getNodeByRank(1)：uid: 4, 分数: 1650721031
 最后一名 back(): uid: 1, 分数: 214073954
 最后一名 getNodeByRank(rank_skiplist.size())：uid: 1, 分数: 214073954
rank_rbtree:
 第一名 begin()：uid: 8, 分数: 1670956371
 第一名 find_by_rank(1): uid: 8, 分数: 1670956371
 最后一名 back()：uid: 0, 分数: 47208798
 最后一名 find_by_rank(rank_rbtree.size())：uid: 0, 分数: 47208798
rank_skiplist 根据uid修改积分 cost time = 75 us
rank_rbtree 根据uid修改积分 cost time = 15 us
rank_skiplist:
 第一名 begin()：uid: 4, 分数: 1285863215
 第一名 getNodeByRank(1)：uid: 4, 分数: 1285863215
 最后一名 back(): uid: 6, 分数: 198807548
 最后一名 getNodeByRank(rank_skiplist.size())：uid: 6, 分数: 198807548
rank_rbtree:
 第一名 begin()：uid: 8, 分数: 1670966371
 第一名 find_by_rank(1): uid: 8, 分数: 1670966371
 最后一名 back()：uid: 0, 分数: 47208798
 最后一名 find_by_rank(rank_rbtree.size())：uid: 0, 分数: 47208798
rank_skiplist 根据uid删除 cost time = 79 us
rank_rbtree 根据uid删除 cost time = 11 us
memory: 0
global_init_count: 41
global_uninit_count: 127
