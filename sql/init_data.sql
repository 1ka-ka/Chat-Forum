USE chatforum;

-- 插入测试用户（密码均为 123456 的 bcrypt 哈希）
-- bcrypt hash for "123456": $2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy
INSERT INTO users (username, password_hash, nickname, avatar_url) VALUES
  ('alice',   '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', '爱丽丝', ''),
  ('bob',     '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', '鲍勃',   ''),
  ('charlie', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', '查理',   '');

-- 插入测试帖子
INSERT INTO posts (user_id, title, content, image_urls, like_count, comment_count) VALUES
  (1, '欢迎来到 ChatForum！', '这是第一个帖子，欢迎大家在这里交流讨论！ChatForum 是一个基于 B/S 架构的聊天论坛系统，支持发帖、评论、点赞和私聊等功能。', NULL, 3, 2),
  (2, 'C++17 新特性分享', '最近在研究 C++17 的新特性，包括 std::optional、std::variant、std::any、结构化绑定、if constexpr 等。大家有什么使用心得吗？', NULL, 2, 1),
  (1, 'Vue 3 Composition API 实践', '在使用 Vue 3 Composition API 开发项目时，发现 ref 和 reactive 的使用场景有很多讲究。分享一些实践经验：对于基本类型用 ref，对象类型用 reactive。', NULL, 1, 1),
  (3, 'Drogon 框架入门指南', 'Drogon 是一个高性能的 C++ Web 框架，支持 HTTP 和 WebSocket。它的异步非阻塞 IO 模型使得处理高并发请求非常高效。推荐大家学习使用！', NULL, 2, 0),
  (2, 'MySQL 索引优化技巧', '最近在优化数据库查询性能，总结了一些索引使用技巧：1. 遵循最左前缀原则 2. 避免在索引列上使用函数 3. 使用覆盖索引减少回表 4. 注意索引的选择性', NULL, 1, 1);

-- 插入测试评论
INSERT INTO comments (post_id, user_id, content) VALUES
  (1, 2, '感谢分享！这个论坛看起来很不错！'),
  (1, 3, '期待更多功能上线！'),
  (2, 1, 'std::optional 确实很实用，避免了使用指针表示可选值的歧义。'),
  (3, 2, '感谢分享！学到了不少。'),
  (5, 1, '索引优化确实很重要，尤其是大数据量的场景。');

-- 插入测试点赞
INSERT INTO likes (post_id, user_id) VALUES
  (1, 2),
  (1, 3),
  (2, 1),
  (2, 3),
  (3, 2),
  (4, 1),
  (4, 2),
  (5, 3);

-- 插入测试私聊消息
INSERT INTO messages (sender_id, receiver_id, content, is_read) VALUES
  (1, 2, '你好，Bob！最近在忙什么？', 1),
  (2, 1, '嗨 Alice！最近在研究 Drogon 框架。', 1),
  (1, 2, '听起来很有趣！能分享一些心得吗？', 0),
  (3, 1, 'Alice，你的帖子写得很好！', 1),
  (1, 3, '谢谢 Charlie！', 0);
